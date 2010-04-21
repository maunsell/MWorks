function cortex_data = cortex_read_fields (ctx_filename, options, stop_after_trial)
%CORTEX_READ_FIELDS (ps-utils): read cortex file into matlab data.
%   CORTEX_DATA = CORTEX_READ_FIELDS (CTX_FILENAME, OPTIONS)
%
%   This routine reads a Cortex data file and returns this information
%   as a structure.  If the input file is omitted, the user will be
%   prompted for a file to select.
%
%   cortex_data is the data in a structure: each field is a vector
%   or in the case of the data buffers (e.g. isi,code,eog,epp), a
%   cell vector.  Each row of the vector corresponds to a trial.
%  
%   options is a cell array of strings.
%    string in array         means
%      'header'                Get header info 
%      'codes'                 Get code and code time data (includes
%                                spike codes)
%      'eog'                   Get eog data
%      'epp'                   Get epp data
%      'all'                   Get all data (default)
%    The above can be combined to get e.g. header and eog data but
%    not epp or codes.
%
%   NOTES
%    Differences from cortex_read.m:
%      Breaks down the header into fields
%      You can choose to exclude certain data.
%
%    Can take up a lot of memory: make sure you have enough virtual
%    memory.  (approx 4x the size of your cortex file; in the worst
%    case of large cortex files, eog/epp data dominates and those 2
%    byte ints are stored in matlab 8-byte doubles.)  We'd rather let
%    the os handle the swapping to disk then do it manually.
%
%    Structure returned has the following fields:
%     option    fields
%     header    cond_no, repeat_no, trial_no, eog_sampling_rate_Hz,
%               kHz_resolution, expected_response, response, response_error,
%               total_trials
%     codes     codes, code_times
%     eog       eog0, eog1
%     epp       epp0, epp1
%     Note that the codes and code_times fields include both cortex encodes and
%     spike codes.
%
%     Note that cond_no, repeat_no, trial_no, block_no are stored on disk
%     starting from 0.  this function adds 1 so the first condition (e.g.)  is
%     numbered 1, not 0.
%
%$Id: cortex_read_fields.m 125 2008-03-20 20:19:22Z vincent $

file=ctx_filename;

% Process options

if nargin < 2 || isempty(options), options = 'all'; end
if nargin < 3, stop_after_trial = Inf; end

[keep_eog keep_epp keep_header keep_codes] ...
    = process_cell_flags ({'eog', 'epp', 'header', 'codes'}, options);

eyedata = {};
eppdata = {};

fid1 = fopen(file, 'r');
assert (fid1 ~= -1, 'Cortex file not found.');

fseek(fid1, 0, 1);
endfile = ftell(fid1);
fseek(fid1, 0, -1);

trial = 0;
while 1   % break when og
   trial = trial + 1;

   if trial > stop_after_trial
       % asking for truncation
       break;
   end

   header = fread(fid1, 9, 'uint16');
   if feof (fid1); break; end     %else keep going.
   header(10:11) = fread(fid1, 2, 'uint8');
   header(12:14) = fread(fid1, 3, 'uint16');

   length(trial) = header(1);
   cond_no(trial) = header(2)+1;    % all the _no fields are numbered
   repeat_no(trial) = header(3)+1;  % by cortex starting at 0
   block_no(trial) = header(4)+1;
   trial_no(trial) = header(5)+1;
   isi_size(trial) = header(6);
   code_size(trial) = header(7);
   eog_size(trial) = header(8);
   epp_size(trial) = header(9);
   eog_ticks_per_sample(trial) = header(10);
   kHz_resolution(trial) = header(11);
   expected_response(trial) = header(12);
   response(trial) = header(13);
   response_error(trial) = header(14);

   if keep_codes
     % Each time-stamp in ctxtimes() is paired with a code in ctxcodes()
     % read in time-stamps:
     ctxtimes{trial} = fread(fid1, isi_size(trial)/4, 'int32');
     % read in behavioral codes
     ctxcodes{trial} = fread(fid1, code_size(trial)/2, 'uint16');
   else
     fseek (fid1, isi_size(trial)+code_size(trial), 'cof');
   end     

   % read epp
   if keep_epp
     if epp_size(trial) > 0 
       eppdata = fread(fid1, epp_size(trial)/2, 'uint16');
       % massage based on cortex's mangling of eppdata
       % channel number in rightmost four bits.
       % So shift right 4 bits by dividing by 2^4, round down, then 
       % subtract 2048.  See EPP_CONV in
       % cortex/source/include/device.h

       %%DEBUG eppchan = bitand (eppdata, 15);
       eppdata = floor (eppdata ./ (2^4)) - 2048; 
     else
       eppdata = [];
     end
     
   %%DEBUG epp_channel_number{trial} = eppchan;
     epp0{trial} = eppdata(1:2:end);
     epp1{trial} = eppdata(2:2:end);
   else
     %skip epp
     fseek (fid1, epp_size(trial), 'cof');
   end
   
   % read eog data
   if keep_eog
     if eog_size(trial) > 0,
       eyedata = fread(fid1, eog_size(trial)/2, 'int16');
     else
       eyedata = [];
     end

     eog0{trial} = eyedata(1:2:end);
     eog1{trial} = eyedata(2:2:end);
   else   % skip entirely
     fseek (fid1, eog_size(trial), 'cof');
   end
   
end

fclose(fid1);

%%% Massage data that we've read

if keep_header
  % massage kHz_resolution
  zero_res = find (kHz_resolution == 0);
  if ~isempty (zero_res)
    kHz_resolution(zero_res) = 1; % if 0 means Auto selected and running at
    % 1kHz.  If running at 10kHz this is ALWAYS 10.
  end
  if any(eog_ticks_per_sample < 1 | eog_ticks_per_sample > 32)
      eog_ticks_per_sample
      error('Invalid eog_ticks_per_sample, displayed above');
  end
  eog_sampling_rate_Hz = kHz_resolution .* 1000 ./ ...
      eog_ticks_per_sample;
  
  total_trials = trial - 1;
end

% check to see if there is any eog/epp data
if (all(cellfun('isempty', epp0)) & all (cellfun('isempty', epp1)))
   keep_epp = 0;
end
if (all(cellfun('isempty', eog0)) & all (cellfun('isempty', eog1)))
   keep_eog = 0;
end




%%%%%%%%% Build structure

% Don't put eog fields in structure if no eog data wanted.
if ~keep_eog
   eog0 = [];
   eog1 = [];
end

% Similarly with epp fields...
if ~keep_epp
   epp0 = [];
   epp1 = [];
end

if ~keep_codes
   ctxcodes = [];
   ctxtimes = [];
end

if ~keep_header
   cond_no=[]; repeat_no=[]; block_no=[]; trial_no=[]; 
   eog_sampling_rate_Hz=[]; kHz_resolution=[]; expected_response=[];
   response=[]; response_error=[]; total_trials=[];
end

cortex_data = struct ('file', file, ...
                      'cond_no', cond_no, ...
                      'repeat_no', repeat_no, ...
                      'block_no', block_no, ...
                      'trial_no', trial_no, ...
                      ... % 'isi_size', isi_size, 
                      ... % 'code_size', code_size, 
                      ... % 'eog_size', eog_size,
                      ... % 'epp_size', epp_size,
                      'eog_sampling_rate_Hz', eog_sampling_rate_Hz, ...
                      'kHz_resolution', kHz_resolution, ...
                      'expected_response', expected_response, ...
                      'response', response, ...
                      'response_error', response_error, ...
                      'total_trials', total_trials, ...
                      'codes', {ctxcodes}, ...
                      'code_times', {ctxtimes}, ...
                      'eog0', {eog0}, ...
                      'eog1', {eog1}, ...                          
                      'epp0', {epp0}, ...
                      'epp1', {epp1});                                                  





                  












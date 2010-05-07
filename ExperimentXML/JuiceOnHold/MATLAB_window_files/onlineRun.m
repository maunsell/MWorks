function [retval] = onlineRun(data_struct, input)

% This function processes event data and saves variables in input.
%
% Then it calls plot functions in a
% try-catch block so that errors in plot functions don't affect
% variable saving.
%  MH 100115
% $Id$

beep off;  % otherwise played through speakers to animal!
format compact; 

ds = data_struct;

% debug
%dumpEvents(ds); 

% First call after pressing "play" in client
if nargin == 1
    % init input
    disp('First trial, initializing input');
    input.count = 0;
    input.trialSinceReset = 1;
    input.reactTimesMs = {};
    input.holdTimesMs = {};
    input.tooFastTimeMs = [];
    input.trialOutcomeCell = {};
    input.holdStartsMs = {};
    input.juiceTimesMsCell = {};
else
  input.trialSinceReset = input.trialSinceReset+1;
end

%% look for constants, save if they are there
tooFastTimeMs = mwGetEventValue(ds.events, ds.event_codec, ...
                                 'tooFastTimeMs', [], 'ignoreMissing');
if ~isempty(tooFastTimeMs)
  input.tooFastTimeMs = tooFastTimeMs;
end

%% process corrects
if ~isempty(mwGetEventValue(ds.events, ds.event_codec, ...
                             'success', [], 'ignoreMissing'))
  input.trialOutcomeCell{end+1} = 'success';
elseif ~isempty(mwGetEventValue(ds.events, ds.event_codec, ...
                               'failure', [], 'ignoreMissing'))
  input.trialOutcomeCell{end+1} = 'failure';  
elseif ~isempty(mwGetEventValue(ds.events, ds.event_codec, ...
                               'ignore', [], 'ignoreMissing'))  
  input.trialOutcomeCell{end+1} = 'ignore';    
else
  disp('Error!  Missing trial outcome variable this trial');
  input.trialOutcomeCell{end+1} = 'error-missing';      
end

thisTrialN = length(input.trialOutcomeCell);

%% process reaction times for this trial
codes = [ds.events.event_code];

stimOnUs = mwGetEventTime(ds.events, ds.event_codec, 'stimulusOn', 1);
totalHoldTimeMs = mwGetEventValue(ds.events, ds.event_codec, 'tTotalReqHoldTimeMs');
%trialStartUs = mwGetEventTime(ds.events, ds.event_codec, 'trialStart', 1);
leverDownUs = mwGetEventTime(ds.events, ds.event_codec, 'leverResult', 1, 1);
leverUpUs = mwGetEventTime(ds.events, ds.event_codec, 'leverResult', 2, 0);

disp(totalHoldTimeMs); 

%reactTimeMs = (leverUpUs - stimOnUs) / 1000;
holdTimeMs = (leverUpUs - leverDownUs) / 1000;
reactTimeMs = (holdTimeMs - totalHoldTimeMs);

% add to array
input.holdStartsMs{end+1} = leverDownUs/1000;
input.holdTimesMs{end+1} = holdTimeMs;
input.reactTimesMs{end+1} = reactTimeMs;

% total reward times
juiceAmtsUs = mwGetEventValue(ds.events, ds.event_codec, 'juice', 'all');
juiceAmtsMs = juiceAmtsUs(juiceAmtsUs~=0)/1000;
input.juiceTimesMsCell{thisTrialN} = juiceAmtsMs;


%% run subfunctions
try
  addpath('/Users/histed/MonkeyWorks-trunk-git/MatlabToolbox/tools-mh');
  
  input = saveMatlabState(data_struct, input);
  
  plotOnlineHist(data_struct, input);

  input = testUpload(data_struct, input);

catch ex
  disp('??? Error in subfunction; still saving variables for next trial')
  printErrorStack(ex);
end


%% save variables for next trial
retval = input;

return


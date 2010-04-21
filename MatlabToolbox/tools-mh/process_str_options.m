function opt_struct = process_str_options(default_cell, option_cell)
%process_str_options (ps-utils): deal with options like Matlab's get/set do
%
% USAGE
%  function opt_struct = process_str_options(default_cell, option_cell)
%
%  default_cell is a cell array with options and their default
%    values in the style of get/set: 
%    { 'OptName1', 'OptValue1', 'Name2', 'Val2', ... }
%    Names must be plain strings, values may be anything, including 
%      cell arrays.
%    This must contain all possible options and their default values.
%  option_cell may be in one of two forms:
%     {'name', 'value', etc...}         % labeled form
%   or{'value', 'value', 'value', etc.} % ordered form
%     For labeled form, order is not important.
%     For ordered form, the order of the values corresponds to the order of
%     options specified in default_cell.  Not all options need to be
%     specified, if some are missing at the end, they'll be filled in with
%     defaults.  Of course you can't just leave out options in the middle,
%     that's what labeled form is for.
%  
%  This function replaces any missing values in option_cell with values from 
%    default_cell and then puts the options into a structure whose field
%    names correspond to option names and field values to option values.
%  All names are case-insensitive
%
% EXAMPLE
%  >> default_cell = ...
%  >>    {'filename', '/tmp/foo', 'experiment_name', 'deep', 'no', 1};
%  >> option_cell = {'FileName', '/tmp/bar'};
%  >> s = process_str_options(default_cell, option_cell)
%  s = 
%           filename: '/tmp/bar'
%    experiment_name: 'deep'
%                 no: 1
%
%$Id: process_str_options.m 125 2008-03-20 20:19:22Z vincent $

% could check for repeated options (would be error) but don't
% user can figure out the confusing error message themselves if they repeat opts

defs = str_options2struct(default_cell);

% exit immediately in trivial case
if nargin < 2 | isempty(option_cell)
   opt_struct = defs;
   return
end

if ~isfield (defs, option_cell{1})
   % this is the ordered form, convert to labeled
   numopts = length(option_cell);  
   labeled_opts = cell(numopts,1);
   labeled_opts(1:2:end) = defnames(1:numopts);   % names
   labeled_opts(2:2:end) = option_cell;           % values
end
opts = str_options2struct(option_cell);

optnames = fieldnames(opts);
for i = 1:length(optnames)
   if isfield (defs, optnames{i})
      defs = setfield(defs, optnames{i}, getfield(opts, optnames{i}));     
   else
      error (sprintf('The option specified: %s, is not allowed.', ...
                     optnames{i}) ...
             );
   end
end
%return
opt_struct = defs;
      

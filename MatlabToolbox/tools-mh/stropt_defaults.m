function opt_struct = stropt_defaults(defaults, options, ignorenodefs, altdefnames)
%STROPT_DEFAULTS (ps-utils): deal with options like Matlab's get/set do
%   OPT_STRUCT = STROPT_DEFAULTS(DEFAULTS, OPTIONS, IGNORENODEFS, ALTDEFNAMES)
%   
%   DEFAULTS: stropt (see STROPT_SET), must contain all possible options and
%   their default values.
%   OPTIONS: stropt.  
%   IGNORENODEFS: char.  If 'ignorenodefs', options without a paired default
%      will be ignored.  If 0/empty/unset, opt names in options but not in
%      defaults will result in an error, to detect option name misspellings.
%   ALTDEFNAMES: stropt, each option name is the same as a name in DEFAULTS,
%      its value is a string or cellstr containing possible alternate
%      spellings for the option name.
%
%   Compatibility options: if any option name in DEFAULTS is a cellstr, the
%   first element in the cellstr is the real option name, and others are
%   compatibility names.  Only one of these names can be specified in
%   OPTIONS, and that name will be converted to the name in DEFAULTS
%  
%   This function replaces any missing values in OPTIONS with values from DEFAULTS
%   (except: If the value of an option in OPTIONS is 'Default', the value from
%   DEFAULTS is used.).  Then options are put into a structure whose field
%   names correspond to option names and field values to option values.
%   All names are case-sensitive
%
%   EXAMPLE
%   >> defaults = ...
%   >>    {'filename', '/tmp/foo', 'experiment_name', 'deep', 'no', 1};
%   >> options = {'FileName', '/tmp/bar'};
%   >> s = stropt_defaults(defaults, options)
%   s = 
%            filename: '/tmp/bar'
%     experiment_name: 'deep'
%                  no: 1
%
%   SEE ALSO: STROPT_SET
%
%$Id: stropt_defaults.m 125 2008-03-20 20:19:22Z vincent $

%% Optimization notes: I built a cached index into real_opt_name because the old
%  for loop was burning a LOT of time.  ISMEMBER is very slow (and prevented the
%  loop from being JIT'ed (MATLAB 6.5).  When possible I should use STRCMP on
%  strings instead.
%  The next thing to do here would be to optimize the main for loop that
%  loops over opt names, but I think that would be hard, and so it's not
%  worth the gain right now

%% argument checking
if nargin < 2 || isempty(options)
    % exit immediately in trivial case
    opt_struct = defaults;
    return
end    
if nargin < 3
    rejectnodefs = 1;
elseif ischar(ignorenodefs) && strcmp('ignorenodefs', ignorenodefs)
    rejectnodefs = 0;
elseif (isempty(ignorenodefs) || ignorenodefs == 0)
    rejectnodefs = 1;
else
    error('IGNORENODEFS must be ''ignorenodefs'', 0, or empty.');
end
if nargin < 4, altdefnames = []; end

if ~iscell(defaults) || ~iscell(options)
    error ('Inputs must be cell arrays');
end
if length(defaults) < length(options)
    error('More opts than defaults: s_defs param order (defs,opts) wrong?');
end


%% process options, merge defaults
% $$$ defnames = stropt_names(defaults);
% $$$ optnames = stropt_names(options);
% $$$ adnames = stropt_names(altdefnames);
defnames = defaults(1:2:end);  % avoid calling stropt_names for speed
optnames = options(1:2:end);
adnames = altdefnames(1:2:end);

% check all names repeats
if ~isempty(adnames), 
    allAltVals = stropt_get(altdefnames, adnames); 
else
    allAltVals = {};
end
allPossNames = cellstr_cat(allAltVals{:}, defnames);  % utils-g func
if length(unique(allPossNames)) ~= length(allPossNames)
    error('Some alternate or normal def names have been repeated!');
end

% iterate over opt names
for i = 1:length(optnames)
    thisoptname = optnames{i};
    fieldval = stropt_get(options, thisoptname);    
    realname = real_opt_name(thisoptname, altdefnames);

    
    if ischar(fieldval) && strcmpi(fieldval, 'default')
        forcedefault = 1;
        warning('''default'' syntax deprecated.  Use ''__default''.');
    elseif ischar(fieldval) && strcmpi(fieldval, '__default')    
        forcedefault = 1;
    else
        forcedefault = 0;
    end
 
    
    if any(strcmp(defnames, realname))
        defaultexists = 1;
    else
        defaultexists = 0;
    end
    
    % note that we now allow 'default' to fall through if ignorenodefs is set.
    % spk_spec calls this fn to merge its default userdefs with the passed in
    % userdefs point was: if the user asked for a default value and there was
    % none specified then error().  but ignorenodefs can be set for merging
    % purposes.  bottom line is that ignorenodefs should NEVER be set when
    % running this on user input.

    if ~defaultexists && rejectnodefs
        error('There is no default by that name (case sensitive): ''%s''', realname);
    elseif defaultexists && forcedefault
        % usage of default value requested and it's already in there, just
        % move on
        continue
    else
        % either defaultexists && ~forcedefault
        % or ~defaultexists && ~rejectnodefs 
        % ... so copy user value in
        defaults = stropt_set(defaults, realname, fieldval);
    end
end

opt_struct = defaults;


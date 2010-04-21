function val = stropt_get (optin, names, ignoremissing)
%STROPT_GET (ps-utils): get opt in stropt by name
%   VAL = STROPT_GET (OPTIN, NAMES, IGNOREMISSING)
%   OPTIN: stropt (a cell array of name/value pairs)
%   NAMES: String with name of option or cellstr with names of several
%   options (in this case VAL is a cell vector)
%   if IGNOREMISSING = 'ignoremissing', 
%       if name not found, 
%          return empty
%       else error
%   Names are case sensitive.
%
%   See also STROPT_SET, STROPT2STRUCT, STROPT_DEFAULTS
%
%$Id: stropt_get.m 319 2008-08-20 22:14:21Z histed $

chkstropt(optin);
if ~( (iscell(names) && all(cellfun('isclass',names,'char'))) ...
      || isstr(names) )
    error('PSUTILS:badparam', 'Name must be a string');
end

if nargin>2
    if strcmp(ignoremissing, 'ignoremissing') == 1
        igmiss = 1;
    elseif isempty(ignoremissing)
        igmiss = 0;
    else
        error('PSUTILS:badparam', ...
              'ignoremissing must be ''ignoremissing'' or empty');
    end
else
    igmiss = 0;
end

tOptinNames = optin(1:2:end);
tOptinVals = optin(2:2:end);
if ischar(names) || length(names) == 1
    % strcmp is a lot faster
    desOptinVals = strcmp(tOptinNames, names);
else
    desOptinVals = ismember(tOptinNames, names);
end

nDesVals = sum(desOptinVals);
val = tOptinVals(desOptinVals);

if nDesVals > 1
    % leave a cell vector to return
    return;
elseif nDesVals == 1
    % just one value requested, no cell needed, return one value
    val = val{1};
    return
elseif nDesVals == 0
    % not found
    if igmiss
        val = [];
        return
    else
        error('PSUTILS:name_not_found', ...
              'Ignoremissing unset, desired stropt name not found: %s', ...
              names);
    end
end





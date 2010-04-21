function optout = stropt_trim(optin, keepnames, ignoremissingstr)
%STROPT_TRIM (ps-utils): remove name/val pairs by specifying names to keep
%   OPTOUT = STROPT_TRIM (OPTIN, KEEPNAMES, IGNOREMISSING)
%   OPTIN: stropt (a cell array of name/value pairs)
%   KEEPNAMES: cellstr with list of names of option/val pairs in OPTIN to
%   copy to OPTOUT
%   if IGNOREMISSING = 'ignoremissing', 
%       if name not found, 
%          return empty
%       else error
%
%   See also STROPT_GET, STROPT_SET, STROPT_NAMES, STROPT2STRUCT, STROPT_DEFAULTS
%
%$Id: stropt_trim.m 125 2008-03-20 20:19:22Z vincent $

chkstropt(optin);

assert(iscellstr(keepnames), 'keepnames must be a cellstr', 'PSUTILS:badparam');

if nargin < 3, ignoremissingstr = []; end
if ~isempty(ignoremissingstr)
    igmiss = true;
    assert(strcmpi(ignoremissingstr, 'ignoremissing'), ...
           'ignoremissing must be ''ignoremissing'' or empty');
else
    igmiss = false;
end

inNames = stropt_names(optin);
optout = {};
for i=1:length(keepnames)
    tKeepname = keepnames{i};
    if any(strcmp(inNames, tKeepname))
        % this keepname is present in optin, set it
        optout = stropt_set(optout, keepnames{i}, ...
                            stropt_get(optin, keepnames{i}));    
    else
        % keepname not present
        if ~igmiss
            error('Ignoremissing unset; a field to keep not found: %s', ...
                  tKeepname);
        end
    end
end




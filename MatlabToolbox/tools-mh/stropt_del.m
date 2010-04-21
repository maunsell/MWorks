function optout = stropt_del (optin, opts_to_del, ignoremissing)
%STROPT_DEL (ps-utils): delete str options 
%   OPTOUT = STROPT_DEL (OPTIN, OPTS_TO_DEL, IGNOREMISSING))
%   OPTIN: stropt (a cell array of name/value pairs)
%   OPTS_TO_DEL: string/cellstr containing names of options to del
%
%   if IGNOREMISSING = 'ignoremissing', 
%       if name not found, 
%          return empty
%       else error
%
%   See also STROPT_GET, STROPT2STRUCT, STROPT_DEFAULTS
%
%$Id: stropt_del.m 125 2008-03-20 20:19:22Z vincent $

if nargin>2, 
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

chkstropt(optin);
if ischar(opts_to_del), opts_to_del = { opts_to_del }; end


% do real processing here

for idel = 1:length(opts_to_del)
    matched = 0;
    for iin = 1:2:length(optin)
        if strcmp(opts_to_del{idel}, optin{iin})
            % kill it
            keepix = ~ismember(1:length(optin), [iin, iin+1]);
            optin = optin(keepix);
            matched = 1;
            break;
        end
    end
    if ~matched
        if igmiss
            continue;
        else
            error('PSUTILS:name_not_found', 'Desired stropt name not found.');
        end
    end
end

optout = optin;


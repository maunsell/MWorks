function optout = stropt_set (optin, varargin)
%STROPT_SET (ps-utils): set options in stropt by using name/value pairs
%   OPTOUT = STROPT_SET (OPTIN, OPTS_TO_SET)
%   OPTIN: stropt (a cell array of name/value pairs)
%   OPTS_TO_SET: stropt containing values in optin to set/change
%   OPTOUT: the result of changing OPTIN with values in OPTS_TO_SET
%
%   OPTOUT = STROPT_SET (OPTIN, NAME, VALUE, ...) also works.
%
%   STROPT FORMATS
%   Options are specified to functions in the style of get/set, as a cell 
%   array of name/value pairs:
%      { 'Name1', 'Value1', 'Name2', 'Val2', ... }
%      Names must be plain strings, values may be anything, including 
%      cell arrays.  Names should be handled case-insensitively.
%     
%   stropt cell arrays can be converted to structures by STROPT2STRUCT, and
%   set with default values by STROPT_DEFAULTS
%
%   See also STROPT_GET, STROPT2STRUCT, STROPT_DEFAULTS
%
%$Id: stropt_set.m 125 2008-03-20 20:19:22Z vincent $

% handle both parameter types
if length(varargin) == 1
    opts_to_set = varargin{1}
else
    opts_to_set = varargin;
end

% now check validity
chkstropt(optin);
chkstropt(opts_to_set);


optInNames = optin(1:2:end);
for iset = 1:2:length(opts_to_set)
    tName = opts_to_set{iset};
    matchIx = strcmp(optInNames, tName);
    matchNameNo = find(matchIx);

    if ~isempty(matchNameNo)
        matchFullNo = 2*(matchNameNo-1) + 1;
        optin(matchFullNo+1) = opts_to_set(iset+1);
    else
        optin(end+1:end+2) = opts_to_set(iset:iset+1);        
    end
end

% $$$     matched = 0;
% $$$     for iin = 1:2:length(optin)
% $$$         if strcmp(opts_to_set{iset}, optin{iin})
% $$$             optin{iin + 1} = opts_to_set{iset+1};
% $$$             matched = 1;
% $$$             break;
% $$$         end
% $$$     end
% $$$     if ~matched
% $$$         optin(end+1:end+2) = opts_to_set(iset:iset+1);
% $$$     end

optout = optin;


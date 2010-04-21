function namelist = stropt_names (optin)
%STROPT_NAMES (ps-utils): list names of options in a stropt cell array
%   NAMELIST = STROPT_NAMES (OPTIN)
%   OPTIN: stropt (a cell array of name/value pairs, see STROPT_SET)
%   NAMELIST: cellstr of all names in OPTIN
%   
%   To get all the values of a stropt cell array, use 
%   STROPT_GET(OPTIN, STROPT_NAMES(OPTIN)).  See help for STROPT_GET
%
%   See also STROPT_SET, STROPT_GET, STROPT2STRUCT, STROPT_DEFAULTS
%
%$Id: stropt_names.m 125 2008-03-20 20:19:22Z vincent $

chkstropt(optin);
namelist = optin(1:2:end);

% checking should be removed for speed
if (length(unique(namelist)) ~= length(namelist))
    error('PSUTILS:badparam', 'No option names can be repeated');
end



function bool = isstropt_name (optin, names)
%ISSTROPT_NAME (ps-utils): does option name exist in stropt?
%   BOOL = ISSTROPT_NAME (OPTIN, NAMES)
%   OPTIN: stropt (a cell array of name/value pairs, see STROPT_SET)
%   NAMES: cellstr of names to check for existence in OPTIN
%
%   See also STROPT_SET, STROPT_GET, STROPT2STRUCT, STROPT_DEFAULTS
%
%$Id: isstropt_name.m 125 2008-03-20 20:19:22Z vincent $

% chkstropt(optin); done in stropt_names
namelist = stropt_names(optin);
bool = ismember(names, namelist);


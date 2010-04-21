function outState = warning_change_default(desStateStr)
%WARNING_CHANGE_DEFAULT (ps-utils) change only the _default_ warning state
%   W = WARNING_CHANGE_DEFAULT(state) 
%   where state is 'on', 'off' and W is a structure as described in HELP
%   WARNING.  
%
%   Note that warning('on') removes any exceptions and sets all warnings to
%   on.  This function allows you to change the default state (the 'all')
%   state while leaving exceptions alone.
%
%   See also WARNING
%
%$Id: warning_change_default.m 125 2008-03-20 20:19:22Z vincent $

w = warning;
if nargout > 0, outState = w; end % support similar syntax to WARNING

idCell = {w.identifier};
allIx = strcmp(idCell, 'all');

w(allIx).state = desStateStr;
warning(w);

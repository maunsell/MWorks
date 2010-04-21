function titlestr = make_title (extraText, mName)
%MAKE_TITLE (stacks_mh): return a string to be used as a plot title
%
%  (note: simplified version of MAKE_PLOT_TITLE (posit)
%
% $Id: make_title.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 2 || isempty(mName)
    mName = caller_mfilename(1, true);
end
mName = strrep(mName, '_', '\_');

titlestr = sprintf('\\bf%s\\rm: %s', mName, extraText);


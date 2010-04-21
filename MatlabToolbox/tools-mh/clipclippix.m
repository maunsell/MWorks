function clipclippix(figH, pixSize)
%CLIPCLIPPIX (tools): snapshot curr fig by literal pixel screenshot
%   clipclippix(figH, tPixSize)
% 
%   This is needed for cases where figures include transparency (due to
%   matlab bug).  Saved to png file on disk.
%
%   pixSize is the size the figure is saved to; by default 800x600
%
%   See also CLIPCLIP, CLIPCLIPIMAGE
%
%$Id: clipclippix.m 267 2008-06-20 19:16:59Z histed $

if nargin < 1 || isempty(figH), figH = gcf; end
if nargin < 2 || isempty(pixSize), pixSize = [800 600]; end

fName = [mfilename '-' datestr(now)];
fName = strrep(fName, ' ', '_');
fName = strrep(fName, ':', '-');

%% set paths
% file export path
%fPath = '~/shared/snapshots';
%fPath = 'i:/users/histed/snapshots';
dirs = directories;
assert(isfield(dirs, 'toolsSnapshots'), ....
       'needed dir entry is missing, edit directories.m');

fullF = fullfileMH(dirs.toolsSnapshots, [fName '.png']);

%% duplicate figure as modal, grab pix
figH = copyobj(gcf,0);
set(figH, 'WindowStyle', 'modal', ...
          'MenuBar', 'none', ...
          'Position', [50 50 pixSize], ...
          'SelectionHighlight', 'off');
drawnow;
g = getframe(figH);
close(figH);

% save to disk
imwrite(g.cdata, fullF);


function clipclipimage(axH)
%CLIPCLIPIMAGE (tools): copy image in current axes to a file
%
%   This is different from CLIPCLIPPIX because it saves only the image,
%   not a pixel screenshot of the entire figure including labels, etc.
%   Note it only saves the topmost image, not all images in the axes.
%
%   See also CLIPCLIP, CLIPCLIPPIX
%
%$Id: clipclipimage.m 267 2008-06-20 19:16:59Z histed $

if nargin < 1, axH = gca; end

%% set paths
% first file export path
%fPath = 'm:/shared/snapshots';
dirs = directories;
fPath = dirs.toolsSnapshots;
fName = [mfilename '-' datestr(now)];
fName = strrep(fName, ' ', '_');
fName = strrep(fName, ':', '-');
fullF = fullfileMH(fPath, fName);

%%% must create a new figure to make sure it's the right size during
%%% screenshot
xLim = get(axH, 'XLim');
yLim = get(axH, 'YLim');
xSize = range(xLim);
ySize = range(yLim);

% copy and remove children, leaving props intact
newFigH = copyobj(get(axH, 'Parent'), 0);
delete(get(newFigH, 'Children'));
set(newFigH, 'WindowStyle', 'modal', ...
             'Units', 'pixels', ...
             'Position', [0 0 xSize+100 ySize+100], ...
             'Resize', 'off');


% copy axes
newAxH = copyobj(axH, newFigH);
set(newAxH, 'Units', 'pixels', ...
            'Position', [50 50 xSize ySize], ...
            'Box', 'off', ...
            'TickDir', 'out', ...
            'Selected', 'off');

% crazy bs: if any are docked, dock this one?
% $$$ aFH = findobj(0, 'Type', 'Figure', 'WindowStyle', 'docked');
% $$$ if ~isempty(aFH)
% $$$     % dock this one
% $$$     %set(newFigH, 'WindowStyle', 'docked');
% $$$ end

%% grab the frame
drawnow;
m = getframe(newAxH);

imwrite(m.cdata, [fullF '.png'], 'png');

close(newFigH);

%unix(sprintf('convert %s %s', [fullF '.png'], [fullF '.wmf']));

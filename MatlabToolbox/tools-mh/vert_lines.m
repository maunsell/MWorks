function lineHandles = vert_lines (xvals, varargin)
%VERT_LINES (ps-utils) draw vertical lines on plot
%   lineHandles = VERT_LINES(xvals, linePlotParams)
%
%   No Alpha (LINE doesn't have an alpha property) as a
%   line should always be just a few pixels wide.  If you need alpha, 
%   use VERT_SHADE
%
%   VERT_LINES('update', axesH) updates the length of the lines (if the YLim
%   of the axes change)
%   YLimits 
%
%$Id: vert_lines.m 125 2008-03-20 20:19:22Z vincent $

if isstr(xvals) 
    % 'update' case
    assert(strcmp(xvals, 'update'), ...
           'Expected ''update'' as the first arg if a string');
    if nargin < 2, 
        axesH = gca;
    else
        axesH = varargin{1};
    end
    
    % update current plot
    nAxes = length(axesH);
    lineHandles = [];
    for iA=1:nAxes
        vH = findobj(axesH(iA), 'Tag', 'VerticalLines');
        yLim = get(axesH(iA), 'YLim');
        set(vH, 'YData', yLim);
        lineHandles = cat(1, lineHandles, vH);
    end
    return
end

%%% Normal syntax
if nargin < 2
    varargin = {'k'};
end

yLim = get(gca, 'YLim');
ymin = yLim(1); ymax = yLim(2);
hold on;

numx=length(xvals);
lineHandles=repmat(NaN,numx,1); % use NaN so bugs will throw an error
                                % note handle vectors are column vectors
for i=1:numx
   lineHandles(i)=plot([xvals(i) xvals(i)], [ymin ymax], varargin{:});
end
set(lineHandles, 'Tag', 'VerticalLines');


% $$$ %%% Draw a new axes on top of the old one
% $$$ ymin = -1; 
% $$$ ymax = 1;
% $$$ 
% $$$ currentAxisH = gca;
% $$$ 
% $$$ % draw vertical lines
% $$$ vertAxisH = axes('Position', get(currentAxisH, 'Position'), ...
% $$$                  'Visible', 'off', ...
% $$$                  'XLim', get(currentAxisH, 'XLim'), ...
% $$$                  'YLimMode', 'manual', ...
% $$$                  'YLim', [ymin ymax], ...
% $$$                  'Tag', 'VerticalLines');
% $$$ hold on;
% $$$ 
% $$$ % Don't have to worry about stacking order: the new axis will always be
% $$$ % created after currentAxisH
% $$$ 
% $$$ numx=length(xvals);
% $$$ for i=1:numx
% $$$    lineHandles(i)=plot([xvals(i) xvals(i)], [ymin ymax], varargin{:});
% $$$ end
% $$$ 
% $$$ % set the current axis back
% $$$ set(gcf, 'CurrentAxes', currentAxisH);

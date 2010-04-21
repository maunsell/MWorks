function textH = suptitle2(str) 
%SUPTITLE2 (ps-utils): title over all subplots; improved version (2)
%   SUPTITLE2('text') adds text to the top of the figure
%   above all subplots (a "super title"). 
% 
%   This is much simpler than the old SUPTITLE because it uses the new
%   OuterPosition property.  It also does not have the side effect of forcing a
%   screen redraw.
%
% $Id: suptitle2.m 125 2008-03-20 20:19:22Z vincent $

suptitleNormHeight = 0.06;

% save old axes
savedEntryAxesH = gca;

% adjust existing subplots
subH = findobj(gcf, 'Type', 'axes');
if any(strcmp('Colorbar', get(subH, 'Tag')))
    % this function needs to be updated to handle colorbars.  The problem is
    % that matlab's automatic colorbar positioning code is very complicated, 
    % resizes the axis, and it's hard to guess what it's going to do.  It
    % might be possible to call colorbar in some way to get it to update the
    % axis position-- but I couldn't figure out an easy way to do this.
    error('Draw colorbars after calling suptitle2');
end
nSubAxes = length(subH);
for iA = 1:nSubAxes
    oPos = get(subH(iA), 'OuterPosition');
    newPos = oPos;
    newPos([2 4]) = newPos([2 4]) ...
                    * (1-suptitleNormHeight); % shrink along y-axis
    set(subH(iA), 'OuterPosition', newPos);
end

invisH = axes('Units', 'normalized', ...
              'Position', [0 0 1 1], ...
              'Visible', 'off', ...
              'Tag', 'suptitle2');
textH = text(0.5, 1-(suptitleNormHeight*0.6), str, ...
             'HorizontalAlignment', 'center', ...
             'VerticalAlignment', 'middle', ...
             'Tag', 'suptitle2');

% restore old axis, without using a drawnow
set(gcf, 'CurrentAxes', savedEntryAxesH);

%%debug: set(gcf, 'Visible', 'off'); keyboard



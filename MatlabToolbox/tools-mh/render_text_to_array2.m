function outImg = render_text_to_array2(tString, imgSize, backgroundColor, ...
                                        pixStart, ...
                                        varargin)
%RENDER_TEXT_TO_ARRAY2 (ps-utils): make an image with text in it
%
% outImg = render_text_to_array(tString, ...)
%
% Arguments to TEXT go after the first argument.  
% outImg is a logical matrix
%  
% render_text_to_array2('35uA', [256 256], 'k', [10 10], ...
%                       'FontUnits', 'pixels', ...
%                       'FontName', 'Arial', ...
%                       'FontSize', 30, ...
%                       'VerticalAlignment', 'bottom', ...
%                       'HorizontalAlignment', 'left');
%  
% $Id: render_text_to_array2.m 125 2008-03-20 20:19:22Z vincent $

%%% check mem cache %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
cacheKey = {mfilename, tString, imgSize, backgroundColor, pixStart, ...
            varargin}; 
outdatedKey = {};
cDat = memory_cache('get', cacheKey, 'allowMissing', outdatedKey);
if ~isempty(cDat), [outImg] = deal(cDat{:}); return; end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

if nargin < 3, backgroundColor = 'k'; end
if nargin < 4 || isempty(pixStart), pixStart = [0 0]; end

textProps = varargin;
if isempty(textProps)
    textProps = { 'VerticalAlignment', 'bottom', ...
                  'HorizontalAlignment', 'left', ...
                  'Color', [1 1 1] }; 
end

fsOff = 20;
figSize = [0 fsOff imgSize(1)+fsOff imgSize(2)+fsOff];  % use to avoid
                                                        % window controls
figH = figure('Units', 'pixels', ...
              'Position', figSize, ...
              'WindowStyle', 'modal');
axH = axes('Units', 'pixels', ...
           'Position', [0 0 imgSize(1:2)], ...
           'Color', backgroundColor, ...
           'XLim', [1 imgSize(1)], ...
           'YLim', [1 imgSize(2)], ...
           'DataAspectRatio', [1 1 1], ...
           'PlotBoxAspectRatio', [1 1 1], ...
           'YDir', 'reverse');

set(figH, 'Units', 'pixels');
set(axH, 'Units', 'pixels');

% draw text

% add spaces around text 
tString = sprintf('%s', tString);
tH = text(pixStart(1),pixStart(2), ...
          tString, ...
          'Units', 'pixels', ...
          textProps{:});

% get image by screen capture
drawnow;
pause(0.1);  % stupid matlab bug
r = getframe(axH);
close(figH);

% resize output
outImg = imresize(r.cdata, imgSize, 'bicubic');

% if output is bw, take only first plane
if all(all(range(r.cdata,3) == 0))
    outImg = outImg(:,:,1);
end

%%% store data in cache %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
memory_cache('set', {outImg}, cacheKey,outdatedKey,'allowDups');
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


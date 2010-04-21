function outImg = render_text_to_array(tString, varargin)
%
% outImg = render_text_to_array(tString, ...)
%
%  arguments to TEXT go after the first argument.  
%  
%  example
%    outImg = render_text_to_array('some text', ...
%                                  'FontUnits', 'pixels', ...
%                                  'FontName', 'Arial', ...
%                                  'FontSize', 30);
%  
%  This works by printing to a temporary file and rereading, so it can be
%  slow.  
%
%
% $Id: render_text_to_array.m 125 2008-03-20 20:19:22Z vincent $

%%% check mem cache %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
cacheKey = {mfilename, tString, varargin}; outdatedKey = {};
cDat = memory_cache('get', cacheKey, 'allowMissing', outdatedKey);
if ~isempty(cDat), [outImg] = deal(cDat{:}); return; end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


figH = figure('Visible', 'off', ...
              'MenuBar', 'none', ...
              'WindowStyle', 'normal');
axH = axes('Visible', 'off', ...
           'Position', [0 0 1 1]);

% draw text

% add spaces around text 
tString = sprintf('%s', tString);

tH = text(0.5,0.5,tString, ...
          varargin{:});

% work in pixels from now on
set(figH, 'Units', 'pixels');
set(axH, 'Units', 'pixels');
set(tH, 'Units', 'pixels');

figPos = get(figH, 'Position');
figOrigin = figPos(1:2);

textExtent = get(tH, 'Extent');
textPos = get(tH, 'Position');
textSize = textExtent(3:4);
textStartPos = textPos(1:2) - textExtent(1:2);

% adjust sizes
resolution = 100;
set(tH, 'Position', [textStartPos]);
set(axH, 'Position', [ 0 0 textSize ]);
set(figH, 'Position', [ figOrigin textSize], ...
          'PaperUnits', 'inches', ...
          'PaperPosition', [0 0 textSize./resolution], ...
          'PaperSize', textSize./resolution, ...
          'PaperOrientation', 'portrait');


% print and reload
fName = tempname;
print(figH, '-dtiff', '-r100', fName);
imgF = imread([fName '.tif']);
% collapse to mono bitmap (matlab does no antialiasing))
outImg = squeeze(imgF(:,:,1));

close(figH);

%%% store data in cache %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
memory_cache('set', {outImg}, cacheKey,outdatedKey,'allowDups');
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% $$$ figure;
% $$$ image(outImg);
% $$$ keyboard

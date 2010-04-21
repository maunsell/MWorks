function makeGratingImage(outFileName, pixSize)

if nargin < 1, outFileName = []; end
if nargin < 2, pixSize = []; end

sfPix = 200;
ori = 90;
contrastPct = 70;
sinOrSq = 'sin';

%%%%%%%%%%%%%%%%

if isempty(pixSize)
  % get size from the monitor
  sz = get(0, 'MonitorPositions');
  if size(sz, 1) > 1
    sz = sz(2,:);
  end
  sz = sz(3:4);
else
  sz = pixSize;
end


% make x vector, xy grid
thetas = (0:max(sz)) / sfPix * 2*pi;
[x,y] = meshgrid(thetas, thetas);
img = (cos(y)*contrastPct/100+1)/2;

rotImg = imrotate(img, ori, 'bilinear', 'crop');

figH = figure(1);
rgb = cat(3,rotImg,rotImg,rotImg);
image(rgb);
%colormap gray

if ~isempty(outFileName)
  imwrite(rgb, outFileName);
end

  

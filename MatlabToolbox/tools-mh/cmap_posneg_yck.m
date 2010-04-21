function scaledMap = cmap_posneg_yck(m, zeroAtPos)
%CMAP_POSNEG_RDBU (ca-utils): a colormap to represent pos/neg values
%   yellow, cyan, black
%
%   MH 080501: support rescaling zero point / asymmetric map
%
%$id: cmap_posneg_rdbu.m 200 2006-04-01 00:24:05Z histed $ 

if nargin < 1, m = []; end
if nargin < 2, zeroAtPos = []; end

rgb = [255 255 0; ...
       80 80 0; ...
       0 0 0; ...       
       0 80 80; ...
       0 255 255] ./ 255;
rgb = flipud(rgb); % yellow pos, blue neg

scaledMap = make_posneg_cmap(rgb, m, zeroAtPos);


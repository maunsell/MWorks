function scaledMap = cmap_posneg_rbk_linear(m, zeroAtPos)
%CMAP_POSNEG_RDBU (ca-utils): a colormap to represent pos/neg values
%   black is the middle value
%
%   MH 080501: support rescaling zero point / asymmetric map
%
%$id: cmap_posneg_rdbu.m 200 2006-04-01 00:24:05Z histed $ 

if nargin < 1, m = []; end
if nargin < 2, zeroAtPos = []; end

rgb = [255 0 0; ...
       128 0 0;
       0 0 0; ...
       0 0 128; ...
       0 0 255] ./ 255;
rgb = flipud(rgb); % red pos, blue neg

scaledMap = make_posneg_cmap(rgb, m, zeroAtPos);



function scaledMap = cmap_posneg_rdbu(m, zeroAtPos)
%CMAP_POSNEG_RDBU (ca-utils): a colormap to represent pos/neg values
%   This map is brightest (least color-saturated) at zero and darkest at edges.
%
%   See http://www.personal.psu.edu/faculty/c/a/cab38/ColorBrewer/ColorBrewer.html
%
%   MH 080501: support rescaling zero point / asymmetric map
%              hand-interpolate to 11 elements, not 10
%
%$Id: cmap_posneg_rdbu.m 205 2008-05-02 03:37:09Z histed $ 

if nargin < 1, m = []; end
if nargin < 2, zeroAtPos = []; end

rgb = [ 103     0    31; ...
        171    22    42; ...
        207    82    70; ...
        235   144   114; ...
        249   197   171; ...
        231   224   220; ...
        184   216   233; ...
        122   182   214; ...
         60   138   190; ...
         30    97   165; ...
          5    48    97 ] ./ 255;
rgb = flipud(rgb); % red pos, blue neg

scaledMap = make_posneg_cmap(rgb, m, zeroAtPos);



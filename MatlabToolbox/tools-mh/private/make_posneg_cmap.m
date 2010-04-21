function scaledMap = make_posneg_cmap(rgb, m, zeroAtPos)
%MAKE_POSNEG_CMAP: given a RGB range, make a full colormap
%   Supports rescaling zero point so map is not always symmetric
%
%   Private function; don't call directly
%
%   See also CMAP_POSNEG_RDBU, CMAP_POSNEG_YCK
%
%$id: cmap_posneg_rdbu.m 200 2006-04-01 00:24:05Z histed $ 

if nargin < 2 || isempty(m), m = size(get(gcf, 'colormap'), 1); end
if nargin < 3 || isempty(zeroAtPos), zeroAtPos = (m+1)/2; end 
%   zeroAtPos must be 1-origin for the parameter name to imply a matrix
%   So we need to add one to find the center index.
%   for even m, this will be inbetween indices.  for odd m it will be at
%   an index value

nRgb = size(rgb,1);
assert(mod(nRgb,2) == 1, 'bug: internal rgb table must have odd nRows');
nAbove = (nRgb-1)/2;  

% create the output axis
if zeroAtPos == 1
    outNs = linspace(0,nAbove,m);
elseif zeroAtPos == m
    outNs = linspace(-nAbove,0,m);
else
    outNs = interp1([1 zeroAtPos m], [-nAbove 0 nAbove], 1:m);
end

scaledMap = interp1(-nAbove:1:nAbove, rgb, outNs, 'cubic');



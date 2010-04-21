function [tColor,nearestCmapNum] = next_color_by_order(colorNum, axesH, colorOrder)
%NEXT_COLOR_BY_ORDER (ps-utils): get next color from axes ColorOrder
%   [tColor,nearestCmapNum] = NEXT_COLOR_BY_ORDER(colorNum, axesH)
%
%   Handles cycling through colors properly.
%
%   Can specify your own colorOrder
%
%$Id: next_color_by_order.m 341 2008-09-24 14:45:44Z histed $

if nargin < 2, axesH = gca; end
if nargin < 3, colorOrder = []; end

if isempty(colorOrder)
    colorOrder = get(axesH, 'ColorOrder');
end

nColors = size(colorOrder, 1);

colorIx = mod(colorNum, nColors);
if colorIx == 0; colorIx = nColors; end

tColor = colorOrder(colorIx, :);

% get nearest cmap num if requested
figH = get(gca, 'Parent');
cmap = get(figH, 'Colormap');
nColorsInMap = size(cmap,1);

cmapDists = sum((cmap - repmat(tColor,nColorsInMap,1)) .^ 2, ...
                2);
[crap nearestCmapNum] = min(cmapDists);


    

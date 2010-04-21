function colorOrder = colors_linspace(nDesColors, axesH)
%COLORS_LINSPACE (ps-utils): axes ColorOrder -> lin spaced colors from cmap
%   colorOrder = COLORS_LINSPACE(nDesColors, axesH);
%
%   nDesColors: how many linearly spaced color points do you want?
%   axesH: axes to use.  Default: gca
%   colorOrder: this function sets the ColorOrder property in axesH and
%      optionally returns it in colorOrder.
%
%$Id: colors_linspace.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 2, axesH = gca; end
figH = get(axesH, 'Parent');

the_cmap = get(figH, 'Colormap');
nColorsInMap = size(the_cmap, 1);

colorIx = round(linspace(1, nColorsInMap, nDesColors));

cOrder = the_cmap(colorIx,:);
set(axesH, 'ColorOrder', cOrder);

if nargout > 0
    colorOrder = cOrder;
end

  

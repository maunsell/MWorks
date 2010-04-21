function cmapNum = colormap_get_index(rgb)
%COLORMAP_GET_INDEX (Posit): get index of color in map; add if missing
%   cmapNum = COLORMAP_GET_INDEX(rgb)
%
%   If color is not found in colormap, add it, and return an index which is
%   the last entry in the new colormap.  RGB can be a matrix; the columns are
%   the R,G,B values.
%
%$Id: colormap_get_index.m 125 2008-03-20 20:19:22Z vincent $

rgb = colorspec2rgb(rgb);
if isvector(rgb), rgb = rowvect(rgb); end
assert(size(rgb, 2) == 3, 'rgb input must have 3 columns');

currMap = get(gcf, 'Colormap');
[isMatchIx,cmapNum] = ismember(rgb, currMap, 'rows');

missingIx = cmapNum == 0;
if any(missingIx)
    nColorsInMap = size(currMap,1);
    nMissing = sum(missingIx);
    newMap = cat(1, currMap, rgb(missingIx,1:3));

    cmapNum(missingIx) = (nColorsInMap+1):(nColorsInMap+nMissing);
    
    nColorsInNewMap = size(newMap,1);
    set(gcf, 'Colormap', newMap);
    assert(max(cmapNum) <= nColorsInNewMap);
end




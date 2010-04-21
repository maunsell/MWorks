function h = scatter3_varsize(x, y, z, scalesize, extraparms)
%SCATTER_VARSIZE (ps-utils): scatter plot with replications 
%   H = SCATTER3_VARSIZE(X, Y, Z, SCALESIZE, EXTRAPARMS)
%
%   Draws the marker sizes proportional to the number of occurrances of a
%   given x,y,z point in the X,Y,Z inputs.  H is a vector of handles of PATCH
%   objects.  
%
%   See also: SCATTER3, SCATTER_VARSIZE
%
%$Id: scatter3_varsize.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 4 || isempty(scalesize), scalesize = 3; end
if nargin < 5, extraparms = ''; end
    
[unq unqix] = unique([x(:), y(:), z(:)], 'rows');
sizes = ones(length(unqix),1);

% probably can be optimized by using histc
for i=1:length(unqix)
    sizes(i) = sum( (x == unq(i,1)) & (y == unq(i,2)) & (z == unq(i,3)));
end

hands = scatter3(unq(:,1), unq(:,2), unq(:,3), sizes .* scalesize, extraparms);

if nargout > 0
    h = hands;
end

    

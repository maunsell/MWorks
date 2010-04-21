function scaledMap = cmap_green(m)
%CMAP_GREEN (ps-utils): colormap linear on green values
%
%$Id: cmap_green.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 1, m = size(get(gcf, 'colormap'), 1); end

scaledMap = [zeros(1,m); 0:(m-1); zeros(1,m)]'./(m-1);


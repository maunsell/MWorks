function scaledMap = cmap_blue(m)
%CMAP_GREEN (ps-utils): colormap linear on green values
%
%$Id: cmap_blue.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 1, m = size(get(gcf, 'colormap'), 1); end

scaledMap = [zeros(1,m); zeros(1,m); 0:(m-1)]'./(m-1);


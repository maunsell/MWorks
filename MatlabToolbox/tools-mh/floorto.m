function X = floorto(Xin,n)
%FLOORTO (ps-utils) take floor to n decimal places
%   FLOORTO(X,n) like ROUNDTO but takes the floor rather than rounding
%   e.g. FLOORTO(3.141592,4) returns 3.141500000..
%
%   See also CHOP, ROUNDTO, CEILTO
%
%$Id: floorto.m 125 2008-03-20 20:19:22Z vincent $

X = floor(Xin .* 10.^(n)) ./ 10.^(n);

function X = roundto(Xin,n)
%ROUNDTO (ps-utils) round to n decimal places
%   ROUNDTO(X,n), rounds X to n decimal places
%   e.g. ROUNDTO(3.141592,4) returns 3.141600000..
%
%   See also CHOP, FLOORTO, CEILTO
%
%$Id: roundto.m 125 2008-03-20 20:19:22Z vincent $

X = round(Xin .* 10.^(n)) ./ 10.^(n);

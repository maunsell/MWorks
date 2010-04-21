function y = drange(x, dim)
%DRANGE (ps-utils): range of data along given dimension
%   Y = DRANGE(X, DIM)
%   DIM defaults to 1.  
% 
%$Id: drange.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 2, dim = 1; end
y = max(x,[],dim) - min(x,[],dim);

function y = nansum_dim(x,dim)
%NANSUM_DIM (ps-utils): sum value ignoring NaNs in any dimension
%   Y = NANSUM(X,DIM)
%   Returns the sum, ignoring NaNs. 
%   DIM specifies the dimension to work along.  Defaults to first
%   non-singleton dimension.
%
%   Notes: not needed for matlab 6 (R14)
%
% $Id: nansum_dim.m 125 2008-03-20 20:19:22Z vincent $

x(isnan(x)) = 0;
y = sum(x,dim);


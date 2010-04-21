function [theta_out, r_out] = vect_polar_add(theta_in, r_in, dim)
%VECT_POLAR_ADD (ps-utils): add vectors in polar form
%   [THETA_OUT, R_OUT] = VECT_POLAR_ADD(THETA_IN, R_IN, DIM)
%
%   DIM: dimension to sum over
%
%   THETA_OUT: scalar
%   R_OUT: scalar
%
%$Id: vect_polar_add.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 3, dim = 1; end

% if inputs are vectors, make them column vectors
if isvector(theta_in), theta_in = theta_in(:); end  
if isvector(r_in), r_in = r_in(:); end              

[Xtemp,Ytemp] = pol2cart(theta_in, r_in);
[theta_out,r_out] = cart2pol(sum(Xtemp,dim), sum(Ytemp,dim));

function y = rectify(x)
%RECTIFY (ps-utils): if input < 0, make it equal 0, if positive, return value
%  Y = RECTIFY(X)
% 
%$Id: rectify.m 125 2008-03-20 20:19:22Z vincent $

x(x<0) = 0;
y = x;
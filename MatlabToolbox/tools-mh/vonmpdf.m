function y = vonmpdf(angles, mu, kappa)
%VONMPDF (posit): von Mises density function defined on 0..2*pi
%
%   y = VONMPDF(angles, mu, kappa)
%   mu and angles should be in radians
%   kappa should be >= 0 
%
%   See p36 of Mardia and Jupp (2000)
%
%$Id: vonmpdf.m 125 2008-03-20 20:19:22Z vincent $


if kappa < 0, warning('Kappa should be >0; M(u+pi,kappa) == M(u,-kappa)'); end
if ~ ( (length(mu) == 1 || length(mu) == length(angles)) ...
       && (length(kappa) == 1 || length(kappa) == length(angles)) )
    error('mu and kappa must be scalars or the same length as the angles');
end

    
y = exp(kappa .* cos(angles - mu)) ./ (2.*pi.*besseli(0,kappa)); 



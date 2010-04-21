function [Z,pvalAtZ] = raytestinv(npts, pvalue)
%RAYTESTINV (ps-utils): invert a rayleigh test (give Z such that P(Z)<=P0)
%   [Z,pvalAtZ] = RAYTESTINV(npts, pvalue)
%
%   Very brute force way of inverting a rayleigh test
%
%$Id: raytestinv.m 125 2008-03-20 20:19:22Z vincent $

assert(pvalue <= 0.1 && pvalue > 0, ...
       'PValue must be <0.1 and > 0');

Zrange = 0:30;
nZ = length(Zrange);
for i=1:nZ
    tZ = Zrange(i);
    p(i) = PfromZandN(tZ,npts);
end

pdiff = abs(pvalue-p);
[tMin,minIx] = min(pdiff);

fineZrange = (Zrange(minIx-1)):0.001:(Zrange(minIx+1));
nZ = length(fineZrange);
for i=1:nZ
    tZ = fineZrange(i);
    p(i) = PfromZandN(tZ,npts);
end
pdiff = abs(pvalue-p);
[tMin,minIx] = min(pdiff);

Z=fineZrange(minIx);
pvalAtZ = PfromZandN(Z,npts);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
function p = PfromZandN(Z,n)
p = exp(-Z) * (1 + (2*Z - Z^2) / (4*n) - (24*Z - 132*Z^2 + 76*Z^3 - 9*Z^4) / (288*n^2));


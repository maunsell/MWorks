function m = madm(x)
%MADM (utils-general): Median absolute deviation from median
%   M = MEDIANAD(X)
%   For vector X, M is const*median(abs(x-median(x))).
%   For matrix X, M is a row vector containing the mad over each column.
%   
%   NaNs are _ignored_.
%
%   const = 1.4826 which is a bias correction factor:  The corrected madm is
%   asymptotically 1 for standard normal variables.  (while the uncorrected
%   madm is asymptotically 1/1.4826 = norminv(0.75), because the madm should
%   converge to the 75th percentile of a distribution).  The corrected madm
%   thus approximates the standard deviation of normal rv's.  
%  
%$Id: madm.m 125 2008-03-20 20:19:22Z vincent $

%   Ref: Hampel, F. R., Ronchetti, E. M., Rousseeuw, P. J. and Stahel,
%   W. A. (1986) Robust Statistics.  The Approach Based on Influence
%   Functions. New York: John Wiley and Sons.
%

nxRows = size(x,1);

constant = 1.4826;

med = nanmedian(x);
m = constant * nanmedian(abs(x-repmat(med, nxRows, 1)));



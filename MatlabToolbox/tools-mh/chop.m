function X = chop(Xin,n,unit,chopType)
%CHOP (ps-utils) round to n significant figures
%   CHOP(X,n), rounds X to n sig figs
%   e.g. CHOP(3.141592,5) returns 3.141600000..
%
%   CHOP(X,n,unit) rounds the elements of X to n significant
%   figures whose digits (mantissa) are exactly divisible
%   by unit.
%
%   CHOP(X,n,unit,chopType), where chopType is 'round', 'floor', or 'ceil',
%   rounds in the specified direction.  Default is 'round', nearest value.  
%   'floor' truncates, 'ceil' goes to the nearest larger value
%
%   e.g. CHOP(3.141592,3,5) returns 3.150000000..
%   CHOP(3.141592,3,3) returns 3.150000000..
%   CHOP(3.141592,3,2) returns 3.140000000..
%
%   See also: ROUNDTO
%
%$Id: chop.m 125 2008-03-20 20:19:22Z vincent $

%% orig. modified from Carlos Lopez (clv2clv@adinet.com.uy)
%  http://groups.google.com/groups?hl=en&lr=&ie=UTF-8&safe=off&selm=eeebaab.2%40webx.raydaftYaTP

% Set last sig. fig. rounding to 1 if only two input arguments.
if nargin<3 || isempty(unit), unit=1; end

if nargin < 4, chopType = 'round'; end

% Deal with numbers and numbers <= 0.
X=abs(Xin) + (Xin==0);
[nx,mx] = size(X);
exponent=unit.*((10*ones(nx,mx)).^(floor(log10(X))-n+1));
switch chopType
  case 'round'
    X=round(X./exponent).*exponent;
  case 'ceil'
    X=ceil(X./exponent).*exponent;
  case 'floor'
    X=floor(X./exponent).*exponent;
end
% Put back sign and zeros
X=sign(Xin).*X.*(Xin~=0);

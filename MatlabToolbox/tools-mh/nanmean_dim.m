function y = nanmean_dim(x,dim)
%NANMEAN_DIM (ps-utils): Average or mean value ignoring NaNs in any dimension
%   Y = NANMEAN0(X,DIM)
%   Returns the average, ignoring NaNs.  If all entries being summed over are
%   NaN, returns NaN at that location.
%   DIM specifies the dimension to work along.  Defaults to first
%   non-singleton dimension.
%
% $Id: nanmean_dim.m 125 2008-03-20 20:19:22Z vincent $

if nargin<2, 
  % Determine which dimension SUM will use
  dim = min(find(size(x)~=1));
  if isempty(dim), dim = 1; end
end

% count NaNs
nanbool = isnan(x);
numnans = sum(nanbool,dim);
numentries = size(x,dim);
result_size = size(numnans);
divisor_mat = numentries - numnans;  % divisor_mat same size as y

% Replace with zeroes and sum
x(nanbool) = 0;

wId = 'MATLAB:divideByZero';
oldWarnState = warning('query', wId);
warning('off', wId);
y = sum(x,dim) ./ divisor_mat; % if all entries are NaN, divisor_mat is 0, so
                               % the resulting value is NaN
warning(oldWarnState.state, wId);

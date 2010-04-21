function I = information_discrete(y, x, biascorrect)
%INFORMATION_DISCRETE (ps-utils): estimate mutual information from samples
%   INFORMATION_DISCRETE(Y, X)
%   INFORMATION_DISCRETE(R, S, biascorrect)
%
%   If doing bias correction, Y must be R and X must be S.  If not, 
%   Y and X are interchangeable.
%
%$Id: information_discrete.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 3, biascorrect = 0; end

% Validate inputs
if any(isnan(x)) || any(isnan(y))
    error('No NaN''s allowed in input vectors');
end
if isempty(y) || isempty(x), error('One input argument is empty'); end

y = colvect(y);
x = colvect(x);

% convert values to ordinals.  remember, alphabet does not affect I(X,Y).
[uv_y, ix_in_y, ix_in_uv] = unique(y);
new_alphabet_y = 1:length(uv_y);
y = new_alphabet_y(ix_in_uv);

[uv_x, ix_in_x, ix_in_uv] = unique(x);
new_alphabet_x = 1:length(uv_x);
x = new_alphabet_x(ix_in_uv);

% now compute f_xy
joint = vec2padbycol(y,x);
a = histc(joint, new_alphabet_y);
f_xy = a ./ sum(sum(a));    % normalize to make it an estimate of the joint PMF

% H(y)
f_y = sum(f_xy, 2);
f_y(f_y==0) = 1;  % remove 0 log 0 cases
Hy = -sum(f_y .* log2(f_y));
clear f_y; % so you're not tempted to use it


% H(Y|X)
f_x = sum(f_xy, 1);
card_f_y = size(f_xy, 1);
dezeroed_f_x = f_x;
dezeroed_f_x(f_x == 0) = 1;
f_y1x = f_xy ./ repmat(dezeroed_f_x, card_f_y, 1);
f_y1x(f_y1x==0) = 1;
Hy1x_i = -sum(f_y1x .* log2(f_y1x), 1);
Hy1x = sum(f_x .* Hy1x_i);

% I
I = Hy - Hy1x;


%%%

if biascorrect
    % following Panzeri and Treves (1995)
    %assert(length(uv_x) < length(uv_y));  % x must be stim, y must be resp
    assert(all(colvect(uv_x) == colvect(new_alphabet_x)));
    
    A_r = uv_y;    % actual used alphabet of r
    A_s = uv_x;
    
    assert(all(A_r) >= 0);
    assert(all(A_s) > 0);

    S = length(uv_x);  % assume no missing stimulus bins
    N = length(y);     % count data points

    % compute A_Rs, the actually appearing alphabet of each Rs
    A_Rs = repmat(A_r, 1, length(A_s));
    assert(all(size(A_Rs) == size(f_xy)));
    A_Rs(f_xy==0) = 0;

    % use heuristics to give Rtilde and Rs_tilde
    unused_weight = 4 ./ 3;   % must be >= 1
    
    cardA_Rs = sum(A_Rs ~= 0, 1);  % absolute minimum Rs_tilde
    A_Rs_range = range(A_Rs) + 1;      
    Rs_unused = A_Rs_range - cardA_Rs;

    cardA_r = length(A_r);
    A_r_range = range(A_r) + 1;
    R_unused = A_r_range - cardA_r;

    Rs_tilde = cardA_Rs + Rs_unused .* unused_weight;
    Rtilde = cardA_r + R_unused .* unused_weight;

    bias = 1 ./ (2*N*log(2)) .* ( sum(Rs_tilde) - Rtilde - (S-1));
    
    I = I - bias;
end





% previous swapping code.  note this violates assumptions in bias correction
% procedure. 

% maybe we need to optimize this at some point, right now we're within errors
% caused by caching for my tests.
% $$$ % swap x and y to speed calculation;
% $$$ % matlab works down the columns
% $$$ if length(uv_x) > length(uv_y)
% $$$     % card(X) is bigger than card(Y), so swap
% $$$     temp_x = x;
% $$$     x = y;
% $$$     y = temp_x;
% $$$ end


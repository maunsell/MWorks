function [hand, b0, b1, r2, pval] = scatter_linearfit (xdata, ydata, robust, ...
                                                       plot_params, text_to_add, ...
                                                       line_xdata)
%SCATTER_LINEARFIT (ps-utils): add regression line to scatterplot
%   [HAND, B0, B1, R2] = SCATTER_LINEARFIT (XDATA, YDATA, ROBUST, PLOT_PARAMS)
%   You must draw the scatter plot yourself
%   robust: 'robust', 'leastsq', 
%       or 'orthogonal': to minimize the perpendicular direction
%
%       orthogonal regression uses principal components, and bartlett's
%       test to compute pvalues
% 
%$Id: scatter_linearfit.m 253 2008-06-06 18:38:39Z histed $

if nargin < 3 || isempty(robust), robust = 'leastsq'; end
if nargin < 4 || isempty(plot_params), plot_params = {'k'}; end
if nargin < 5 || isempty(text_to_add), text_to_add = { 'slope', 'offset', 'r^2', 'p' }; end
if nargin < 6 || isempty(line_xdata), line_xdata = [min(xdata) max(xdata)]; end  




xdata = squeeze(xdata);  % scatter.m does this (or at least shows this
ydata = squeeze(ydata);  % behavior),  so we should too

if isvector(xdata), xdata = xdata(:); end  % make sure a col vector
if isvector(ydata), ydata = ydata(:); end  

numx = length(xdata);
X = ones(numx,2);
X(:,2) = xdata;

if ~ischar(robust)
    error(['Robust parameter must be a string.  See help ' ...
           'SCATTER_LINEARFIT']);
end

switch robust
  case 'robust'
    [b,st] = robustfit(X(:,[2:end]), ydata);
    pval = st.p(2);
    r = st.coeffcorr(2);
    
    tss = nansum( st.w .* (ydata - nanmean(ydata)) .^ 2);
    yhat = b(1) + xdata * b(2);
    ess = nansum( st.w .* (ydata - yhat) .^ 2);
    r2 =  1 - ess / tss;
  case 'leastsq'
    [b crap1 crap2 crap3 stats] = regress(ydata,X);
    pval = stats(3);
    r2 = stats(1);
    r = sqrt(r2) .* sign(b(2));
  case 'orthogonal'
    % take out each mean
    xM = mean(xdata);
    yM = mean(ydata);
    yd0 = ydata - yM;
    xd0 = xdata - xM;
    
    X = [xd0 yd0];
    [coeff,score,roots] = princomp(X);
    
    % eq for best fit line
    eig1 = coeff(:,1);
    b(2) = eig1(2)/eig1(1);  % slope of best fit line is given by first
                             % eigenvector direction
    b(1) = yM - b(2)*xM;     % offset given by inverting y = mx+b

    pctVar = roots' ./ sum(roots);
    r2 = pctVar(1);
    
    [ndim,pv] = barttest(X, 0.9999);  % use high alpha, we don't care
                                      % about ndims
    
    pval = pv;
    
otherwise
    error('Invalid value for robust parameter');
end

b0 = b(1);
b1 = b(2);    

%xrange = [min(xdata) max(xdata)];

hand(1) = plot(line_xdata, line_xdata*b1 + b0, plot_params{:});

% label plot
tStr = [];
if any(ismember(text_to_add, 'slope'))
    tStr = [tStr sprintf('\\beta_1 = %g\n', chop(b1,3))];
end
if any(ismember(text_to_add, 'offset'))
    tStr = [tStr sprintf('\\beta_0 = %g\n', chop(b0,3))];
end
if any(ismember(text_to_add, 'r^2'))
    tStr = [tStr sprintf('r^2 = %3.2f\n', r2)];
end
if any(ismember(text_to_add, 'p'))
    tStr = [tStr sprintf('p = %g\n', chop(pval,2))];
end
if any(ismember(text_to_add, 'r'))
    tStr = [tStr sprintf('r = %3.2f\n', sqrt(r2))];
end

hand(2)=text(0,0, tStr);


set(hand(2), 'Units', 'normalized', ...
        'Position', [0.02 0.98], ...
        'VerticalAlignment', 'top', ...
        'HorizontalAlignment', 'left', ...
        'FontSize', 10, ...
        'Tag', 'scatter_linearfit-text');



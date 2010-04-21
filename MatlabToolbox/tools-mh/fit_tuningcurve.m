function outStruct = fit_tuningcurve(varargin)
%FIT_TUNINGCURVE (ps-utils) given data, fit a 1-d curve using opt toolbox
% outStruct = FIT_TUNINGCURVE(STROPTS)
%
% fits a function of form   a * sin(x - theta0) + c
% three free parameters: a (amplitude)
%                        theta0 (pref dir)
%                        c (scalar offset)
% outStruct fields:
%   fitFunctionHandle: a handle to the fit function (takes params x, xdata)
%       See LSQCURVEFIT    
%   fitParamNames: cellstr describing the names of each position in the x
%       parameter vector
%   upperBounds, lowerBounds
%   fitParams: x vector
%
% Do transforms on data before calling this function
% 
%
%$Id: fit_tuningcurve.m 125 2008-03-20 20:19:22Z vincent $

% option processing
udefs = { 'Data', 'error: must specify', ...
          'DirRadians',  'error: must specify', ...
          'FitFunctionName', 'cos', ... % cos, vonm3, vonm4
          'CosLB', [0, 0, 0], ...  
          'CosUB', [2*pi-10*eps, Inf, Inf], ...
          'CosRectifiedLB', [0, 0, 0], ...  
          'CosRectifiedUB', [2*pi-10*eps, Inf, Inf], ...
          'VonM3LB', [0, 0, 0], ...
          'VonM3UB', [2*pi-10*eps, Inf, Inf], ...                    
          'VonM4LB', [0, 0, 0, 10*eps], ...
          'VonM4UB', [2*pi-10*eps, Inf, Inf, Inf ], ... 
          'DoPlot', 0, ...
          'DoTitle', true, ...
          'PlotDataTransformInverse', [], ...
          'OptimDisplay', 'off', ...
          'OptimMaxFunEvals', 10^5, ... % empirically needed; default 100*nP
          'OptimMaxIter', 10^5, ... % emp. needed; def 400
        };

specopts={};
uopts = stropt2struct(stropt_defaults(udefs, varargin));

% parameter checking
assert(length(uopts.Data) == length(uopts.DirRadians));

% get the desired function
switch uopts.FitFunctionName
    case 'cos'
        fitFn = @cosFit;
        LB = uopts.CosLB;
        UB = uopts.CosUB;
        paramNames = { 'theta0', 'amplitude', 'offset' };
    case 'cosRectified'
        fitFn = @cosRectifiedFit;
        LB = uopts.CosRectifiedLB;
        UB = uopts.CosRectifiedUB;
        paramNames = { 'theta0', 'amplitude', 'offset' };
    case 'vonm3'
        fitFn = @vonm3Fit;
        LB = uopts.VonM3LB;
        UB = uopts.VonM3UB;        
        paramNames = { 'mu', 'amplitude', 'offset' };
    case 'vonm4'        
        fitFn = @vonm4Fit;
        LB = uopts.VonM4LB;
        UB = uopts.VonM4UB;        
        paramNames = { 'mu', 'amplitude', 'offset', 'kappa' };        
    otherwise
        error('Invalid FitFunctionName: %s', uopts.FitFunctionName);
end

nParams = length(LB);

os.fitFunctionHandle = fitFn;
os.fitParamNames = paramNames;
os.nParams = nParams;
os.lowerBounds = LB;
os.upperBounds = UB;

if ~ (nParams == length(paramNames)), error('bug'); end

% bail early if inputs are bad
if iswithintol(range(uopts.Data), 0, eps*10^2)
    warning('All inputs are zero; returning NaNs');
    os.fitParams = repmat(NaN, 1, nParams);
    os.SSerr = NaN;
    if uopts.DoPlot
        axH = newplot;
        text(0.5,0.5, 'All input data == 0', ...
             'Units', 'normalized', ...
             'VerticalAlignment', 'middle', ...
             'HorizontalAlignment', 'center');
    end
    outStruct = os;
    return
end

% do optimization 
rates = rowvect(uopts.Data);
angles = rowvect(uopts.DirRadians);
[unqDirs,crap1,grpNums] = unique(uopts.DirRadians);

padrates = vec2padbycol(rates, grpNums);
grpmeans = nanmean_dim(padrates,1);
mu_rates = mean(rates);
[x, maxGrpNum] = max(grpmeans);

% initial values
P0(1) = unqDirs(maxGrpNum);
P0(2) = range(grpmeans);
P0(3) = mu_rates;
switch uopts.FitFunctionName 
    case 'vonm4'
        % need to fill in fourth initial value
        P0(4) = 0.5;
end


op = optimset('Display', uopts.OptimDisplay, ...
              'MaxFunEvals', uopts.OptimMaxFunEvals, ...
              'MaxIter', uopts.OptimMaxIter);

[P, err2, resid, exitflag] ...
    = lsqcurvefit(fitFn, P0, angles, rates, LB, UB, op);
if exitflag <= 0
    first_optim_converged = 0;
else
    first_optim_converged = 1;
end

assert(exitflag == 1, 'exitflag != 1: debug this error');

%%%% deal with angle wrap-around
if abs(P(1) - 2*pi) < 0.1
    theta_orig = P0(1);
    P0(1) = 0; 
    retry_optimization = 1;
elseif abs(P(1) - 0) < 0.1
    theta_orig = P0(1);    
    P0(1) = 2*pi;
    retry_optimization = 1;
else
    retry_optimization = 0;
end

if retry_optimization    
    % redo the optimization here
    if ~ strcmp(uopts.OptimDisplay, 'off')
        disp(sprintf(['Redoing optimization for angle wrap. ' ...
                      'theta_orig: %g, exitflag %g'], ... 
                     theta_orig, exitflag))
    end
    [Pcont, err2cont, residcont, exitflagcont] ...
        = lsqcurvefit(fitFn, P0, angles, rates, LB, UB, op);
    if err2cont < err2
        P = Pcont;
        err2 = err2cont;
        resid = residcont;
    end
else
    % if not retrying, just set the exit flag to the original value
    exitflagcont = exitflag; 
end

% now check exitflags and other outputs
assert(exitflag > 0 || exitflagcont > 0, ...
       'Optimization did not converge');
assert(all(P >= LB)  && all(P <= UB), ...
       'Values outside of bounds?');

% return parameters
os.fitParams = P;
os.SSerr = err2;
outStruct = os;

if uopts.DoPlot
    axesH = newplot;
    hold on;
    
    xs = (-pi/3):0.01:(7/3*pi);
    ys = feval(fitFn, P, xs);
    if ~isempty(uopts.PlotDataTransformInverse)
        ys = feval(uopts.PlotDataTransformInverse, ys);
        tData = feval(uopts.PlotDataTransformInverse, uopts.Data);
        fName = strrep(func2str(uopts.PlotDataTransformInverse), '_', '\_');
        noteStr = sprintf('transf^{-1} applied:  %s', fName);
                          
        text(0.02, 0.98, noteStr, ...
             'Units', 'Normalized', ...
             'HorizontalAlignment', 'left', ...
             'VerticalAlignment', 'top', ...
             'FontSize', 8);
    else
        tData = uopts.Data;
    end

    plot(xs,ys,':', 'Tag', 'FitLine');
    plot(uopts.DirRadians, tData, ...
         'LineStyle', 'none', ...
         'Marker', 'o', ...
         'MarkerFaceColor', 'r', ...
         'Tag', 'FitPoints');
    
    ylabel('');
    %set(gca, 'XTickLabel', num2str(colvect(xs),2));
    yLim = get(gca, 'YLim');
    set(gca, 'XLim', [-(1/6)*pi 2*pi - (1/6)*pi], ...
             'YLim', [0, yLim(2)]);

    if uopts.DoTitle
        titleStr = make_plot_title([], 'PncFileName', 0);
        title(titleStr);
    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function y = cosFit(P, angles)
%P: theta, amp, offset
y = P(2)*(cos(angles-P(1)) + 1) + P(3);

%%%%%%%%%%%%%%%%

function y = cosRectifiedFit(P, angles)
%P: theta, amp, offset
y = P(2)*(cos(angles-P(1))+ 1) + P(3);
y(y<0) = 0;

%%%%%%%%%%%%%%%%


function y = vonm4Fit(P, angles)
% P: [mu, amplitude, offset, kappa]
y = P(2) * vonmpdf(angles, P(1), P(4)) + P(3);

%%%%%%%%%%%%%%%%
function y = vonm3Fit(P, angles)
% P: [mu, amplitude, offset], kappa fixed at 1/2
y = P(2) * vonmpdf(angles, P(1), 0.5) + P(3);





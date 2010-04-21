function output = smooth1(input, kernel, width, sigma, dim)
%SMOOTH1 (posit): smooth along one dimension of a matrix
%   OUTPUT = SMOOTH2(INPUT, KERNEL, WIDTH, SIGMA, DIM)
%
%   Smooths the matrix INPUT with a kernel specified by options
%  
%   KERNEL:  'gauss' or 'uniform'.  Default: 'gauss'
%   WIDTH: Gaussian kernel: specifies the length of the vector of the
%              kernel used to smooth with, in standard deviation units.  
%              This is automatically calculated
%              if empty.  (default: 6: 3 std dev's from mean)
%          Uniform kernel: number of points where kernel is non-zero.
%             SIGMA and WIDTH cannot both be specified 
%             If only SIGMA is specified it is used to compute WIDTH
%   SIGMA: for a Gaussian kernel, the kernel width in standard devs
%          Uniform kernel: specifies the width, as width/sqrt(12) =
%          sigma for the uniform.
%   Returns a matrix the same size as the input.   
%   DBTYPE SMOOTH1 shows some programming notes.
%
%   See also FILTER, CONV, CONVN, SMOOTH, KSDENSITY, SMOOTH2, SMOOTH3
%
%$Id: smooth1.m 125 2008-03-20 20:19:22Z vincent $

%   NOTES
%
%   Old SMOOTH1 defaults were kernel 'gauss', width 6, sigma 10
%
%   This is faster than SpikeTools's SMOOTH since it uses MATLAB's builtin
%   FILTER and SMOOTH (SpikeTools) uses a for loop.
%   Note that the kernel used here is different
%   than that used in SMOOTH and so results will be slightly different.

%% argument processing
if nargin < 2, kernel = 'gauss'; end

% error check
if (nargin >= 2 && isnumeric(kernel))
    error('Invalid kernel argument: note, dim is last arg');
end
if (nargin > 2 && isstr(width) ...
    && any(ismember({'Kernel', 'Sigma', 'Width'}, width)))
    error('smooth1.m arguments have changed, needs updating');
end
if any(isnan(input(:))), 
    warning('NaNs in input will be propagated, you prob. want to take them out');
end

% clean up kernel name
switch lower(kernel)
    case { 'gaussian', 'gauss', 'g' }
        kernel = 'gauss';
    case {'uniform', 'box', 'u', 'b'}
        kernel = 'uniform';
    otherwise
        error('Invalid kernel type %s', kernel');
end
% set sigma/width defaults
switch kernel
    case 'gauss'
        if nargin < 3 || isempty(sigma), error('Specify sigma for gaussian kernel'); end
        if isempty(width), width = 6; end  
    case 'uniform'
        if nargin == 2, error('Specify width/sigma for uniform kernel'); end
        if nargin < 4, sigma = []; end  
end
if nargin < 5
    dim = 1; % smooth down cols by default
    % but special case dim when input is a row vector
    if isvector(input) && size(input, 1) == 1
        dim = 2;
    end
end

%% construct kernel
switch kernel
    case 'gauss'
        kern = kernNorm(sigma, width);
        assert(length(kern) == ceil(width*sigma) ...
               || length(kern) == ceil((width*sigma)-1));
    case 'uniform'
        if isempty(width)
            % use sigma to compute it, and write back to user options
            width = round(sigma * sqrt(12));
            % if even, undersmooth by 1
            if mod(width,2) == 0
                newWidth = width - 1;
                warning('This sigma has even kernel width %d, using width %d', ...
                    width, newWidth);
                width = newWidth;
            end            
        else
            % Width was specified.
            assert(isempty(sigma), ...
                   'Cannot specify both Width and Sigma for uniform kernel');
            if mod(width,2) == 0
                error('Uniform widths must be odd');
            end
        end

        % generate kernel
        kern = ones(width,1) ./ width;
end
kernLen = length(kern);
assert(mod(kernLen, 2) == 1);  % make sure it's odd

if width >= sigma
    warning('width >= sigma, is that what you want? (%g %g)', sigma, width);
end


% we don't check this anymore as severe truncations (width<<4)can cause errors
% $$$ xs = 1:length(kern);
% $$$ assert((round(sqrt(sum(kern.*(xs-mean(xs)).^2))) - uo.Sigma)./uo.Sigma ...
% $$$        < 0.20, ...
% $$$        'Bug: kernel standard dev is not within 20% of uo.Sigma');

% note that if the kernel is not odd, the convolution changes the x values:
% each new x value becomes the average of two old x values.

%%% do convolution using convn

% whip the kernel into the right shape
inputDims = ndims(input);
[nRows,nCols] = size(input);
kernelSize = ones(1,inputDims);
kernelSize(dim) = kernLen;
kern = reshape(kern, kernelSize);

% now pad the input
nPads = (kernLen-1)/2;
for i=1:inputDims
    nThisDim = size(input,i);
    if i==dim
        dimNs{i} = ...
            [repmat(1,[1,nPads]), 1:nThisDim, repmat(nThisDim,[1,nPads])];
    else
        dimNs{i} = 1:nThisDim;
    end
end
padInput = input(dimNs{:});

output = convn(padInput, kern, 'valid');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function out = kernNorm(sigma, width)
numpts = sigma * width;
ptsAboveMean = ceil(numpts/2) - 1;  % add a point if necessary, 1 point for 0
if ptsAboveMean == 0
    error('Width (%g) too small for this sigma (%g)', width, sigma);
end

xp = (-ptsAboveMean:ptsAboveMean) / ptsAboveMean * width/2;
% since xp is in units of standard dev's, 
out = exp(-0.5 * (xp.^2)) ./ (sqrt(2*pi));
out = out/sum(out);  % normalize to 1


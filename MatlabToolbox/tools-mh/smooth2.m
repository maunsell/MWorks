function output = smooth2(input, kernel, width, sigma)
%SMOOTH2 (posit): smooth a 2d matrix along both dimensions
%   OUTPUT = SMOOTH2(INPUT, KERNEL, WIDTH, SIGMA)
%
%   Smooths the matrix INPUT with a kernel specified by options.
%   Different than using conv2 because the matrix is padded with replicates
%   of the end value, not with zeros.
%  
%   KERNEL:  'gauss' or 'uniform'.  Default: 'gauss'
%   WIDTH: Gaussian kernel: specifies the length of the vector of the
%              kernel used to smooth with, in standard deviation units.  
%              This is automatically calculated
%              if empty.  (default: [6 6]: 3 std dev's from mean
%              in each direction)
%          Uniform kernel: number of points where kernel is non-zero.
%             SIGMA and WIDTH cannot both be specified 
%             If only SIGMA is specified it is used to compute WIDTH
%   SIGMA: for a Gaussian kernel, the standard dev of the kernel
%          Uniform kernel: specifies the width, as width/sqrt(12) =
%          sigma for the uniform.
%
%   See also FILTER, CONV, CONVN, SMOOTH, KSDENSITY, SMOOTH3
%
%$Id: smooth2.m 311 2008-07-31 20:25:12Z histed $

if nargin < 2, kernel = 'gauss'; end
% clean up kernel name
if ~isstr(kernel), error('2nd parameter must be kernel, e.g. ''gauss'''); end
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
        if isempty(width), width = [6 6]; end  
    case 'uniform'
        if nargin == 2, error('Specify width/sigma for uniform kernel'); end
        if nargin < 4, sigma = []; end  
end

if ~isempty(width) && length(width) == 1
    width = [width width];
end
if ~isempty(sigma) && length(sigma) == 1
    sigma = [sigma sigma];
end

   

% construct kernel
switch kernel
    case 'gauss'
        kern = kernNorm2(sigma, width);

    case 'uniform'
        if isempty(width)
            % use sigma to compute it, and write back to user options
            width = round(sigma * sqrt(12)) .* [1 1];
            % if even, undersmooth by 1
            evenIx = mod(width,2) == 0;
            if any(evenIx)
                newWidth(evenIx) = width(evenIx) - 1;
                warning('This sigma has even kernel width [%d %d], using width [%d %d]', ...
                    width(1), width(2), newWidth(1), newWidth(2));
                width = newWidth;
            end
        else
            % Width was specified.
            assert(isempty(sigma), ...
                'Cannot specify both Width and Sigma for uniform kernel');
        end
        % kernel must be odd
        if any(mod(width,2) == [0 0])
            error('Uniform widths must be odd');
        end

        kern = ones(width(1),width(2));
        kern = kern ./ sum(kern(:)); % normalize to 1
end
kernLen = length(kern);
assert(mod(kernLen, 2) == 1);  % make sure it's odd
assert(iswithintol(sum(kern(:)),1,10^-6));

% note that if the kernel is not odd, the convolution changes the x values:
% each new x value becomes the average of two old x values.

%%% do convolution 

% make sure a float
if ~ (isa(input, 'single') || isa(input, 'double'))
    input = double(input);
end

% whip the data into the right shape by padding,
% replicating the end values
nPads = (kernLen-1)/2;
[nRows,nCols,nFrames] = size(input);
colNs = [repmat(1,[1,nPads]) 1:nCols repmat(nCols,[1,nPads])];
rowNs = [repmat(1,[1,nPads]) 1:nRows repmat(nRows,[1,nPads])];
padInput = input(rowNs,colNs,:);

% Note, conv2 is an order of magnitude faster than convn on the same input
%output = convn(padInput, kern, 'valid');
if nFrames > 1
    output = 0*input;
    for iF = 1:nFrames
        fprintf(1, '%s: frame %d/%d\n', mfilename, iF, nFrames);
        output(:,:,iF) = conv2(padInput(:,:,iF), kern, 'valid');      
    end
else
    output = conv2(padInput, kern, 'valid');
end






%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function out = kernNorm2(sigma, width)
numpts = sigma .* width;
ptsAboveMean = ceil(numpts/2) - 1;  % add a point if necessary, 1 point for 0
if ptsAboveMean == 0
    error('Width (%s) too small for this sigma (%s)', ...
          mat2str(width(:)'), mat2str(sigma(:)'));
end
if prod(width) > 6^2
    error('width too large: units are standard deviations');
end
xp = (-ptsAboveMean:ptsAboveMean) / ptsAboveMean(1) * width(1)/2;
yp = (-ptsAboveMean:ptsAboveMean) / ptsAboveMean(2) * width(2)/2;
[x0,y0] = meshgrid(xp,yp);
% since xp is in units of standard dev's, 
out = exp(-0.5 * (x0.^2 + y0.^2)) ./ (sqrt(2*pi));
out = out./sum(out(:));  % normalize to 1

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% note this is faster than smooth3, by a factor of 2 as you would expect.
% to use smooth3 to do this you have to make a 3d matrix with length 2
% in the third dimension

%>> nreps = 10^2;
%>> input = ones(100,100);
%>> tic; for i=1:nreps, smooth2(input,'uniform',[9 9]); end; toc
%Elapsed time is 3.265538 seconds.
%>> i2 = cat(3,input,input);
%>> tic; for i=1:nreps, smooth3(i2,'box',[9 9 1]); end; toc
%Elapsed time is 7.628019 seconds.


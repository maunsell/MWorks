function matOut = cell2mat_padded (cellIn, dim, padVal)
%CELL2MAT_PADDED (ps-utils): Convert cell vector of unequal size els to matrix
%   MATOUT = CELL2MAT_PADDED (CELLIN, DIM, PADVAL)
%   functions identically to cell2mat except elements can be of
%   variable sizes, and each is padded as necessary with
%   padVal to be the same size as the largest element.
%   padVal is optional and defaults to 0.
%   dim is the dimension over which to concatenate
%
%   Notes
%   Right now this only handles cell vectors.
%   This should be expanded to handle cell matrices like cell2mat does.
%
%$Id: cell2mat_padded.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 2, padVal = 0; end
if nargin < 3, dim = 1; end

% check input
assert(isvector(cellIn), 'Only cell vectors are supported now');
nEls = length(cellIn);

%% calculate output size
maxDims = max(cellfun('ndims', cellIn));
maxSizes = zeros(1, maxDims);
for iD=1:maxDims
    maxSizes(iD) = max(cellfun('size', cellIn, iD));
end

%% construct padded versions of output matrix
for iE = 1:nEls
    sizeDiff = maxSizes - size(cellIn{iE});
    cellOut{iE} = padarray(cellIn{iE}, sizeDiff, padVal, 'post');
end

%% concatenate
matOut = cat(dim, cellOut{:});



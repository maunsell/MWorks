function cellout = mat2cell_singleton (mat)
% mat2cell_singleton (ps-utils): mat2cell but cell has singleton entries
%
% USAGE
%  function cellout = mat2cell_singleton (mat)
%
% Same as:
%   mat2cell (mat, 1, ones (size(mat,1), size(mat,2)));
% but shorthand
%
% $Id: mat2cell_singleton.m 125 2008-03-20 20:19:22Z vincent $

cellout = mat2cell (mat, 1, ones (size(mat,1), size(mat,2)));
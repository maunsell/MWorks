function num = sqcell_to_num (cellin)
%sqcell_to_num (ps-utils): convert cell array of 2d squares to numeric array
%
% num = function sqcell_to_num (cellin)
%
% Each element of the cell array must contain a square 2D numeric
% array, and each of these arrays must be the same size.
%
% $Id: sqcell_to_num.m 125 2008-03-20 20:19:22Z vincent $

if ndims (cellin) > 2
  warning ('Greater than two dimensions in cell array argument.');
  return
end

[crows ccols] = size (cellin);
[elrows elcols] = size (cellin{1});

nrows = crows.*elrows;
ncols = ccols.*elcols;

num = zeros (nrows, ncols);

for celli = 1:crows,
  for cellj = 1:ccols,
    numi = (celli-1).*elrows + 1;
    numj = (cellj-1).*elcols + 1;
    currcell = cellin {celli, cellj};
    % Debugging check only, disable for fastest runtime.
    if (ndims (currcell) > 2) | ~isequal (size (currcell), ([elrows, elcols]))
      disp (sprintf ('index into cell array of bad element: (%i, %i)', ...
                     celli, cellj));
      warning (['Above element of cell array is wrong size or' ...
            ' dimension.  Returning incomplete array.']);
      return
    end
    % end debug
    num (numi:(numi+elrows-1),numj:(numj+elcols-1)) = cellin {celli, cellj};
  end
end

function cv = colvect (row_or_col_vect)
%COLVECT (ps-utils): make sure a vector is a column vector
%   CV = COLVECT (ROW_OR_COL_VECT)
%   If the input is a column vector it's left alone.
%   If it's a row vector it's transposed.
%
%   See also ROWVECT
%
%$Id: colvect.m 125 2008-03-20 20:19:22Z vincent $

[rs cs] = size (row_or_col_vect);

if (cs == 1)   % optimize for non-transpose case
        cv = row_or_col_vect;
elseif (rs == 1)
        cv = row_or_col_vect';
else           % detect errors without adding much overhead to non-error cases
        error ('Not a vector!!!');
end



function rv = rowvect (row_or_col_vect)
%ROWVECT (ps-utils): make sure a vector is a row vector
%   RV = ROWVECT (ROW_OR_COL_VECT)
%   If the input is a row vector it's left alone.
%   If it's a column vector it's transposed.
%
%   See also COLVECT, colon syntax e.g row_or_col_vect(:)
%
%$Id: rowvect.m 125 2008-03-20 20:19:22Z vincent $

[rs cs] = size (row_or_col_vect);

if (rs == 1)
        rv = row_or_col_vect;
elseif (cs == 1)
        rv = row_or_col_vect';
else
        error ('Not a vector!!!');
end



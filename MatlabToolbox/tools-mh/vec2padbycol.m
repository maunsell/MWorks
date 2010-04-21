function padmat = vec2padbycol(invec, colnum, pad, maxcol)
%VEC2PADBYCOL (ps-utils): convert ragged vec to matrix, by giving col#
%    PADMAT = VEC2PADBYCOL(INVEC, COLNUM, PAD, MAXCOL)
%    This takes a ragged matrix stored as a vector and a set of column numbers
%    and rearranges it to give a rectangular matrix, padding as needed.
%    PAD defaults to NaN
%    MAXCOL gives the maximum number of columns of the matrix: if max(colnum)
%    is less than this, the rest of the columns will be padded
%
%    Note the output matrix is stored in column-major form.
%   
%$Id: vec2padbycol.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 3, pad = NaN; end
if nargin < 4, maxcol = max(colnum); end

invec = colvect(invec);

colcounts = histc(colnum,1:maxcol);

padmat = zeros(max(colcounts), maxcol) .* pad;

% fast in matlab 6.5, everyone else loses.
for i=1:maxcol
    thiscol = invec(colnum == i);
    padmat(1:length(thiscol),i) = thiscol(:);
end



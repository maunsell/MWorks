function D = euclid_dist(X1,X2)
%EUCLID_DIST (ps-utils): n-d euclidian distance between points
%    D = EUCLID_DIST(X1,X2) returns the distance between the corresponding
%    points in the vectors X1 and X2.  X1 and X2 are vectors of size (n,m) 
%    where n is the number of points, m is the number of dimension in the
%    euclidian space.  For X1 and X2, each row is a single point.  
%    In two dimensions, X1 and X2 are of size (n,2), the first column is 
%    the x value and the second the y value.
%    D has size (n,1).
%
%$Id: euclid_dist.m 125 2008-03-20 20:19:22Z vincent $

[N1,d1] = size(X1);
[N2,d2] = size(X2);
if d1 ~= d2, error('x1 and x2 must have same number of columns'); end

if N1 == 1
    N = N2;
    X1 = repmat(X1, [N,1]);
elseif N2 == 1
    N = N1;
    X2 = repmat(X2, [N,1]);
elseif N1 == N2
    N = N1;
else 
    error('Number of rows in input arguments must be equal or 1');
end


D = sqrt(sum(((X1-X2) .^ 2), 2));

assert(all(size(D) == [N,1]));
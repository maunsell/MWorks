function d = dummyv(x, coding)
%DUMMYV (posit) Create a matrix of dummy variables for a grouping var
%   D = DUMMYV(X,CODING) creates a dummy variable matrix D from the grouping 
%   vector X.  D will be have rank one less than the number of unique
%   grouping variables, (i.e., one column will be omitted).  The maximum
%   value of the grouping variable will be omitted.
%   Each cell will contain either an 0 or a 1.  
%   It will not include a column of all ones for the mean!
%   Group numbers need not be contiguous.
%
%   CODING = 'full' (default) means use one column for each variable
%   CODING = 'compact' means use the minimum number of columns possible
%  
%   based on MATLAB DUMMYVAR; bugs removed  
%
%$Id: dummyv.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 2, coding = 'full'; end
switch coding
    case 'compact'
        error;
    case 'full'
        [c1,c2,grp]=unique(colvect(x));
        d = sparse(1:length(x),grp,1);
        d = full(d); 
        % make it full rank
        d = d(:,1:(end-1));
    otherwise
        error ('Bad coding value');
end


function patchH = errorpatch(x,y,l,u)
%ERRORPATCH (Posit): draw an error region as a patch
%   patchH = ERRORPATCH(x,y,l,u)
%   patchH = ERRORPATCH(x,y,e)
%   This function draws an error region as a gray filled patch. 
%   You will usually want to use ANYSTACK to put the error patch at the
%   bottom of the stacking order.
%   
%   x, y, l, and u are as specified in ERRORBAR
%
%   See also ERRORBAR, ANYSTACK
%   
%$Id: errorpatch.m 125 2008-03-20 20:19:22Z vincent $


%%% Process arguments
if nargin < 4, 
    u = l;  % (x,y,e) case
end

if prod(size(x)) ~= length(x)
    % x is a matrix
    nPatches = size(x, 2);
else
    % convert to column vectors
    x = x(:);
    y = y(:);
    l = l(:);
    u = u(:);
    nPatches = 1;
end

if ~ (all(size(x) == size(y)) ...
      && all(size(x) == size(l)) ...
      && all(size(x) == size(u)))
    error('All inputs must be the same size');
end
if any(any(isnan(x) | isnan(y) | isnan(l) | isnan(u)))
    error('No NaNs allowed in input');  % causes PATCH to behave strangely
                                        % (some faces might not be filled)
end

%%% Constants
defaultPatchColor = [0.8 0.8 0.8];

%%%

for iP = 1:nPatches
    tX = x(:,iP);
    tY = y(:,iP);
    tL = l(:,iP);    
    tU = u(:,iP);
    
    upperErr = tY + tU;
    lowerErr = tY - tL;    
    
    ebY = cat(1, upperErr, flipud(lowerErr));
    ebX = cat(1, tX, flipud(tX));
            
    patchH(iP,1) = patch(ebX, ebY, defaultPatchColor);
end

set(patchH, 'EdgeColor', 'none', ...
            'Tag', 'Posit:errorpatch', ...
            'FaceLighting', 'flat');


    

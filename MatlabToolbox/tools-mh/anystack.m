function restackedH = anystack(inHandles, stackPlacementStr)
%ANYSTACK (Ps-utils): restack any objects
%  restackedH = ANYSTACK(inHandles, stackPlacementStr)
%
%  stackPlacementStr: 'top' or 'bottom'
%
%  This is like UISTACK except that it works.
%   UISTACK is broken, at least in R14SP1
%
%  See also UISTACK
%
%$Id: anystack.m 125 2008-03-20 20:19:22Z vincent $

if isempty(inHandles)
    error('Empty handle matrix passed in');
end


parents = get(inHandles, { 'Parent' });
assert(~isempty(parents), 'Error: no parents found?');

parentMat = cat(1, parents{:});
assert(all(parentMat == parentMat(1)), ...
       'Objects must all have the same parent');

pChildren = get(parentMat(1), 'Children');

assert(all(ismember(inHandles, pChildren)), ...
       'Desired handles not found in parent''s children');

foundIx = ismember(pChildren, inHandles);
newPChildren = pChildren(~foundIx);  % delete input handles
newPChildren = newPChildren(:);

switch lower(stackPlacementStr)
    case 'top'
        newPChildren = cat(1, colvect(inHandles), newPChildren);
    case 'bottom'
        newPChildren = cat(1, newPChildren, colvect(inHandles));        
    otherwise 
        error('Invalid placement: %s (only top/bottom supported now)', ...
              stackPlacementStr);
end

set(parentMat(1), 'Children', newPChildren);                           

if nargout > 0, restackedH = newPChildren; end



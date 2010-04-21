function outCell = cellstr_cat(varargin)
%CELLSTR_CAT (ps-utils): take cellstr and char strings, return a single cellstr
%  OUTCELL = CELLSTR_CAT(VARARGIN)
%
%$Id: cellstr_cat.m 125 2008-03-20 20:19:22Z vincent $

% a lot of hairy work here to avoid looping.

tempCell = varargin;

charIx = cellfun('isclass', varargin, 'char');
nChars = sum(charIx);
allChars = varargin(charIx);
% wrap each string inside its own cell array
if ~isempty(allChars)
    buriedChars = mat2cell(allChars, 1, ones(1,nChars));
    % stick each wrapped char array back into tempCell
    tempCell(charIx) = buriedChars(:);
end
outCell=cat(2,tempCell{:});




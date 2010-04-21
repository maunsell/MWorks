function structArray = structincell2structarray(cellArrayIn)
%STRUCTINCELL2STRUCTARRAY (ps-utils): concat structures inside cell array
%   structArray = STRUCTINCELL2STRUCTARRAY(cellArrayIn)
% 
%   cellArrayIn must have a single structure in each cell.  Each
%   structure must have the same fields.  If a cell contains NaN (the
%   singleton numeric matrix), all fields of the struct for that cell are set
%   to NaN's
%
%$Id: structincell2structarray.m 125 2008-03-20 20:19:22Z vincent $

% find 1x1 NaN numeric matrices in CellArrayIn 
nonStructIx = ~cellfun('isclass', cellArrayIn, 'struct');
nonStructVect = cat(1,cellArrayIn{find(nonStructIx)});
assert(all(isnumeric(nonStructVect)) && all(isnan(nonStructVect)), ...
       'Expect all non-struct values in cell array to be NaNs !');
nanIx = nonStructIx;

% make a filler struct where all fields are NaN
structFNames = fieldnames(cellArrayIn{1});
nFNames = length(structFNames);
for iF=1:nFNames
    tFName = structFNames{iF};
    fillerStruct.(tFName) = NaN;
end

% replace singleton nans with the filler struct
filledCell = cellArrayIn;
filledCell(nanIx) = { fillerStruct };

% now convert cell array to structure array
structArray = cell2mat(filledCell);

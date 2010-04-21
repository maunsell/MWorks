function singleStruct = structarray2singleton(structArrIn)
%
%  Note that this uses cell2mat on each field to make it a single matrix, so
%  it is subject to the limitations of cell2mat.  
%       Specifically, if there are any field elements that are empty, they will be
%       left out of the resulting big matrix (it will be smaller).  Fill with
%       NaN's if you care about this.
%
%$Id: structarray2singleton.m 125 2008-03-20 20:19:22Z vincent $

error('not tested well, verify')

anonCell = struct2cell(structArrIn);
fNames = fieldnames(structArrIn);
anonCellSize = size(anonCell);
nFields = anonCellSize(1);
structArrSize = anonCellSize(2:end);

for iF=1:nFields
    tFieldName = fNames{iF};
    tFieldVals = reshape(anonCell(iF,:), structArrSize); %handle: multi-dims
                                                         %(of orig struct arr)    
    singleStruct.(tFieldName) = cell2mat(tFieldVals);
end


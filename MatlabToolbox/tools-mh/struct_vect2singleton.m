function outStruct = struct_vect2singleton(inStruct, fieldDims)
%STRUCT_VECT2SINGLETON (ps-utils): struct array -> single struct, cat fields
%   outStruct = STRUCT_VECT2SINGLETON(inStruct, fieldDims)
% 
%   inStruct is a struct vector of length structLen
%   fieldDims is either a scalar or a vector of length nFields, specifying
%      the dimensions to cat for each field.  
%   
%$Id: struct_vect2singleton.m 125 2008-03-20 20:19:22Z vincent $

% Notes: there should be a way to do this for struct arrays with ndims >= 2
% but I don't know how right now

if nargin < 2 || isempty(fieldDims), fieldDims = 1; end

if ~isvector(inStruct), disp('inStruct must be a struct vector'); end

inStruct = rowvect(inStruct); 

fNames = fieldnames(inStruct);
nFields = length(fNames);

if length(fieldDims) == 1, fieldDims = repmat(fieldDims, 1, nFields); end
assert(length(fieldDims) == nFields);

for iField = 1:nFields
    tName = fNames{iField};
    tDim = fieldDims(iField);
    oneFieldVal = inStruct(1).(tName);
    
    % special case function handles
    if isa(oneFieldVal, 'function_handle')
        % each must be a scalar, so make it a cell array
        outStruct.(tName) = { inStruct.(tName) };
        continue
    
    elseif ischar(oneFieldVal)
        % special case char arrays: make a cellstr
        %   Note we do not make a big char array first (avoid empty str case)
        outStruct.(tName) = deblank(cellstr({ inStruct.(tName) }));
        continue
    end
        

    % auto compute tDim if unspecified and possible
    if nargin < 2,
        if isvector(oneFieldVal)
            if size(oneFieldVal, 1) == 1
                tDim = 1;
            elseif size(oneFieldVal, 2) == 1
                tDim = 2;
            elseif isempty(oneFieldVal)
                if ~isempty(cat(1, inStruct.(tName)));
                    error('First field empty but all are not empty');
                end
            else                
                error('should never get here');
            end
        end
    end

    outStruct.(tName) = cat(tDim, inStruct.(tName));
end




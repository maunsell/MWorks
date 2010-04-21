function outStruct = create_null_struct(inStruct, useNaNForNonFloatsStr)
%CREATE_NULL_STRUCT (ps-utils): Make a dup structure filled with NaN's
%   outStruct = CREATE_NULL_STRUCT(inStruct, useNaNForNonFloats)
%
%   This function creates a new structure with the same fields as the old
%   structure and fills the field values with NaNs as appropriate.  This is
%   useful for concatenating single struct arrays into a vector/matrix struct
%   array.  
%
%   If useNaNForNonFloats == 'useNaNForNonFloats', then numeric arrays which
%   are not floating point (integer arrays, including logicals), are set to
%   NaN.  This will cause conversion to float on concatenation.
%
%   Note that this function is called recursively when inStruct has fields
%   that are structs.  Be careful not to nest too deeply.
%
%$Id: create_null_struct.m 125 2008-03-20 20:19:22Z vincent $

% notes: could use blanks for strings at some point

if nargin < 2, useNaNForNonFloatsStr = ''; end

useNaNForNonFloats = false; % default
if strcmpi(useNaNForNonFloatsStr, 'useNaNForNonFloats')
    useNaNForNonFloats = true;
end

nullAnonFunctionHandle = @() NaN;  % so we only create it once
    
fNames = fieldnames(inStruct);
nFields = length(fNames);

for i=1:nFields
    tFName = fNames{i};
    tInField = inStruct.(tFName);

    if isfloat(tInField) 
        nullVal = repmat(NaN, size(tInField));
    elseif isnumeric(tInField) 
        if useNaNForNonFloats
            nullVal = repmat(NaN, size(tInField));
        else
            error('Integer numeric arr found and useNaNForNonFloats unset');
        end
    elseif isa(tInField, 'function_handle')
        nullVal = nullAnonFunctionHandle;  % defined above
    elseif ischar(tInField) 
        nullVal = '';
    elseif iscell(tInField) 
        nullVal = {};
    elseif isstruct(tInField)         
        % recurse
        nullVal = create_null_struct(tInField, useNaNForNonFloatsStr);
    elseif issparse(tInField) || isobject(tInField)
        error('Field type not allowed: %s', tFName);
    else
        error('Field type unrecognized: %s', tFName);
    end

    outStruct.(tFName) = nullVal;
end

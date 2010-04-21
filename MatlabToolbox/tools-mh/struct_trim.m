function structOut = struct_trim(structIn, keepNames, ignoremissing)
%STRUCT_TRIM (ps-utils): remove/prune fields by specifying fields to keep
%   STRUCTOUT = STRUCT_TRIM (STRUCTIN, KEEPNAMES, IGNOREMISSING)
%   KEEPNAMES: cellstr with list of field names to copy to STRUCTOUT
%   if any name not found, 
%       if IGNOREMISSING == 'ignoremissing'
%           return empty
%       else error
%
%   See also FIELDNAMES, RMFIELD
%
%$Id: struct_trim.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 3, ignoremissing = []; end

fNames = fieldnames(structIn);
removeNames = fNames(~ismember(fNames, keepNames));
structOut = rmfield(structIn, removeNames);


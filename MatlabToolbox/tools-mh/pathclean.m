function outPath = pathclean(inPath)
%PATHCLEAN (ca-tools): is filesep / colon format etc. correct in path str?
%
%   080616: histed: created
%
%$Id$

if isunix
    error('not implemented yet: usually use forward slashes all the time');
elseif ispc
    % replace forward with reverse slashes
    outPath = strrep(inPath, '/', '\');
end

    

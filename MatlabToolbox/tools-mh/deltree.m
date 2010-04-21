function deltree(pathToDel)
%DELTREE (ps-utils): remove a directory and all its components: be careful
%   calls 'rm -rf' on unix and 'rmdir /s' on Windows
%
%   080315 histed - brought in from old posit code
%
%$Id: deltree.m 266 2008-06-19 05:42:49Z histed $

%%% Check args
if iscell(pathToDel) && length(pathToDel) == 1
    pathToDel = pathToDel{1};
end
if ~ischar(pathToDel)
    error('Input path must be a string');
end
if any(pathToDel == ' ')
    error('Input path can contain no spaces');
end
if ~exist(pathToDel, 'file')
    error('Path to delete not found: %s', pathToDel);
end

    

%%% do deletion
if isunix
    commandName = [ 'rm -rf ' pathToDel];

    [status,result] = unix(commandName);
else
    pathToDel = pathclean(pathToDel);
    commandName = [ 'rmdir /q /s ' pathToDel ];
    [status,result] = dos(commandName);
end

if status ~= 0
    error('Problem executing ''%s'', result %s', ...
              commandName, pathToDel);
end


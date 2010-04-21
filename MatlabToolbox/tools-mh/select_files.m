function [cellfiles,nfiles,result] = select_files (inpath, inmask)
%SELECT_FILES (ps-utils): graphical file selection dialog box
%   [CELLFILES,NFILES,RESULT] = SELECT_FILES (INPATH, INMASK)
%   inpath is the dir containing files to select
%   inmask is the wildcard mask specifying files to choose from.  Defaults to
%   all files.
%   If result is specified, it returns 1 on success and 0 on error.  If not
%   specified this program exits on error.
%   If a filename is passed in, it is just returned.
%
%$Id: select_files.m 125 2008-03-20 20:19:22Z vincent $

if ~exist ('inmask')
  inmask = '';
end

names = dir(fullfile(inpath, inmask));
cellnames = {names.name};
if length(cellnames) == 1
  cellfiles = cellnames;
  nfiles = 1;
  return
end


% sort cellnames and remove dir entries
cellnames = sortrows (cellnames');
if strcmp (cellnames{2}, '..')
  cellnames = cellnames (3:end);
elseif strcmp (cellnames{1}, '.')
  cellnames = cellnames (2:end);
end

[selection, ok] = ...
    listdlg('name', 'Select files', ...
            'PromptString', 'Select file(s)', ...
            'SelectionMode', 'multiple', ...
            'ListString', cellnames);

if ok == 0, 
  errstr = ('File selection canceled.');
  if nargout == 3, result=0; warning (errstr); return; 
  else error (errstr);  
  end
end

cellfiles = cellnames(selection);
nfiles = length(selection);


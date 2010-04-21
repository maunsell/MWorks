function filenames = pick_file_dialog (from_path, file_mask)
%pick_file_dialog (ps-utils): a dialog box to select files
%
% USAGE
% 
%  function filenames = pick_file_dialog (from_path, file_mask)
%
% from_path: path to files to select from
% file_mask: file mask to select from.  Defaults to '*' on unix or
%            '*.*' on windows
%
% filenames is a cell array of selected filenames with full paths.

if ~exist ('from_path'), from_path = ''; end
if ~exist ('file_mask')
  file_mask = '';  % dir handles the case when there is no mask
end

names = dir(fullfile(from_path, file_mask));
cellnames = {names.name};

% sort cellnames
c = cellnames';
c = strvcat(c{:});
c = sortrows(c);
cellnames = num2cell(c,2);

[selection, ok] = ...
    listdlg('name', 'pick_file_dialog (ps-utils)', ...
    'PromptString', 'Select file(s) to convert', ...
    'SelectionMode', 'multiple', ...
    'ListString', cellnames);

if ok == 0, 
  warning ('No files selected, returning empties.');
  filenames = [];
  return; 
end

filenames = {cellnames{selection}};
[nrows ncols] = size(filenames);

for i=1:ncols,
  filenames{i}= deblank(fullfile(from_path, filenames{i}));
end




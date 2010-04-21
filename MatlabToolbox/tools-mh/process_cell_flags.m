function varargout = process_cell_flags (flag_names_cell, flags_cell)
%PROCESS_CELL_FLAGS (ps-utils): test for flag strings in option variable
%   CELLOUT = PROCESS_CELL_FLAGS (FLAG_NAMES_CELL, FLAGS_CELL)
%
%   flags_names_cell is a cell array of strings containing the possible flags
%   flags_cell is the cell vector that the user passed in, case doesn't matter.
%     'all' or 'none' are specially processed.  If there's only one option this
%     can be a matrix or cell, and if it's empty, defaults to 'none'.
%   boolout has the same size as flag_name_cell but there is a 1 if the option
%     is there and 0 if not.
%
%   Example
%   (from an early version of cortex_read)
%   [keep_eog keep_epp keep_header keep_codes] = ...
%       process_cell_flags ({'eog', 'epp', 'header', 'codes'}, varargin);
%
%$Id: process_cell_flags.m 125 2008-03-20 20:19:22Z vincent $

if ~iscell (flags_cell), flags_cell = {flags_cell}; end
num_poss_flags = length (flag_names_cell);
cellbool = cell (num_poss_flags, 1);
[cellbool{:}] = deal(0);

% if empty, short-circuit to save time in this routine.
% (not that it helps much, since we have to do the above first.  no good way
% to make much faster.)
if isempty (flags_cell), varargout = cellbool; return; end

for iopt = 1:length(flags_cell)
  switch flags_cell{iopt}
   case {'all', ''}
    [cellbool{:}] = deal(1);
    varargout = cellbool;
    return
    
   case 'none'
    [cellbool{:}] = deal(0);
    varargout = cellbool;
    return
    
   case flag_names_cell
    % ok, which one did it match
    for ifn = 1:num_poss_flags
      if strcmp (lower(flags_cell{iopt}), lower(flag_names_cell{ifn}))
        cellbool{ifn} = 1;
        break;
      end
    end
    
   otherwise
    error(sprintf ('Unknown flag specified %s', flags_cell{iopt}));
  end
end

varargout = cellbool;



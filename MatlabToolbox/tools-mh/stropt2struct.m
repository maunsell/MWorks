function opt_struct = stropt2struct (options_cell)
%STROPT2STRUCT (ps-utils): convert get/set-style options to a structure
%   FUNCTION OPT_STRUCT = STROPT2STRUCT (OPTIONS_CELL)
%   options_cell has to be a single cell array: call this function as
%   stropt2struct ( {'name1', 1, 'name2', 2'} )
%   NOT 
%   stropt2struct ('name1', 1, 'name2', 2')
%  
%$Id: stropt2struct.m 125 2008-03-20 20:19:22Z vincent $

if (iscell(options_cell) && length(options_cell) == 1) 
    options_cell = options_cell{1};  % unwrap it and try it out
end

if isempty(options_cell) 
    opt_struct=struct([]); 
    return
elseif isstruct(options_cell)
    opt_struct = options_cell; 
    return; 
elseif iscell(options_cell)
    opt_struct = cell2struct(options_cell(2:2:end), options_cell(1:2:end), 2);
    % Yeah it's trivial.
    return
else
    error('Unrecognized argument type');
end


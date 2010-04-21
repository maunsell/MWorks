function outfiles = complete_filenames (infiles, ...
                                        def_path, def_name, def_ext, ...
                                        options)
%COMPLETE_FILENAMES (ps-utils): add default path and extension to filenames.
%   OUTFILES = COMPLETE_FILENAMES(INFILES, DEF_PATH, DEF_NAME, DEF_EXT, OPTIONS)
%
%   infiles: a list of filenames.  If no path, name or extension is
%     specified, one will be filled in.  
%   def_path, def_name, def_ext: defaults for each path component.
%     If any of these parameters are '' then that component will not
%     be added to an infilename even if the component is missing.
%   option: cell array of string options: 'error' is default, means die 
%     if there is no component and '' specified, 'ignoremissing' means ignore it
%   outfiles: the list with path and extension completed.
%
%$Id: complete_filenames.m 125 2008-03-20 20:19:22Z vincent $

[frows fcols] = size (infiles);
outfiles = [];

if ~exist ('options'), 
  ignoremissing = 0; 
else
  flag_names = {'error', 'ignoremissing'};
  [a ignoremissing] = process_cell_flags (flag_names, options);
end

for ifile = 1:frows,
  [path name ext] = fileparts (deblank(infiles(ifile,:)));

  if isempty (path), 
    if isempty(def_path) & ~ignoremissing
      error ('Missing required path.'); return 
    else
      path = def_path; 
    end
  end
  
  if isempty (ext), 
    if isempty (def_ext) & ~ignoremissing
      error ('Missing required ext.');
    else
      ext = def_ext; 
    end
  end
  
  if isempty (name),
    if isempty (def_name) & ~ignoremissing
      error ('Missing required file/dir name');
    else
      name = def_name; 
    end
  end

  outfiles = strvcat (outfiles, fullfile (path, [name ext]));
end



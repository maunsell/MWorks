% TOOLS
%
% Files
%
% math functions
%   angle_map_to_minuspi_pi  - remap thetas to [-pi,pi) range
%   angle_map_to_zero_2pi    - remap thetas to [0,2pi) range
%   atan3                    - convert y and x values into theta in range [0,2*pi)
%   ceilto                   - take ceil to n decimal places
%
% data type
%   cell2cellstr             - recursively convert all cells of a cell arr to str
%   cell2csv                 - Writes Cell-Array content into csv.
%   cell2mat_padded          - Convert cell vector of unequal size els to matrix
%   cellstr_cat              - take cellstr and char strings, return a single cellstr
%   colvect                  - make sure a vector is a column vector
%
% color map
%   cmap_blue                - CMAP_GREEN colormap linear on green values
%   cmap_green               - colormap linear on green values
%   cmap_posneg_rbk          - CMAP_POSNEG_RDBU a colormap to represent pos/neg values
%   cmap_posneg_rdbu         - a colormap to represent pos/neg values
%   cmap_posneg_yck          - CMAP_POSNEG_RDBU a colormap to represent pos/neg values
%   cmap_red                 - colormap linear on green values
%   colormap_get_index       - (Posit): get index of color in map; add if missing
%   colors_linspace          - axes ColorOrder -> lin spaced colors from cmap
%   colorspec2rgb            - (Posit): convert Matlab colorSpec to rgb vector
%
%
%   anystack                 - restack any objects
%   arrow3                   - ARROW3
%   assert_always            - test condition, if it fails, error (cannot disable)
%   assert_args              - test condition.  Use for checking arguments!
%   axes_draw_scalebar       - (mh-utils): convert tick-labeled axes into scalebar labeled
%   axis_label_radians       - draw radian tick labels (with \pi's)
%   call_function_in_dir     - call a function in a given working dir
%   caller_mfilename         - return the mfilename of a fn's caller
%   camera_apply             - restore position of camera previously saved
%   camera_save              - save position of camera so it can be loaded later
%   check_for_R              - check_for_R See if R is initialized, if not, set it up
%   chkstropt                - does this parameter look like a stropt cell array?
%   chop                     - round to n significant figures
%   chop_seconds             - Sig digits of YYHHMMSS time representation
%   clipclip                 - (utils-mh): copy current figure to a snapshot on disk for ppt etc.
%   clipclipimage            - copy image of current axes to a file
%   comet_speed              - comet-like trajectory with speed control.
%   complete_filenames       - add default path and extension to filenames.
%   con2mat                  - parse Cortex conditions file
%   cortex_read_fields       - read cortex file into matlab data.
%   create_null_struct       - Make a dup structure filled with NaN's
%   csvread_dataframe        - read CSV file outputted by R as a dataframe
%   csvread_textornum        - read CSV file, converting numeric fields to double
%   deal_to_mat              - 
%   deblank_bothsides        - remove leading and trailing blanks from a string
%   deletechar               - deletechar remove all occurrances of a char from vector
%   deltree                  - remove a directory and all its components: be careful
%   disk_cache               - (posit): temporary disk caches
%   drange                   - range of data along given dimension
%   dummyv                   - (posit) Create a matrix of dummy variables for a grouping var
%   ensure_cellstr           - ensure_cellstr make sure a string is a cell matrix of strings
%   errorpatch               - (Posit): draw an error region as a patch
%   euclid_dist              - n-d euclidian distance between points
%   export_to_libsvm         - write a text file that libsvm can read
%   exportfigPrint           - EXPORTFIG_PRINT Export figure, wrapper around PRINT
%   extract_fields           - retun vals of specified struct fields in cell array
%   fig_max_print_size       - set figure to fill entire page when printed
%   figure_note              - make little note in the bottom-right of figure
%   find_consecutive         - return indices of runs of consec. numbers
%   find_consecutive_true    - indices of consecutive true entries
%   fit_tuningcurve          - given data, fit a 1-d curve using opt toolbox
%   floorto                  - take floor to n decimal places
%   fontname_fixed           - get the name of a fixed-width font on this system
%   fullfileMH               - fullfileMH concatenate paths, using '/' always as filesep
%   generalized_batch        - generalized_batch call a series of functions
%   hash_chars               - hash function for ASCII strings
%   hash_recurse             - MD5 hash arrays, handles cell/struct recursively
%   homogen_projtoplane      - plane projection matrix (homogeneous coords)
%   hostname                 - return hostname of machine matlab is running on
%   information_discrete     - estimate mutual information from samples
%   isgraphical              - isgraphical Are graphics available or is term text-only?
%   isnumber                 - isnumber true for numbers in strings.
%   isstropt                 - does this parameter look like a stropt cell array?
%   isstropt_name            - does option name exist in stropt?
%   iswithintol              - are two floating point numbers equal?
%   labels_between_ticks     - draw labels on axis and ticks between them
%   lastmodtime              - Use DIR to get the last modification time of a file
%   madm                     - (utils-general): Median absolute deviation from median
%   make_title               - (stacks_mh): return a string to be used as a plot title
%   mat2cell_singleton       - mat2cell_singleton mat2cell but cell has singleton entries
%   memory_cache             - (posit): keep data in memory for speed
%   memory_used              - baseOrCurrentStr can be 'base', 'current', or 'both'
%   nanmean_dim              - Average or mean value ignoring NaNs in any dimension
%   nansum_dim               - sum value ignoring NaNs in any dimension
%   next_color_by_order      - get next color from axes ColorOrder
%   num2str_metric           - NUM2STR_METRIC: convert num to a str with metrix postfixes
%   offset_markers           - if data values of lines overlap, offset them
%   onepct_etimestr          - pretty-print time rounded to ~ 1% accuracy
%   pad_string               - make a string exactly a certain size
%   parse                    - break input string into a series of words, using strtok.
%   pathcell                 - return current path in a cellstr
%   pick_file_dialog         - pick_file_dialog a dialog box to select files
%   plot_distrib             - plot the mean and std dev of a matrix
%   pp_hash_key              - pretty print a hash key to a str for display
%   process_cell_flags       - test for flag strings in option variable
%   process_str_options      - process_str_options deal with options like Matlab's get/set do
%   raytestinv               - invert a rayleigh test (give Z such that P(Z)<=P0)
%   rectify                  - if input < 0, make it equal 0, if positive, return value
%   remove_trailing          - remove specified trailing chars from a vec 
%   render_text_to_array     - outImg = render_text_to_array(tString, ...)
%   render_text_to_array2    - make an image with text in it
%   resize_legend            - - Changes LEGEND fontsize and axes position
%   rotate_patch             - rotate a patch object: instead of buggy ROTATE.m
%   roundto                  - round to n decimal places
%   rowvect                  - make sure a vector is a row vector
%   sample_fstatistic        - compute the F statistic on data
%   scatter3_varsize         - SCATTER_VARSIZE scatter plot with replications 
%   scatter_linearfit        - add regression line to scatterplot
%   scatter_varsize          - scatter plot with replications 
%   seconds2str              - convert num seconds to 'NNdNNhNNmNNs'
%   secsplit                 - convert number of seconds to days, hours, min, sec
%   select_files             - graphical file selection dialog box
%   smooth1                  - (posit): smooth along one dimension of a matrix
%   smooth2                  - (posit): smooth a 2d matrix along both dimensions
%   sqcell_to_num            - sqcell_to_num convert cell array of 2d squares to numeric array
%   strip_chars_except       - strip_chars_except remove all chars not specified from str
%   stropt2struct            - convert get/set-style options to a structure
%   stropt_defaults          - deal with options like Matlab's get/set do
%   stropt_del               - delete str options 
%   stropt_get               - get opt in stropt by name
%   stropt_merge             - merge two sets of stropts
%   stropt_names             - list names of options in a stropt cell array
%   stropt_set               - set options in stropt by using name/value pairs
%   stropt_trim              - remove name/val pairs by specifying names to keep
%   struct2stropt            - convert opt struct to get/set type stropts
%   struct_sort              - sort structure field names
%   struct_trim              - remove/prune fields by specifying fields to keep
%   struct_union             - merge two structures into one.
%   struct_vect2singleton    - struct array -> single struct, cat fields
%   structarray2singleton    - Note that this uses cell2mat on each field to make it a single matrix, so
%   structincell2structarray - concat structures inside cell array
%   suptitle                 - Puts a title above all subplots.
%   suptitle2                - title over all subplots; improved version (2)
%   toggle                   - changes the state of a boolean var.
%   transfinv_anscombe       - (sacvector2): invert the var-stabilizing Anscombe transf
%   transform_anscombe       - (sacvector2): do a var-stabilizing Anscombe transformation
%   vec2padbycol             - convert ragged vec to matrix, by giving col#
%   vec2padded               - convert ragged mat from vector to mat
%   vect_polar_add           - add vectors in polar form
%   verify_warnings_on       - if warning state is off, error()
%   vert_lines               - draw vertical lines on plot
%   vert_shade               - draw vertical shaded square gradient on plot
%   vert_shade_gauss         - VERT_SHADE draw vertical gaussian-shaded square gradient 
%   vonmpdf                  - (posit): von Mises density function defined on 0..2*pi
%   warning_change_default   - change only the _default_ warning state
%   warning_state_restore    - restore ALL possible warning state
%   warning_state_save       - save ALL possible warning state
%   wildcard                 - return wildcard character for present platform

function exportfigPrint(figH, fileName, varargin)
%EXPORTFIG_PRINT (ps-utils): Export figure, wrapper around PRINT
%   EXPORTFIG_PRINT(FIGHANDLE, FILENAME, SPEC)
%   See code for spec options. ('edit exportfig_print')
%   This takes the place of EXPORTFIG_OPTS: with Matlab R14, EXPORTFIG itself
%   is not needed because support for OuterPositions has been
%   added.
%   
%   Notes:
%   It may just be easier to call PRINT; however this function
%       will save and restore the existing PaperPosition of the
%       figure.
%   The default figure size is a multiple of [4 3]; this is the
%       normal figure aspect ratio on-screen.
%   The easiest way to scale text, axes labels, etc, is to specify
%       a small figure size (e.g. 2*[1 0.75] inches) and a high
%       resolution (e.g. 400dpi, giving 800x600 image).  Note that
%       formats like TIFF and PNG save the real size in inches so
%       you will have to rescale.  The alternative is to set all
%       line/text size properties in the figure by hand.
% 
%   See also: PRINT, SPK_SPEC
%
%$Id: exportfigPrint.m 266 2008-06-19 05:42:49Z histed $

user_defaults = { 'Size', 7.*[1 0.75] ... % vector; [width height]
                  'Units', 'inches', ... % units of size
                  'Resolution', 400, ...
                  'FileFormat', 'pdf', ... % see -d options to PRINT
                  'Renderer', '', ...
                  'BoundingBox', [], ...
                  'CMYKColor', false, ...
                };
% note default options give you an 800x600 figure that is supposed
% to be 4x3 inches in size.  
% for a monitor: you want to double the size (giving resolution 100dpi)
% for a printer: fine to use as is, or re-export with higher
%   resolution.

uopts = stropt2struct(stropt_defaults(user_defaults, varargin));

%%% save old paper pos / units
% note order is important here: e.g. units must come before pos
fieldsToSave = { 'PaperType', 'PaperUnits', 'PaperOrientation', ...
                 'PaperPosition', 'PaperPositionMode', ...
                 'PaperSize' };
savedCell = get(figH, fieldsToSave);

% note order is important here: units must come before pos
set(figH, 'PaperUnits', uopts.Units, ...
          'PaperOrientation', 'Portrait', ...
          'PaperPosition', [0 0 uopts.Size(1), uopts.Size(2)], ...
          'PaperSize', uopts.Size);

% print opts
pOpts = { sprintf('-d%s', uopts.FileFormat), ...
          sprintf('-r%d', uopts.Resolution) };
if ~isempty(uopts.Renderer)
    pOpts = cat(2, pOpts, { sprintf('-%s', uopts.Renderer) });
end
if uopts.CMYKColor
    pOpts = cat(2, pOpts, '-cmyk');
end
if strcmp(uopts.BoundingBox, 'loose')
    pOpts = cat(2, pOpts, '-loose');
elseif strcmp(uopts.BoundingBox, 'tight')    
    pOpts = cat(2, pOpts, '-tight');    
end

%%Deal with GS bug on paths > 120chars
if length(fileName) > 120
    error('Output file name too long: Ghostscript will cause error');
    
    % one way to deal is below; needs debugging

    [desPath desName desExt] = fileparts(fileName);
    tempName = fullfile(tempdir, [desName desExt]);
    
    % do export to temp file
    print(figH, tempName, pOpts{:});
    
    % figure out the saved ext
    if isempty(desExt)
        % no ext means we need to use a wildcard to find it
        savedName = ls([tempName '*.*']);
        [crap actName actExt] = fileparts(savedName);
    else
        actExt = desExt;
    end
    savedFullName = fullfile(tempdir, [desName actExt]);
    desFullName = fullfile(desPath, [desName actExt]);
    
    % move to desired location
    movefile(savedFullName, desFullName);
else
    % just print normally
    print(figH, fileName, pOpts{:});
end


%%% restore old fig details
set(figH, fieldsToSave, savedCell);

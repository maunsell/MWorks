function clipclip(figH, tSize, fFormat, boundingBox)
%CLIPCLIP (tools): copy current figure to a snapshot on disk for ppt etc.
%   clipclip(figH, tSize, fFormat)
% 
%   export path is hardcoded below
%
%   notes: figures with transparency don't work with this, printing
%   doesn't work (matlab bug).  Use clipclippix to grab a screenshot.
%
%   See also CLIPCLIPPIX
%
%$Id: clipclip.m 293 2008-07-10 02:45:19Z histed $

if nargin < 1 || isempty(figH), figH = gcf; end
if nargin < 2 || isempty(tSize), tSize = 6*[1 0.75]; end
if nargin < 3 || isempty(fFormat), fFormat = 'pdf'; end

fName = [mfilename '-' datestr(now, 'yymmdd-HHMM_SS_FFF')];

%% set paths
% file export path
%fPath = '~/shared/snapshots';
%fPath = 'i:/users/histed/snapshots';
dirs = directories;
assert(isfield(dirs, 'toolsSnapshots'), ....
       'needed dir entry is missing, edit directories.m');

fullF = fullfileMH(dirs.toolsSnapshots, fName);
exportfigPrint(figH, fullF, ...
               'FileFormat', fFormat, ...
               'Size', tSize, ...
               'Renderer', 'painters');





%unix(sprintf('convert %s %s', [fullF '.png'], [fullF '.wmf']));

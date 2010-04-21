function outPath = fullfileMH(varargin)
%fullfileMH (ps-utils): concatenate paths, using '/' always as filesep
%
%   Windows MATLAB understands '/' as a filesep, and sprintf gets confused
%   by '\', so use '/'
%
%$Id: fullfileMH.m 125 2008-03-20 20:19:22Z vincent $

% first strip trailing slashes
varargin = regexprep(varargin, '(.*)/$', '$1');

% concat
nPathsIn = length(varargin);
newOut = cell(1,nPathsIn*2-1);
newOut(1:2:end) = varargin(:);
[newOut{2:2:end}] = deal('/');
outPath = cat(2,newOut{:});

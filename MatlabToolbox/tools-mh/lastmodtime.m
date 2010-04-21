function dirTime = lastmodtime(filename)
%LASTMODTIME (ps-utils): Use DIR to get the last modification time of a file
%
%   dirTime = LASTMODTIME(filename)
%
%$Id: lastmodtime.m 125 2008-03-20 20:19:22Z vincent $

d = dir(filename);
if isempty(d)
    error('File not found');
else
    dirTime = d.date;
end


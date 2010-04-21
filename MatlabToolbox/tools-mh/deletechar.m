function vectorout = deletechar (vectorin, char_to_remove)
%deletechar (ps-utils): remove all occurrances of a char from vector
%
% USAGE
%   function vectorout = deletechar (vectorin, char_to_remove)
%
% char_to_remove is optional, and defaults to ascii 0.
%
% This is a quick and dirty function to remove all occurances of a
% given character from a vector.  These characters will be deleted
% and the vector will shrink.
%
% $Id: deletechar.m 125 2008-03-20 20:19:22Z vincent $

if ~exist ('char_to_remove')
  char_to_remove = 0;
end

vectorout = vectorin ((find (vectorin ~= char_to_remove)));


function [starti,endi] = find_consecutive_true (invec, how_many)
%FIND_CONSECUTIVE_TRUE (ps-utils): indices of consecutive true entries
%  [STARTI ENDI] = FIND_CONSECUTIVE (INVEC, HOW_MANY)
%
%  find consecutive true entries of HOW_MANY or more in a logical vector
%
%  $Id: find_consecutive_true.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 2, how_many = 2; end

assert(islogical(invec));

[startS endS] = find_consecutive(invec, 0);

runLens = endS-startS;
isTrueToStart = invec(startS) == true;

desIx = runLens >= how_many & isTrueToStart;

starti = startS(desIx);
endi = endS(desIx);






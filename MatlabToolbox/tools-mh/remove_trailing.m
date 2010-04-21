function newvec = remove_trailing (oldvec, trailers_to_remove)
%REMOVE_TRAILING (ps-utils): remove specified trailing chars from a vec 
%   NEWVEC = REMOVE_TRAILING (OLDVEC, TRAILERS_TO_REMOVE)
%   trailers_to_remove is a vector of chars.  if any of these chars
%   are found at the end of oldvec in any combination, they'll be removed.
%  
%   Examples
%      remove_trailing ([0 1 2 3 4 5 6 7 8 7], [8 7]) == [0 1 2 3 4 5 6]
%      remove_trailing (str, ' ') is the same as deblank (str)
%
%$Id: remove_trailing.m 125 2008-03-20 20:19:22Z vincent $

is = find (~ismember (oldvec, trailers_to_remove));
newvec = oldvec (1:max(is));

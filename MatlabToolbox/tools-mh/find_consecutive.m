function [starti,endi] = find_consecutive (invec, increment)
%FIND_CONSECUTIVE (ps-utils): return indices of runs of consec. numbers
%  [STARTI ENDI] = FIND_CONSECUTIVE (INVEC, INCREMENT)
%
%  increment is optional and tells what the difference of i and i+1
%  should be to be considered a run.  (default is 0)
%
%  Examples:
%  invec = [0 0 0 1 2 3 3 0 4 8 10 12 14 24 25 26 27 20 0 0 0];
%                   |         |             |             |
%  (index)         (5)       (10)          (15)          (20)
%
%  find_consecutive (invec, 1): starti = [ 3 14 ]
%                               endi   = [ 6 17 ]
%  find_consecutive (invec, 0): starti = [ 1 6 19 ]
%                               endi   = [ 3 7 21 ]
%  find_consecutive (invec, 2): starti = [  9 ]
%                               endi   = [ 13 ]
%
%  $Id: find_consecutive.m 125 2008-03-20 20:19:22Z vincent $

starti = [];
endi = [];

if nargin < 2, increment = 0; end

invec=invec(:);
if size(invec,2) > 1,
  error('Input must be a vector.');
end

in_prime = diff (invec);
des_increment = in_prime==increment;
in_double_prime = diff([0; des_increment; 0]);

starti = find(in_double_prime == 1)';
endi = find(in_double_prime == -1)';

return

% Replaced by above diff-twice code
%  
% $$$ % This code is HORRIBLY NASTY.  
% $$$ 
% $$$ deltas = diff(invec);
% $$$ consec = find (deltas == increment);
% $$$ 
% $$$ % hack in special cases of consec array.
% $$$ if (size (consec) == [1 1])
% $$$   starti = consec(1);
% $$$   endi = consec(1) + 1;
% $$$   return
% $$$ elseif isempty (consec)
% $$$   return
% $$$ end  
% $$$ 
% $$$ last_consec = max (find (consec));
% $$$ 
% $$$ between = 1;
% $$$ consec_pos = 1;
% $$$ all_done = 0;
% $$$ while 1   % iterate over runs
% $$$   starti (between) = consec (consec_pos);
% $$$   while 1   % iterate inside runs
% $$$     if consec_pos + 1 == last_consec  % end of consec array
% $$$       % assign to the last end entry, then end.
% $$$       endi (between) = consec (consec_pos+1) + 1;
% $$$       all_done = 1;
% $$$       break
% $$$     elseif consec(consec_pos) + 1 ~= consec (consec_pos + 1)
% $$$       % run has ended
% $$$       endi (between) = consec(consec_pos) + 1;
% $$$       consec_pos = consec_pos + 1;
% $$$       between = between + 1;
% $$$       break % out from inside run
% $$$     else
% $$$       % run is continuing
% $$$       consec_pos = consec_pos + 1;
% $$$     end
% $$$   end
% $$$   if all_done == 1
% $$$     break
% $$$   end
% $$$ end

% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% OLD COMMENTS: from when I was trying to get this to work with
% matrices, not just vectors.  That should happen in the future but
% will require substantial work.  
%
% NOTES
%  runs cannot extend from one row to the next.
%
% EXAMPLES
%  inmatrix = [ 0  0  0  1  2  3  3;
%               0  4  8 10 12 14 24;
%              25 26 27 20  0  0  0 ];
%
%  find_consecutive (inmatrix, 1): starti = [ 3  7 ]
%                                  endi   = [ 9 16 ]
%  find_consecutive (inmatrix, 0): starti = [ 1 16 15 ]
%                                  endi   = [ 7 19 21 ]
%  find_consecutive (inmatrix, 2): starti = [ 5  ]
%                                  endi   = [ 17 ]
% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 
  

  












function cellout = generalized_batch (cellin, display_status)
%generalized_batch (ps-utils): call a series of functions
% function cellout = generalized_batch (cellin, display_status)
% 
% cellin is a cell array where each row has the following format:
% { function_name nargsout function_parameters }
%     function_name is a string specifying the function to call
%     nargsout is the number of output arguments you want returned
%        from that function.
%     function_parameters is a cell array of the arguments you want
%     passed to the function.
%
% If display_status is defined at all, this function displays which
% row of cellin it is executing.
%
% cellout is a cell row vector which contains the output of each
% function call as a cell vector.  
%
% The point of using nested cell arrays on output is to allow
% functions with different numbers of output arguments to be mixed
% in a single call to generalized_batch without having to pad
% cellout with empty matrices.
%
% BUGS
%   No checks for argument validity now.
%
%   Incredibly long explanation for very little code because of
%   complicated input/output transforms into cell matrix.  Is this
%   function really necessary?
% 
% EXAMPLE
%   To return the row and column indices where ones are found
% in certain arrays, use the following rows of cellin:
%   {'find' 2 { [0 1; 0 0] } ; ... 
%    'find' 1 { [0 0 0 1 ] }
% This function will return the following rows of cellout:
%   { {[1] [2]} 
%     {[4]}     }
%
% $Id: generalized_batch.m 125 2008-03-20 20:19:22Z vincent $

[inrows incols] = size (cellin);
%outputsizes = sqcell_to_num (cellin (:,2));
cellout = cell (inrows, 1);

for ifunc = 1:inrows,
  tempout = cell (1,cellin{ifunc, 2}); % create output vector with
                                       % same size as number of
                                       % output arguments desired.
  allinputs = cellin {ifunc, 3};

  % do status output
  if exist ('display_status')
    if isgraphical
      if ifunc == 1
        w=waitbar (1/inrows, 'generalized\_batch: Processing rows');
      else
        waitbar (ifunc/inrows, w);
      end
    else
      disp (sprintf ('generalized_batch: row %d of %d', ifunc, inrows));
    end
  end

  [ tempout{:} ] = feval (cellin{ifunc, 1}, allinputs{:});
  cellout (ifunc) = {tempout};
end
if isgraphical, close (w); end







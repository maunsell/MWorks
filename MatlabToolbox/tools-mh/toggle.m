function new = toggle (old)
%TOGGLE (ps-utils): changes the state of a boolean var.
%   NEW = TOGGLE (OLD)
%   Examples:
%   toggle (1) == 0 
%   toggle (0) == 1
%   toggle (548) == 0
%
%$Id: toggle.m 125 2008-03-20 20:19:22Z vincent $

if old == 0
  new = 1;
else
  new = 0;
end

  
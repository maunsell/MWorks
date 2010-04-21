function outbool = isnumber (instr)
%isnumber (ps-utils): true for numbers in strings.
%
% USAGE
%  function outbool = isnumber ()
%
% true for characters like '1', false for [1].
% See also ISLETTER.
% This is likely slow, isletter is implemented as an internal
% function.
%
% $Id: isnumber.m 125 2008-03-20 20:19:22Z vincent $

numbers = '1234567890';
outbool = [];

if ~ischar(instr)
  warning ('Parameter is not a string, returning []');
  return
end

outbool = ismember (instr, numbers);




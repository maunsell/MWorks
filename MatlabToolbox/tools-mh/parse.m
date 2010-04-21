function output = parse (input, delimiters)
%PARSE (ps-utils): break input string into a series of words, using strtok.
%   OUTPUT = PARSE (INPUT, DELIMITERS)
%   words in input are broken at any character found in
%   delimiters.   delimiters defaults to whitespace: spaces,
%   tabs, and newlines.
%
%   delimiters in input get chomped by this routine and do not
%   appear in output.
% 
%$Id: parse.m 125 2008-03-20 20:19:22Z vincent $

remain = input;
output = [];
while 1,
  if nargin == 1
    [word remain] = strtok (remain);
  else 
    [word remain] = strtok (remain, delimiters);
  end
  output = strvcat (output, word);
  if isempty (remain)
    break
  end
end


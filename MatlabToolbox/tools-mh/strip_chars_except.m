function outstr = strip_chars_except (instr, chars_to_not_strip);
%strip_chars_except (ps-utils): remove all chars not specified from str
%
% USAGE
%  function outstr = strip_chars_except (instr, chars_to_not_strip);
%
% $Id: strip_chars_except.m 125 2008-03-20 20:19:22Z vincent $

outstr = [];
for i = 1:size(instr,1)
  boolsleft = ismember (instr(i,:), chars_to_not_strip);
  indicesleft = find(boolsleft);
  outline = instr (i,indicesleft);
  outstr = strvcat (outstr, outline);
end


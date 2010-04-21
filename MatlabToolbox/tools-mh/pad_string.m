function outstr = pad_string (instr, size, padchar)
%PAD_STRING (ps-utils): make a string exactly a certain size
%   OUTSTR = PAD_STRING (INSTR, SIZE, PADCHAR)
%
%$Id: pad_string.m 125 2008-03-20 20:19:22Z vincent $

% The old way (pads with blanks)
%formatstr = sprintf ('%%-%ds', size);
%outstr = sprintf (formatstr, instr);

%% The new way (pads with arbitrary char, also works with matrices not 
%% just single strings)
if iscellstr(instr), error ('We don''t handle cellstr''s yet.'); end
if isempty(instr), instr = 0; end
[rs cs] = size(instr);
outstr = char (ones(rs, size) .* double(padchar));
outstr(1:end, 1:cs) = instr;


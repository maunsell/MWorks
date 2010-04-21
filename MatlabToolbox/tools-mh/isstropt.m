function boolOut = isstropt(putative_stropt)
%ISSTROPT (ps-utils): does this parameter look like a stropt cell array?
%   boolOut = ISSTROPT(PUTATIVE_STROPT)
%   Used for stropt param checking
%
%$Id: isstropt.m 125 2008-03-20 20:19:22Z vincent $

if ~(isempty(putative_stropt) ...
     || (mod(length(putative_stropt),2) == 0 ...
         && all(cellfun('isclass',putative_stropt(1:2:end),'char'))))
    boolOut = false;
else
    boolOut = true;
end

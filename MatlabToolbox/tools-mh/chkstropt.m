function chkstropt(putative_stropt)
%CHKSTROPT (ps-utils): does this parameter look like a stropt cell array?
%   CHKSTROPT(PUTATIVE_STROPT)
%   Used for stropt param checking
%
%$Id: chkstropt.m 319 2008-08-20 22:14:21Z histed $

if ~isstropt(putative_stropt)
    strfromcell = evalc('disp(putative_stropt)');
    error('This doesn''t look like a stropt:\n%s', ...
          strfromcell);
end
tNames = putative_stropt(1:2:end);
if length(unique(lower(tNames))) ~= length(tNames)
    error('Duplicate names in stropt');
end



function assert (expr, errmsg, errid, cannotdisable)
%ASSERT (ps-utils): test condition, if it fails, error()
%   ASSERT (EXPR, ERRMSG, ERRID, CANNOTDISABLE)
% 
%$Id: assertMH.m 90 2008-03-14 18:48:02Z histed $

% dumb since the main cost is calling this function, not the code inside
%global NO_ASSERTIONS  

% deal with arguments
if nargin < 2 || isempty(errmsg)
    errmsg = 'Assertion failed!  Use ''dbstop if error'' to investigate'; 
    show_source_lines = 1;    % only display source lines if no message specified.
else
    show_source_lines = 0;
end
if nargin < 3 || isempty(errid), errid = 'PSUTILS:assertion'; end
% ignore arg 4

% do assertion
if ~isempty(expr) 
    if prod(size(expr)) > 1
        error ('Assert expression must be convertible to logical scalar');
    end
    if ~(expr)
        if show_source_lines
            extralines = 2;
            
            [st,i] = dbstack;
            inMFile = st(i+1).file;
            inMFile = strrep(lower(inMFile), '.m', ''); % remove ext
            
% $$$                 % look for and remove a subfunction name
% $$$                 [sIx,eIx]=regexp(inMFile,' \(.*\)$');
% $$$                 if ~isempty(sIx)
% $$$                     inMFile = inMFile(1:sIx-1);
% $$$                 end

            disp('Assertion at:')
            dbtype(inMFile, [num2str(st(i+1).line),':', ...
                             num2str(st(i+1).line + extralines)]);
            dbstack
        end
        error (errid, errmsg);
    end
end



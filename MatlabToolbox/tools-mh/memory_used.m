function b = memory_used(baseOrCurrentStr)
%
%   baseOrCurrentStr can be 'base', 'current', or 'both'
%  
%$Id: memory_used.m 125 2008-03-20 20:19:22Z vincent $


if nargin < 1, baseOrCurrentStr = 'both'; end

boolOut = ismember({'base','current','both'}, baseOrCurrentStr);
doBase = boolOut(1);
doCurrent = boolOut(2);
if boolOut(3), 
    doBase=true; 
    if ~strcmp(caller_mfilename(0), 'base')
        % we're not in the base workspace
        % this is here so we don't count twice
        doCurrent=true; 
    end
end

b = 0;
if doBase
    s=evalin('base', 'whos;');
    if ~isempty(s)
        b = b+sum(cat(1,s.bytes));
    end
end
if doCurrent
    s=evalin('caller', 'whos;');
    if ~isempty(s)
        b = b+sum(cat(1,s.bytes));
    end
end

return


   

function ppStr = pp_hash_key(inKey, maxChars)
%PP_HASH_KEY (ps-utils): pretty print a hash key to a str for display
%
%$Id: pp_hash_key.m 125 2008-03-20 20:19:22Z vincent $

if isvector(inKey), inKey = rowvect(inKey); end
if iscell(inKey) && length(inKey) == 1, inKey = inKey{1}; end

% unwrap cell vectors with one element
if iscell(inKey) 
    nEls = prod(size(inKey));
    for iE=1:nEls
        if iscell(inKey{iE}) && length(inKey{iE}) == 1
            inKey{iE} = inKey{iE}{1};
        end
    end
end


% use matlab's display routine
ppLargeStr = evalc('disp(inKey)');

% pull out only the first line
ppStr = ppLargeStr(1,:);
if regexp(ppStr, '.*Columns .* through .*')
    % first line is a header, pull out second line
    [crap1,crap2,crap3,rStrs]=regexp(ppStr,'\n[^\n]*\n');
    ppStr = rStrs{1};
end

% strip leading blanks
ppStr = fliplr(deblank(fliplr(ppStr))); 
% remove multiple blanks
ppStr = regexprep(ppStr, ' *', ' '); 
% remove newlines
ppStr = regexprep(ppStr, '\n', ''); 

% wrap with chars to indicate type of matrix
if iscell(inKey)
    ppStr = [ '{' ppStr '}' ];
elseif isnumeric(inKey)
    ppStr = [ '[' ppStr ']' ];        
elseif isstr(inKey)
    ppStr = [ '''' ppStr '''' ];            
end

if length(ppStr) > maxChars  % truncate if too long
    ppStr = [ ppStr(1:(maxChars-4)) ' ...' ];
end

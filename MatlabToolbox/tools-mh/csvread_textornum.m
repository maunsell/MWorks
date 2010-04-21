function dat = csvread_textornum(filename, leaveUnknownAsText)
%CSVREAD_TEXTORNUM (ps-utils): read CSV file, converting numeric fields to double
%  DAT = CSVREAD_TEXTORNUM(FILENAME)
%  DAT is a structure with fields corresponding to the columns of the CSV
%  file. 
%  
%  File format:  
%  The first line must be a header line and specifies the names of the fields in
%  the structure.  (Note: no spaces allowed as they are not allowed in structure
%  names)
% 
%  Each column is checked to see whether it is text or numeric.  
%    If text, a cellstr vector is returned.  
%    If numeric, a numeric column vector is returned.  
%    If both text and numeric values occur in a column, an error results.
%        (to just return this as a cell, set leaveUnknownAsText = true)
%    Missing values result in NaN
% 
%  Data is cached to avoid multiple reads, should be transparent to user.
% 
%$Id: csvread_textornum.m 253 2008-06-06 18:38:39Z histed $

if nargin < 2, leaveUnknownAsText = false; end

%%% check mem cache %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
cacheKey = {mfilename,filename,leaveUnknownAsText}; 
outdatedKey = {dir(filename)}; 
cDat = memory_cache('get', cacheKey, 'allowMissing', outdatedKey);
if ~isempty(cDat), [dat] = deal(cDat{:}); return; end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Make sure index file exists on disk
if nargin<1, filename = []; end
if ~exist(filename, 'file')
    error('Missing CSV file: %s', filename);
end
if nargin < 2, leaveUnknownAsText=false; end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Read index from disk

% first get header
fid = fopen(filename);
headertxt = fgetl(fid);
colnames = deblank_bothsides(strread(headertxt, '%q', 'delimiter', ','));
ncols = length(colnames);
formatstr = repmat('%q',1,ncols);

% now read data
[d{1:ncols}] = textread(filename, ...
                        formatstr, ...
                        'delimiter', ',', ...
                        'headerlines', 1, ...
                        'emptyvalue', NaN);

% special-case: when last cell in last row is empty, re-add it
colLengths = cellfun(@length, d);
if all(colLengths(1:end-1) == colLengths(end)+1)
    d{end}{end+1} = [];
end


% deal with invalid fieldnames
for iCol = 1:ncols
    tName = colnames{iCol};
    if ~isvarname(tName)
        % first check for em
        
        colnames{iCol} = genvarname(tName,colnames);
        disp(sprintf('Invalid field name "%s" -> "%s"', ...
            tName, colnames{iCol}));
    end
end
     
        

% convert numeric fields to numeric and text to text
fieldVals = {};
for iCol = 1:ncols
    tFieldVal = d{iCol};
    emptyIx = cellfun('isempty', tFieldVal);
    numField = str2double(tFieldVal);
    
    % is this really a numeric field?
    textIx = isnan(numField) & ~emptyIx;
    numIx = ~isnan(numField);
    assert(all(numIx == (~textIx & ~emptyIx)), ...
           'Bug: I made a wrong assumption about str2double and NaN results?');
    numIx = find(numIx); 
    textIx = find(textIx);
    if ~isempty(numIx) && isempty(textIx)
        % numeric field
        fieldVals{iCol} = numField;
    elseif (leaveUnknownAsText ...
            || ( (~isempty(textIx) && isempty(numIx)) ...   % text field
                 || (isempty(textIx) && isempty(numIx)) ) )  %   all empty
        fieldVals{iCol} = deblank(tFieldVal);  % TEXTREAD converts to string
                                               % (remove trailing whitespace)
    else
        error([ 'Both string and numeric values were found in ' ...
                'index field: %s' ], ...
              colnames{iCol});
    end
end



% convert to a structure
dat = cell2struct(fieldVals, colnames, 2);

%%% store data in cache %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
memory_cache('set', {dat}, cacheKey,outdatedKey,'allowDups');
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


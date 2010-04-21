function dat = csvread_dataframe(filename)
%CSVREAD_DATAFRAME (ps-utils): read CSV file outputted by R as a dataframe
%  DAT = CSVREAD_DATAFRAME(FILENAME)
%  DAT is a structure
%  
%  write dataframe like this:
% > write.table(nbSum$coefficients, '/tmp/t1.txt', sep=',')
%
%$Id: csvread_dataframe.m 125 2008-03-20 20:19:22Z vincent $

% first get header
fid = fopen(filename);
headertxt = fgetl(fid);
colnames = deblank_bothsides(strread(headertxt, '%q', 'delimiter', ','));
ncols = length(colnames)+1;
formatstr = repmat('%q',1,ncols);

% now read data
[d{1:ncols}] = textread(filename, ...
                        formatstr, ...
                        'delimiter', ',', ...
                        'headerlines', 1, ...
                        'emptyvalue', NaN);


% convert numeric fields to numeric and text to text
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
    elseif ( (~isempty(textIx) && isempty(numIx)) ...   % text field
             || (isempty(textIx) && isempty(numIx)) )   % entire field empty
        fieldVals{iCol} = deblank(tFieldVal);  % TEXTREAD converts to string
                                               % (remove trailing whitespace)
    else
        error(sprintf(['Both string and numeric values were found in ' ...
                       'index field: %s'], ...
                      colnames{iCol}));
    end
end

% convert to a more raw structure
dat.colNames = colnames;
dat.rowNames = fieldVals(:,1);
dat.columns = fieldVals(:,2:end);


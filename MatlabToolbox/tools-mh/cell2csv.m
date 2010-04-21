function cell2csv(datName,cellArray,trennZeichen,quoteChar)
%CELL2CSV (ps-utils) Writes Cell-Array content into csv.
%
%   CELL2CSV(DATNAME,CELLARRAY,TRENNZEICHEN,QUOTECHAR)
% 
% DATNAME = Name of the file to save. [ i.e. 'text.csv' ]
% CELLARRAY = Name of the Cell Array where the data is in
% TRENNZEICHEN = seperating sign, normally:',' it's default
% QUOTECHAR default '"', character to use to quote strings
%
% $Id: cell2csv.m 125 2008-03-20 20:19:22Z vincent $
% based on code by Sylvain Fiedler, KA, 2004


if nargin < 3 || isempty(trennZeichen)
    trennZeichen = ',';
end
if nargin < 4 || isempty(quoteChar)
    quoteChar = '"';
end


datei = fopen(datName,'w');
for z=1:size(cellArray,1)
    for s=1:size(cellArray,2)
        
        var = eval(['cellArray{z,s}']);
        
        var = var(:);
        if length(var) == 0
            % empty
            var = '';
        elseif isnumeric(var)
            var = num2str(var);
        else
            % character, surround with quotes
            var = [quoteChar; var; quoteChar];
        end
        
        fprintf(datei,var);
        
        if s ~= size(cellArray,2)
            fprintf(datei,trennZeichen);
        end
    end
    fprintf(datei,'\n');
end
fclose(datei);
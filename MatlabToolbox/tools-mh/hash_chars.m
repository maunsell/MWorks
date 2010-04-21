function outVect = hash_chars(inStrs)
%HASH_CHARS (ps-utils): hash function for ASCII strings
%
%   Based on the CRC hash function
%
%$Id: hash_chars.m 125 2008-03-20 20:19:22Z vincent $

% some profiling done but this could be faster...

if iscellstr(inStrs)
    inStrs = char(inStrs);
end
inStrs = uint32(inStrs);

[nRows,nCols] = size(inStrs);
outVect = zeros(nRows,1,'uint32');
for i=1:nCols
    highorder = bitand(outVect, 4160749568);       % hex2dec('f8000000')
    outVect = bitshift(outVect, 5);
    outVect = bitxor(outVect, bitshift(highorder, -27));
    outVect = bitxor(outVect, inStrs(:,i));
end


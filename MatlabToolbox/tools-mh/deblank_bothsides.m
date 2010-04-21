function strOut = deblank_bothsides(strIn)
%DEBLANK_BOTHSIDES (ps-utils): remove leading and trailing blanks from a string
%   strOut = DEBLANK_BOTHSIDES(strIn)
%
%$Id: deblank_bothsides.m 125 2008-03-20 20:19:22Z vincent $

if iscellstr(strIn)
    nStrs = length(strIn);
    strOut = cell(1,nStrs);
    for i=1:nStrs
        strOut{i} = deblank(fliplr(deblank(fliplr(strIn{i}))));
    end
    return
elseif isstr(strIn)
    strOut = deblank(fliplr(deblank(fliplr(strIn))));
    return
else
    error('Not a string');
end


   
function fixed_name = fontname_fixed
%FONTNAME_FIXED (ps-utils): get the name of a fixed-width font on this system
%  NAME = FONTNAME_FIXED
%
%$Id: fontname_fixed.m 125 2008-03-20 20:19:22Z vincent $

l = listfonts;

% must be lowercase
possFontNames = { 'lucidatypewriter', ... 
                  'courier new', ...
                  'courier' };

lowerL = lower(l);
for i=1:length(possFontNames)
    tIx = strcmp(lowerL, possFontNames{i});
    if any(tIx)
        desN = find(tIx);
        assert(length(desN) == 1);
        break;
    end
end
fixed_name = l{desN};
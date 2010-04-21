function outStr = seconds2str(inSec, formatStr, stripEndZeros)
%SECONDS2STR (ps-utils): convert num seconds to 'NNdNNhNNmNNs'
%   outStr = SECONDS2STR(inSec, formatStr)
%
%   formatStr gets used for each element, defaults to '%d%1s'.
%       For zero-padded nums use '%02d1s'.
%       For spaces between elements use '%d1s '
%
%   See also: NUM2STR_METRIC, DATE, CLOCK, ETIME
%
%$Id: seconds2str.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 2 || isempty(formatStr), formatStr = '%d%1s'; end
if nargin < 3, stripEndZeros = false; end
nEls = 5;

[els{1:nEls}] = subModSeconds(inSec);

outStr = '';
chars = {'y', 'd', 'h', 'm', 's'};

% skip 0 els at beginning
startN = find(cumsum(cat(2,els{:})) == 0, 1, 'last');
if isempty(startN), startN=0; end
startN = startN+1;
% and end if desired
if stripEndZeros
    endFrontN = find(cumsum(fliplr(cat(2,els{:})))==0, 1, 'last');
    if isempty(endFrontN), endFrontN = 0; end
    endN = nEls - endFrontN;
else
    endN = nEls;
end
% iterate and format
for i=startN:endN
    % check for fractional seconds 
    if i == 5 && ~iswithintol(mod(els{i},1), 0, eps*10)
        error('Seconds must be integral value: use ROUND on input');
    end

    outStr = cat(2, outStr, sprintf(formatStr, els{i}, chars{i}));
end

return

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    

function [nYears nDays nHours nMinutes nSeconds] = subModSeconds(inSec)
sec = inSec;

nYears = floor(sec / (365*60*60*24));  
sec = mod(sec, 365*60*60*24);

nDays = floor(sec / (60*60*24));  
sec = mod(sec, 60*60*24);

nHours = floor(sec / (60*60));
sec = mod(sec, 60*60);

nMinutes = floor(sec / 60);
sec = mod(sec, 60);

nSeconds = sec;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    

function outSec = subChopSeconds(inSec, sigDigits)

mult = [ 3.65 10 10 2.4 10 6 10 6 10 ];
nMult = length(mult);

for i=1:(nMult-1)
    if cumprod(mult(1:i)) >= inSec & cumprod(mult(1:(i+1))) < inSec
        firstMultLess = i+1;
        break;
    end
end
lastMult = max(firstMultLess+sigDigits, nMult);
divisor = cumprod(mult(firstMultLess-1:lastMult)); 
outSec = round(inSec ./ divisor) .* divisor

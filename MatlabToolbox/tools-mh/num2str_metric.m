function outStr = num2str_metric(inNum, numSigDigits, formatStr)
%NUM2STR_METRIC: convert num to a str with metrix postfixes
%   outStr = num2str_metric(inNum)
%
%   if inNum is between 10^3 and 10^6, M will be appended, etc.
%
%$Id: num2str_metric.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 2, numSigDigits = []; end
if nargin < 3, formatStr = '%g'; end

if inNum == 0; outStr = '0'; return; end % special case->log(0) bad

minPower10 = floor(log10(inNum));

if minPower10 >= -15 && minPower10 < -12
    postfix = 'f';
    powOf10 = -15;
elseif minPower10 >= -12 && minPower10 < -9
    postfix = 'p';
    powOf10 = -12;
elseif minPower10 >= -9 && minPower10 < -6
    postfix = 'n';
    powOf10 = -9;
elseif minPower10 >= -6 && minPower10 < -3
    postfix = 'u';
    powOf10 = -6;
elseif minPower10 >= -3 && minPower10 < 0
    postfix = 'm';    
    powOf10 = -3;
elseif minPower10 >= 0 && minPower10 < 3
    % do nothing, less than one thousand
    postfix = '';
    powOf10 = 0;
elseif minPower10 >= 3 && minPower10 < 6
    postfix = 'k';
    powOf10 = 3;
elseif minPower10 >= 6 && minPower10 < 9
    postfix = 'M';
    powOf10 = 6;
elseif minPower10 >= 9 && minPower10 < 12
    postfix = 'G';
    powOf10 = 9;
elseif minPower10 >= 12 && minPower10 < 15
    postfix = 'T';
    powOf10 = 12;
end

convNum = inNum ./ 10.^powOf10;
if ~isempty(numSigDigits)
    convNum = chop(convNum, numSigDigits);
end
outStr = sprintf([formatStr postfix], convNum);

if any(lower(outStr) == 'e')
    warning('Number out of range, define more metric prefixes');
    % fallback
    outStr = sprintf('%g', inNum);
end


    

    
    

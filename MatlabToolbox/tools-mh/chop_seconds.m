function outSec = chop_seconds(inSec, sigDigits)
%CHOP_SECONDS (ps-utils): Sig digits of YYHHMMSS time representation
%
%   outSec = chop_seconds(inSec, sigDigits)
%
%   Example:
%   >> secs=60*60+111
%   >> chopped = chop_seconds(secs,3)
%   chopped =
%           3720
%   >> {seconds2str(secs), seconds2str(chopped)}
%   ans = 
%       '1h1m51s'    '1h2m0s'
%
%   See also SECONDS2STR, CHOP
%
%$Id: chop_seconds.m 125 2008-03-20 20:19:22Z vincent $

if sigDigits <= 1, error('Invalid sigDigits: %d', sigDigits); end

mult = [ 3.65 10 10 2.4 10 6 10 6 10 1 ];
nMult = length(mult);

if inSec == 0; outSec = 0; return; end
if abs(inSec) < 1, outSec = chop(inSec, sigDigits); return; end
for i=1:(nMult-1)
    if prod(mult(i:end)) > inSec & prod(mult((i+1):end)) <= inSec
        firstMultLess = i+1;
        break;
    end
end

firstMult = min(firstMultLess+sigDigits-1, nMult);
divisor = prod(mult(firstMult:end));
outSec = round(inSec ./ divisor) .* divisor;

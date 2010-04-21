function theta = atan3(y,x)
%ATAN3 (ps-utils): convert y and x values into theta in range [0,2*pi)
%   THETA = ATAN3(Y,X)
%
%   See also ATAN2, ATAN
%
%$Id: atan3.m 125 2008-03-20 20:19:22Z vincent $
theta = atan2(y,x);
if theta < 0, theta = 2*pi + theta ; end

% use to ensure this is correct, can be removed eventually
assert( (x >= 0 && y >= 0 && theta >= 0 && theta <= pi/2) ...
        || (x < 0 && y >= 0 && theta > pi/2 && theta <= pi) ...
        || (x < 0 && y < 0 && theta > pi && theta < 3*pi/2) ...
        || (x >= 0 && y < 0 && theta >= 3*pi/2 && theta < 2*pi ) );
function outAngles = angle_map_to_zero_2pi(inAngles)
%ANGLE_MAP_TO_ZERO_2PI (ps-utils) remap thetas to [0,2pi) range
%
%   outAngles = ANGLE_MAP_TO_ZERO_2PI(inAngles)
%
%Id%

outAngles = inAngles;

% find number of multiples of two pi
nTwoPis = floor(outAngles ./ (2*pi));

outAngles = outAngles - nTwoPis*2*pi;
outAngles(outAngles == 2*pi) = 0;  % this can happen if input is -0.0000 (?)

% $$$ if ~(all( (outAngles >= 0 & outAngles < 2*pi) ...
% $$$           | isnan(outAngles))), 
% $$$     error('Assert: output of this function should be in [0, 2*pi)');
% $$$ end




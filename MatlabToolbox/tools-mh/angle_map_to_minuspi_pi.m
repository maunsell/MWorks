function outAngles = angle_map_to_minuspi_pi(inAngles)
%ANGLE_MAP_TO_MINUSPI_PI (ps-utils) remap thetas to [-pi,pi) range
%
%   outAngles = ANGLE_MAP_TO_MINUSPI_PI(inAngles)
%
%Id%

outAngles = angle_map_to_zero_2pi(inAngles);
outAngles(outAngles >= pi) = outAngles(outAngles >= pi) - (2*pi);

% was an assertion; taken out for speed
% $$$ if ~all( ((outAngles < pi) & (outAngles >= -pi)) ...
% $$$          | isnan(outAngles)), 
% $$$     error('Assert: all outputs from this fn should be in range [-pi,pi)');
% $$$ end




function textHandles = labels_between_ticks(labelStrings, whichAxis, tickLocations)
%LABELS_BETWEEN_TICKS (ps-utils): draw labels on axis and ticks between them
%   TEXTHANDLES = LABELS_BETWEEN_TICKS(labelStrings, whichAxis, tickLocations)
%   labelstrings: char array or cellstring of tick axis labels
%   whichAxis: 'X' or 'Y'
%   tickLocations: numeric vector of length length(labelStrings)+1: where on
%       the y axis to put the ticks.  Default: space them evenly on the axis
% 
%   Only works for 2-d axes now.
%
%$Id: labels_between_ticks.m 125 2008-03-20 20:19:22Z vincent $

% check arguments
if nargin < 3, tickLocations = []; end

switch whichAxis
    case 'X'
        thisAxis = 'X';
        otherAxis = 'Y';
    case 'Y'
        thisAxis = 'Y'; 
        otherAxis = 'X';
    otherwise 
        error('Invalid whichAxis: %s', mat2str(whichAxis));
end

tLimStr = sprintf('%1sLim', thisAxis);
oLimStr = sprintf('%1sLim', otherAxis);
tTickStr = sprintf('%1sTick', thisAxis);
tTickLabelStr = sprintf('%1sTickLabel', thisAxis);

tLim = get(gca, tLimStr);
oLim = get(gca, oLimStr);

nLabels = length(labelStrings);

if iscell(labelStrings)
    labelStrings = char(labelStrings);
end
blankLabelString = repmat(' ', 1, size(labelStrings,2)+2);

% compute tickLocations (if necessary), labelLocations
if isempty(tickLocations)
    tickLocations = linspace(tLim(1), tLim(2), nLabels+1);
end
assert(length(tickLocations) == length(labelStrings)+1)
labelLocations = diff(tickLocations)./2 + tickLocations(1:end-1);

% draw ticks
set(gca, tTickStr, tickLocations);
% put a blank label string on the ticks so axis titles are positioned correctly
set(gca, tTickLabelStr, blankLabelString); 
        
% put label text in place manually
oPos = -0.025*(oLim(2)-oLim(1)) + oLim(1);  % offset sl. outside of axis
tPos = labelLocations;

for iLab=1:nLabels
    if thisAxis == 'X'
        fullpos = [tPos(iLab), oPos];
    elseif thisAxis == 'Y'
        fullpos = [oPos, tPos(iLab)];
    end
    
    tH(iLab,1) = text('Position', fullpos, ...
                      'String', labelStrings(iLab,:), ...
                      'HorizontalAlignment', 'right', ...
                      'FontSize', 10, ...
                      'Tag', 'labels_between_ticks');
end
        
textHandles = tH;

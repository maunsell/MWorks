function textH = axis_label_radians(axisLim, nTicks, whichAxis)
%AXIS_LABEL_RADIANS (ps-utils): draw radian tick labels (with \pi's)
%
%   textH = axis_label_radians(axisLim, nTicks, whichAxis)
%
%   whichAxis should be 'X' or 'Y': defaults to X.  No support for 3d labels
%   yet. 
%   nTicks counts the first and last, so nTicks == 3 gives a tick at the
%   start and end and one in the middle
%$Id: axis_label_radians.m 125 2008-03-20 20:19:22Z vincent $

% 3d notes: should offset in only one dimension
%   changes: allow whichAxis = Z
%       find orthogonal dimension
%       set text coords in advance: textCoords = [x,y] | [x,y,z]

if nargin < 1 || isempty(axisLim), axisLim = [0 2*pi]; end
if nargin < 2 || isempty(nTicks), nTicks = 7; end
if nargin < 3 || isempty(whichAxis), whichAxis = 'X'; end

oppTextOff = -0.01;

% set axis
whichAxis = upper(whichAxis);
switch whichAxis
    case 'X'
        textCoords = cat(2, linspace(0,1,nTicks)', ...
                         repmat(oppTextOff,nTicks,1));
        horizAlign = 'center';
        vertAlign = 'top';
    case 'Y'
        textCoords = cat(2, repmat(oppTextOff,nTicks,1), ...
                         linspace(0,1,nTicks)');
        horizAlign = 'right';
        vertAlign = 'middle';
    otherwise
        error('Invalid whichAxis: %s; should be X or Y');
end
aLimName = sprintf('%1sLim', whichAxis);
aTickName = sprintf('%1sTick', whichAxis);
aTickLabelName = sprintf('%1sTickLabel', whichAxis);        

tickVals = linspace(axisLim(1), axisLim(2), nTicks);
set(gca, aLimName, axisLim, ...
         aTickName, tickVals, ...
         aTickLabelName, ' ');
         



ratTol = 0.1;
[nums,denoms] = rat(tickVals / pi, ratTol);

for i=1:nTicks
    tNum = nums(i);
    tDenom = denoms(i);
    tCoords = textCoords(i,1:2);
    quot = tNum/tDenom;
    
    if tNum == 0 
        % this is a zero
        tStr = '0';
    elseif tNum == tDenom
        % +1 
        tStr = '\pi';
    elseif tNum == -tDenom
        % -1
        tStr = '-\pi';
    elseif tNum == 1
        % numerator is 1
        tStr = sprintf('\\pi/%d', tDenom);
    elseif tNum == -1
        % numerator is -1
        tStr = sprintf('-\\pi/%d', tDenom);
    elseif iswithintol(quot, round(quot), ratTol/100)
        % this is an integer
        tStr = sprintf('%s\\pi', int2str(round(tNum/tDenom)));
    else
        % ratio of two ints
        tStr = sprintf('%d\\pi/%d', tNum, tDenom);
    end
    textH(i) = text(tCoords(1), tCoords(2), tStr, ...
                    'Units', 'Normalized');
end

% old method:
% $$$ for i=1:nTicks
% $$$     [num,denom] = rat(2*(i-1)/(nTicks-1));
% $$$     if num == 1
% $$$         numStr = '';
% $$$     else
% $$$         numStr = int2str(num);
% $$$     end
% $$$     if denom == 1
% $$$         denomStr = '';
% $$$     else
% $$$         denomStr = ['/', int2str(denom)];
% $$$     end
% $$$     labTxt = strcat(numStr, '\pi', denomStr);
% $$$     textH(i) = text(textCoords(i,1), textCoords(i,2), labTxt, ...
% $$$                     'Units', 'Normalized');
% $$$ end


aFont = get(gca, 'FontName');
set(textH, 'HorizontalAlignment', horizAlign, ...
           'VerticalAlignment', vertAlign, ...
           'FontName', aFont);

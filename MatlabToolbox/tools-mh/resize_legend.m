function resize_legend (hL, resizeFact, whichPartOfLegend)
%RESIZE_LEGEND (ps-utils) - Changes LEGEND fontsize and axes position
%   RESIZE_LEGEND(HL, RESIZEFACT, WHICHPARTOFLEGEND)
%   HL           Legend axes handle
%   RESIZEFACT   Factor by which to resize the legend.
%                Default is 0.8
%   WHICHPARTOFLEGEND cellstr, can contain 'text', 'box', 'markers', 'lines'
%                or 'all'.  Default: 'all'
%
%   Example: 
%   %Make fontsize and legend axes twice bigger
%   hL=legend(......);
%   resize_legend(hL,2);  
%
%
%Based on code by: Jim Phillips and Denis Gilbert, 03-Dec-1999
%$Id: resize_legend.m 125 2008-03-20 20:19:22Z vincent $

%  Notes: 
%     We should really install a ResizeFcn on the figure which calls 
%     LEGEND('ResizeLegend')

if nargin < 2, resizeFact = 0.8; end
if nargin < 3, whichPartOfLegend = 'all'; end

if isempty(hL)
    error('Legend handle is empty: cannot resize');
end


if strcmp(whichPartOfLegend, 'all'), 
    % Must change this line when new parts of legend to resize are added to
    % this function
    whichPartOfLegend = { 'text', 'box', 'markers', 'lines' };
elseif ischar(whichPartOfLegend)
    % make it a cellstr if a single str is passed in
    whichPartOfLegend = {whichPartOfLegend};
end

if any(ismember(whichPartOfLegend, 'text'))                          
    ht = findobj( get(hL,'children'), 'type', 'text');

    % sometimes you end up with some empty strings(no idea why), remove them
    tStrings = get(ht, 'String');
    tRealH = ht(~isempty(tStrings));
    assert(length(tRealH) == 1);

    set(ht, 'FontSize', get(tRealH,'FontSize')*resizeFact)
end

lineH = findobj(get(hL, 'Children'), 'type', 'line');

if any(ismember(whichPartOfLegend, 'markers')) 
    markerType = get(lineH, 'Marker');
    if ~isempty(lineH)
        lineHWithMarkers = lineH(~strcmp(markerType, 'none'));
        if ~isempty(lineHWithMarkers)
            for i=1:length(lineHWithMarkers)
                tH = lineHWithMarkers(i);
                set(tH, 'MarkerSize', get(tH, 'MarkerSize') * resizeFact);
            end
        end
    end
end

if any(ismember(whichPartOfLegend, 'box'))                          
    p = get(hL, 'Position');
    p(3) = p(3)*resizeFact;
    p(4) = p(4)*resizeFact;
    set(hL,'Position', p);
end


% don't do this because it changes the line size in the graph

% $$$ if any(ismember(whichPartOfLegend, 'lines'))
% $$$     lineStyle = get(lineH, 'LineStyle');
% $$$     if ~isempty(lineH)
% $$$         lineHWithLines = lineH(~strcmp(lineStyle, 'none'));
% $$$         assert(~isempty(lineHWithLines));
% $$$         for i=1:length(lineHWithLines)
% $$$             tH = lineHWithLines(i);
% $$$             set(tH, 'LineWidth', get(tH, 'LineWidth') * resizeFact);
% $$$         end
% $$$     end
% $$$ end

% fix the resizeFcn
%set(gcf,'ResizeFcn','doresize(gcbf)')


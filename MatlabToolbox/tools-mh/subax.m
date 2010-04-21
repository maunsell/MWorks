function ax = subax(rows,cols,ind,space)
%SUBAX Create axes in tiled positions
% AX = SUBAX(ROWS,COLS,IND)
%
% See SUBPLOT

if nargin < 4
    space = .25 % percent space between axes.
end

fig = gcf;
children = get(fig,'children');
ax = children(find(strcmp(get(children,'type'),'axes')));

if length(ax)
    cax = ax(end);
else
    cax = gca;
    set(cax,'pos',[0.025,0.025,0.95,0.9]);
% 
%     % Create/find BigAx and make it invisible
%     BigAx = newplot(cax);
%     fig = ancestor(BigAx,'figure');
%     hold_state = ishold(BigAx);
    set(cax,'Visible','off','color','none')
end

% Create and plot into axes
pos = get(cax,'Position');
width = pos(3)/cols;
height = pos(4)/rows;
pos(1:2) = pos(1:2) + space*[width height];
xlim = zeros([rows cols 2]);
ylim = zeros([rows cols 2]);
BigAxHV = get(cax,'HandleVisibility');
BigAxParent = get(cax,'Parent');

% fill figure rowwise
[j,i]=ind2sub([cols,rows],ind);

axPos = [pos(1)+(j-1)*width pos(2)+(rows-i)*height ...
         width*(1-space) height*(1-space)];
         
% findax = findobj(fig,'Type','axes','Position',axPos);
% if isempty(findax),
  ax = axes('Position',axPos,'HandleVisibility',BigAxHV,'parent',BigAxParent);
  set(ax,'visible','on');
% else
%   ax = findax(1);
% end
    
set(ax,'xlimmode','auto','ylimmode','auto','xgrid','off','ygrid','off');

return

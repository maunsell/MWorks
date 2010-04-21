function figH = gaf;
%GAF Get all figure handles
% FIGH = GAF;

h = sort(get(0,'children'));
these = find(strcmp(get(h,'type'),'figure'));

figH = h(these);

return

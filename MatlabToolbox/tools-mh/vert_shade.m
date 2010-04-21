function pH = vert_shade (xvals, colorvals, yminmax)
%VERT_SHADE (ps-utils): draw vertical shaded square gradient on plot
%
%   PH = VERT_SHADE (XVALS, COLORVALS, YMINMAX)
%   xvals specify the edges of the patches, so the number of patches is
%   length(xvals) - 1
%
%   yminmax(1) is ymin, yminmax(2) is ymax.  Defaults to max and min of current
%   axis. 
%
%   See also VERT_SHADE_GAUSS, VERT_LINES, SHADE, PATCH.
%
%$Id: vert_shade.m 350 2008-10-09 22:02:17Z histed $

% get y range
if nargin < 3 || isempty(yminmax)
    v=axis;  ymin=v(3); ymax=v(4);
else
    ymin = yminmax(1); ymax=yminmax(2);
end
%if length(xvals) == 2, xvals = [xvals(:); flipud(xvals(:))]; end


xvals = colvect(xvals);
numx=length(xvals);

% construct x and y vectors
xinp = zeros(5, numx-1);
xinp(1,:) = xvals(1:end-1)';   % x0
xinp(2,:) = xvals(1:end-1)';   % x0
xinp(3,:) = xvals(2:end)';     % x1
xinp(4,:) = xvals(2:end)';     % x1
xinp(5,:) = xvals(1:end-1)';   % x0

yinp = zeros(5, numx-1);
yinp(1,:) = ymin;  % y0
yinp(2,:) = ymax;  % y1
yinp(3,:) = ymax;  % y1
yinp(4,:) = ymin;  % y0
yinp(5,:) = ymin;  % y0

if range(colorvals,1) == 0
    fColor = colorvals;
else 
    fColor = 'interp';
end

% $$$ pH = patch(xinp, yinp, colorvals, ...
% $$$                 'EdgeColor', 'none', ...
% $$$                 'FaceColor', fColor, ...
% $$$                 'Tag', 'vert_shade patch');
pH = patch(xinp, yinp, colorvals);
nFaces = length(get(pH, 'Faces'));
set(pH, 'EdgeColor', 'none', ...
        'FaceColor', 'interp', ...
        'FaceVertexCData', repmat(colorvals, [nFaces 1]), ...
        'Tag', 'vert_shade patch');

% stack to bottom
anystack(pH, 'bottom');
% $$$ for i=1:length(pH)
% $$$     parent = get(pH(i),'Parent');
% $$$     peers = get(parent, 'Children');
% $$$     peers = [colvect(peers(peers~=pH(i))); pH(i)];
% $$$     set(parent, 'Children', peers);
% $$$     
% $$$     % also bury for flat 3d plots
% $$$     %set(pH(i), 'ZData', [-1,-1,-1,-1]);
% $$$     % bug here, 
% $$$ end


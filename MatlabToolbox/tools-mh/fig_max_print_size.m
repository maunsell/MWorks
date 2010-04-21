function fig_max_print_size (fig_handle)
%FIG_MAX_PRINT_SIZE (ps-utils): set figure to fill entire page when printed
%
%***** Usage
%  function fig_max_print_size (fig_handle)
%
%  fig_handle specifies the figure to change, default is current figure
%
% $Id: fig_max_print_size.m 125 2008-03-20 20:19:22Z vincent $

margin_size = 0.2;  % inches

if nargin < 1
    fig_handle = gcf;
end

set(fig_handle, 'PaperOrientation', 'landscape', ...
                'PaperPositionMode', 'manual', ...
                'PaperUnits', 'inches');
s = get(fig_handle, 'PaperSize');
s = s - (2*margin_size);    % shrink to allow for margins

pos = [ margin_size, margin_size, s(1), s(2) ];
set(fig_handle, 'PaperPosition', pos);

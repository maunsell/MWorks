function comet_speed(x, y, slowdown, p)
%COMET_SPEED (ps-utils): comet-like trajectory with speed control.
%
%   Taken from the matlab COMET utility.
%
%   COMET(Y) displays an animated comet plot of the vector Y.
%   COMET(X,Y) displays an animated comet plot of vector Y vs. X.
%   COMET(X,Y,t) causes the motion to be slowed down by t seconds
%   for each draw iteration.  t can be fractional.
%   COMET(X,Y,t,p) sets a comet length of p.  default is 0.10.
%
%   Example:
%       t = -pi:pi/200:pi;
%       comet(t,tan(sin(t))-sin(tan(t)))
%
%   See also COMET3.

%   Charles R. Denham, MathWorks, 1989.
%   Revised 2-9-92, LS and DTP; 8-18-92, 11-30-92 CBM.
%   Copyright (c) 1984-98 by The MathWorks, Inc.
%   $Revision: 12 $  $Date: 2002-06-07 16:33:04 -0400 (Fri, 07 Jun 2002) $
%
%   $Id: comet_speed.m 125 2008-03-20 20:19:22Z vincent $

if nargin == 0, error('Not enough input arguments.'); end
if nargin < 2, y = x; x = 1:length(y); end
if nargin < 3, slowdown = 0; end
if nargin < 4, p = 0.10; end

ax = newplot;
if ~ishold,
  axis([min(x(isfinite(x))) max(x(isfinite(x))) ...
        min(y(isfinite(y))) max(y(isfinite(y)))])
end
co = get(ax,'colororder');

if size(co,1)>=3,
  % Choose first three colors for head, body, and tail
  head = line('color',co(1,:),'marker','o','erase','xor', ...
              'xdata',x(1),'ydata',y(1));
  body = line('color',co(2,:),'linestyle','-','erase','none', ...
              'xdata',[],'ydata',[]);
  tail = line('color',co(3,:),'linestyle','-','erase','none', ...
              'xdata',[],'ydata',[]);
else
  % Choose first three colors for head, body, and tail
  head = line('color',co(1,:),'marker','o','erase','xor', ...
              'xdata',x(1),'ydata',y(1));
  body = line('color',co(1,:),'linestyle','--','erase','none', ...
              'xdata',[],'ydata',[]);
  tail = line('color',co(1,:),'linestyle','-','erase','none', ...
              'xdata',[],'ydata',[]);
end

m = length(x);
k = round(p*m);

% Grow the body
for i = 2:k+1
   j = i-1:i;
   set(head,'xdata',x(i),'ydata',y(i))
   set(body,'xdata',x(j),'ydata',y(j))
   drawnow
   pause (slowdown);
end

% Primary loop
for i = k+2:m
   j = i-1:i;
   set(head,'xdata',x(i),'ydata',y(i))
   set(body,'xdata',x(j),'ydata',y(j))
   set(tail,'xdata',x(j-k),'ydata',y(j-k))
   drawnow
   pause (slowdown);
end

% Clean up the tail
for i = m+1:m+k
   j = i-1:i;
   set(tail,'xdata',x(j-k),'ydata',y(j-k))
   drawnow
   pause (slowdown);
end

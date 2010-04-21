function yesgraphics = isgraphical;
%isgraphical (ps-utils): Are graphics available or is term text-only?
%
% stores state between calls so subsequent calls are faster
%
% $Id: isgraphical.m 125 2008-03-20 20:19:22Z vincent $

persistent pyesgr;
if isempty (pyesgr)
  size = get(0, 'ScreenSize');
  if size == [1 1 1 1]
    pyesgr = 0;
  else
    pyesgr = 1;
  end
end
yesgraphics = pyesgr;

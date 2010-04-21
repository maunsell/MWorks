function cellP = pathcell
%PATHCELL (ps-utils): return current path in a cellstr
%   cellP = PATHCELL
%
%$Id: pathcell.m 125 2008-03-20 20:19:22Z vincent $

p = path;
p1 = strrep(p, ':', ''',''');
cellP = eval(['{''', p1, '''}']);



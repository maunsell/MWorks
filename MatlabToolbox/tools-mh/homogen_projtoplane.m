function P = homogen_projtoplane(normalVect, centerVect)
%HOMOGEN_PROJTOPLANE (ps-utils): plane projection matrix (homogeneous coords)
%   P = HOMOGEN_PROJTOPLANE(NORMALVECT, CENTERVECT)
%
%   Note that one needs to multiply through on the LEFT side: v*P
%$Id: homogen_projtoplane.m 125 2008-03-20 20:19:22Z vincent $

% See Strang, 1993: "Intro to Linear Algebra"

n = normalVect(:);
assert(iswithintol(norm(n),1));
c = centerVect(:);

Tm = [eye(3), zeros(3,1); -c', 1];
Tp = [eye(3), zeros(3,1); c', 1];
Ps = [ eye(3) - n*n', zeros(3,1); zeros(1,3), 1];

P = Tm*Ps*Tp;
function rotate_patch(patchHandle, whichAxis, deg)
%ROTATE_PATCH (ps-utils): rotate a patch object: instead of buggy ROTATE.m
%   ROTATE_PATCH(H, WHICHAXIS, ALPHA)
%
%   Note: DIRVECT must have three elements, and only [0,0,0] is supported as
%   a center: support for both could easily be added.
%
%   See also: ROTATE (bug: does not rotate VertexNormals)
%   
%$Id: rotate_patch.m 125 2008-03-20 20:19:22Z vincent $

switch whichAxis
    case 'X'
        R = Rx(eye(3), deg, 'degrees');
    case 'Y'
        R = Ry(eye(3), deg, 'degrees');        
    case 'Z'
        R = Rz(eye(3), deg, 'degrees');
    otherwise 
        error;
end
v = get(patchHandle, 'Vertices');
vn = get(patchHandle, 'VertexNormals') - v;
set(patchHandle, 'Vertices', (R*v')');
set(patchHandle, 'VertexNormals', (R*vn')'+v);

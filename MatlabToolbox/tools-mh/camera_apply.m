function camera_apply(cam_struct, axisHandle)
%CAMERA_APPLY (ps-utils): restore position of camera previously saved
%
%   CAMERA_APPLY(CAM_STRUCT, AXISHANDLE)
%
%$Id: camera_apply.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 2, axisHandle = gca; end

cs = cam_struct;

set(axisHandle, 'CameraPosition', cs.CameraPosition, ...
                'CameraTarget', cs.CameraTarget, ...
                'CameraUpVector', cs.CameraUpVector, ...
                'CameraViewAngle', cs.CameraViewAngle);

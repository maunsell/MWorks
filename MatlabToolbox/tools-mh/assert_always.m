function assert_always(varargin)
%ASSERT_ALWAYS (ps-utils): test condition, if it fails, error (cannot disable)
%  ASSERT_ALWAYS(EXPR, ERRMSG, ERRID) 
%
%  Setting the global NO_ASSERTIONS variable has no effect on this function
%
%  See also ASSERT
% 
%$Id: assert_always.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 3, varargin{3} = 'PSUTILS:always_assert_error'; end
% this handles the case where nargin == 1 because setting the 3rd element
% makes the 2d empty if it doesn't already exist

assert(varargin{1:3}, 'cannotdisable');
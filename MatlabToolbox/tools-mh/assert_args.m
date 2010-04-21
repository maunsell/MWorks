function assert_args (expr, errmsg)
%ASSERT_ARGS (ps-utils): test condition.  Use for checking arguments!
%   ASSERT_ARGS (EXPR, ERRMSG)
%
%   Calls ASSERT with errmsg set to 'PSutils:argumentError'
% 
%$Id: assert_args.m 125 2008-03-20 20:19:22Z vincent $

errId = 'PSutils:argumentError';

if nargin < 2, errmsg = 'An input argument is invalid'; end
assert(expr, errmsg, errId, 'cannotdisable');

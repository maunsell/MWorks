function verify_warnings_on
%VERIFY_WARNINGS_ON (ps-utils): if warning state is off, error()
%   VERIFY_WARNINGS_ON
%
%$Id: verify_warnings_on.m 125 2008-03-20 20:19:22Z vincent $

ws = warning;
wId = { ws.identifier };

allState = ws(strcmp(wId, 'all')).state;

if strcmp(allState, 'off')
    error('Global warning state is off');
else
    assert(strcmp(allState, 'on') || strcmp(allState, 'backtrace'), ...
           'Warning state is not ''off'' ''on'' or ''backtrace''');
end



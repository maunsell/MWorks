function wStateStruct = warning_state_save
%WARNING_STATE_SAVE (ps-utils): save ALL possible warning state
%
%  wStateStruct = warning_state_save
%
%  This is needed because backtrace, verbose, etc. warning settings are not
%  saved by doing W = WARNING.
%
%$Id: warning_state_save.m 125 2008-03-20 20:19:22Z vincent $

w0 = warning('query', 'all');
w2 = warning('query', 'verbose');
w3 = warning('query', 'backtrace');

wStateStruct = cat(1, w0, w2, w3);

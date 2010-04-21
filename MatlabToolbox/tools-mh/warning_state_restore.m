function warning_state_restore(wStateStruct)
%WARNING_STATE_RESTORE (ps-utils): restore ALL possible warning state
%
%   warning_state_restore(wStateStruct)
%
%   wStateStruct must be the output of warning_state_save.
%
%   See also WARNING_STATE_SAVE
%
%$Id: warning_state_restore.m 125 2008-03-20 20:19:22Z vincent $

% This is simple, but this function exists because we may need to do more
% complicated things in the future (i.e. handle different matlab versions)
warning(wStateStruct);

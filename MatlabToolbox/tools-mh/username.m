function outName = username
%USERNAME (ps-utils): return user name of the caller
%   T = USERNAME returns user name of the caller 
%
% by Vincent Bonin 
% based on HOSTNAME by Mark Histed
%$Id: username.m 452 2009-01-21 19:26:11Z vincent $

outName = getenv('USERNAME');

return;

% PREVIOUS CODE, DID NOT WORK ON XP PRO HOSTS.

% % cache check
% [retval,tName] = system('whoami');
% 
% % some error checks
% assert(retval == 0, 'Error running hostname');
% 
% [pathstr,name]=fileparts(tName);
% 
% outName = deblank(name);

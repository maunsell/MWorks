function wc = wildcard
%WILDCARD (ps-utils): return wildcard character for present platform
%
%$Id: wildcard.m 125 2008-03-20 20:19:22Z vincent $

if isunix
  wc = '*';
else % assume windows!
  wc = '*.*';
end
  
 
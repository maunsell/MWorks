function outName = hostname
%HOSTNAME (ps-utils): return hostname of machine matlab is running on
%   T = HOSTNAME returns the hostname of the current machine.  If FQDN
%   Only the hostname (without domain) is returned.
%
%$Id: hostname.m 125 2008-03-20 20:19:22Z vincent $

% cache check
persistent cached_name
if ~isempty(cached_name), outName=cached_name; return; end

if ismac
    [retval,tName] = system('scutil --get ComputerName');
else
    [retval,tName] = system('hostname');
end

% some error checks
assert(retval == 0, 'Error running hostname');
assert(~any(tName == '.'), 'Dots found in hostname: is it a fqdn?');

outName = deblank(tName);

%load cache
cached_name=outName;


% Old fqdn code below

% persistent cached_name cached_fqdn
% if nargin == 0, fqdn = 0; end
%  
% % check cache
% if fqdn
%     if ~isempty(cached_fqdn)
%         T = cached_fqdn; 
%         return
%     end
% else 
%     if ~isempty(cached_name)
%         T = cached_name;
%         return
%     end
% end
% 
% % uncached, fetch
% if isunix
%     [result hname] = unix('hostname -f');
%     if result ~= 0, error('running `hostname`'); end
% 
%     if ~fqdn
%         % need to strip domain name,
%         % (this is the way /bin/hostname does it)
%         firstdot = min(strfind(hname, '.'));
%         if ~isempty(firstdot)
%             hname = hname(1:firstdot-1);
%         end
%     end
%     if hname(end) == sprintf('\n')
%         % remove newline        
%         T = hname(1:end-1); 
%     else
%         T = hname;
%     end
%     
%     if fqdn, cached_fqdn = T; else cached_name = T; end
% else
%     % windows
%     [result hname] = unix('hostname');
%     
%     if fqdn, error('no fqdn possible on windows'); end
%     
%     % remove newline
%     T = hname(1:end-1);         
% 
%     cached_name = T;
% end
% 
% assert(~isempty(T));

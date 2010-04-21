function mname = caller_mfilename (levels_up, use_fcn_names)
%CALLER_MFILENAME (ps-utils): return the mfilename of a fn's caller
%   MNAME = CALLER_MFILENAME(LEVELS_UP, USE_FCN_NAMES)
%   levels_up defaults to 1
%   USE_FCN_NAMES defaults to false: if true, return subfunction names
%   when traversing the stack, else return the mfilename. 
%
%
%$Id: caller_mfilename.m 125 2008-03-20 20:19:22Z vincent $

if nargin < 1, levels_up = 1; end
if nargin < 2, use_fcn_names = false; end

[st ix] = dbstack;
if use_fcn_names
    names = {st.name};
else
    names = {st.file};
end

desix = ix + levels_up + 1;
if desix > length(names) + 1
    error('Trying to go above the base workspace in the call stack');
elseif desix == length(names) + 1
    mname = 'base'; 
    return
end

mfullname = names{desix};
[mpath mname mext] = fileparts(mfullname);


    

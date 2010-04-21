function optout = stropt_merge(optinMaster, optinSlave)
%STROPT_MERGE (ps-utils): merge two sets of stropts
%   OPTOUT = STROPT_MERGE(optinMaster, optinSlave)
%   
%   If any duplicate stropts are found, the values in optinMaster take
%   precedence. 
%
%   See also STROPT_GET, STROPT_SET, STROPT_NAMES, STROPT2STRUCT, STROPT_DEFAULTS
%
%$Id: stropt_merge.m 125 2008-03-20 20:19:22Z vincent $


chkstropt(optinMaster);
chkstropt(optinSlave);

mNames = stropt_names(optinMaster);
slaveTrimmed = stropt_del(optinSlave, mNames, 'ignoremissing');

optout = cat(2,optinMaster,slaveTrimmed);
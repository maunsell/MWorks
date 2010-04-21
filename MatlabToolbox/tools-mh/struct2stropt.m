function stropts = struct2stropt (struct)
%STRUCT2STROPT (ps-utils): convert opt struct to get/set type stropts
%  STROPTS = STRUCT2STROPT(STRUCT)
%
%$Id: struct2stropt.m 125 2008-03-20 20:19:22Z vincent $

s=fieldnames(struct);
v=struct2cell(struct);
stropts=cell(1,length(s).*2);
stropts(1:2:end)=s;
stropts(2:2:end)=v;

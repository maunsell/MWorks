function [F,p] = sample_fstatistic(X,group)
%SAMPLE_FSTATISTIC (ps-utils): compute the F statistic on data
%
%   F = sample_fstatistic(X,GROUP)
%   X and GROUP are defined as in ANOVA1
%
%   See also: ANOVA1
%   
%$Id: sample_fstatistic.m 125 2008-03-20 20:19:22Z vincent $

[P,anovaTab,stats] = anova1(X,group,'off');
F = anovaTab{2,5};
p = anovaTab{2,6};


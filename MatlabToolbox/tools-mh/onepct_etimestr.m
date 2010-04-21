function timeStr = onepct_etimestr(elapsedSeconds)
%ONEPCT_ETIMESTR (ps-utils): pretty-print time rounded to ~ 1% accuracy
%   timeStr = ONEPCT_ETIMESTR(elapsedSeconds)
%   length(timeStr) <= 9 unless elapsedSeconds > 999 days.
%
%$Id: onepct_etimestr.m 125 2008-03-20 20:19:22Z vincent $

[eDays,eHrs,eMins,eSecs] = secsplit(elapsedSeconds);
if eDays > 100
    timeStr = sprintf('%d days', eDays);
elseif eDays > 0
    timeStr = sprintf('%dd %2dh', eDays, eHrs);
elseif eHrs > 0
    timeStr = sprintf('%2dh %2dm', eHrs, eMins);
elseif eMins > 0
    timeStr = sprintf('%2dm %2ds', eMins, round(eSecs));
elseif eSecs >= 10
    timeStr = sprintf('%4.1fs', eSecs);
elseif eSecs >= 0.01
    timeStr = sprintf('%3.2fs', eSecs);
else
    timeStr = sprintf('%gs', eSecs);
end


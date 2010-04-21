function [days, hrs, mins, secs] = secsplit(elapsedSeconds)
%SECSPLIT (ps-utils): convert number of seconds to days, hours, min, sec
%
%   [days, hrs, mins, secs] = SECSPLIT(elapsedSeconds)
%   each output argument is an integer except possibly seconds.
%
%$Id: secsplit.m 125 2008-03-20 20:19:22Z vincent $

assert(elapsedSeconds >= 0, 'elapsedSeconds cannot be negative');

sec = elapsedSeconds;

secDays = floor(sec / (24*60*60));
sec = sec - secDays*24*60*60;

secHrs = floor(sec / 3600);
sec = sec - secHrs*3600;

secMins = floor(sec / 60);
sec = sec - secMins * 60;

assert(secDays*24*60*60+ secHrs*3600 + secMins*60 + sec == elapsedSeconds, ...
       'Some arithmetic error');

days = secDays;
hrs = secHrs;
mins = secMins;
secs = sec;






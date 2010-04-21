function time_us = mwGetEventTime(events, codec, tag, ...
                                   occurrence, value, ignoreMissingStr)
%mwGetEventTime: given tag and possibly value, return event time
%
%  time_us = subGetEventTime(events, codec, tag, ...
%                            occurrence, value, ignoreMissingStr)
%
%
%  MH 100115: created
%$Id$

if nargin < 4 || isempty(occurrence), occurrence = 1; end
if nargin < 5 || isempty(value), value = []; end
if nargin < 6, ignoreMissingStr = []; end

if strcmp(ignoreMissingStr, 'ignoreMissing') 
  doIgnoreMissing = true;
else
  doIgnoreMissing = false;
end

codes = [events.event_code];
tCode = codec_tag2code(codec, tag);

eventNs = find(codes == tCode);
if length(eventNs) < 1
  if ~doIgnoreMissing
    disp(sprintf('Code not found: %s', tag));
    time_us = []; 
    return
  end
else
  % at least one
  if ~isempty(value)
    datas = [events(eventNs).data];
    dataNs = find(datas == value);
    tN = [];
    if length(dataNs) == 0
      disp(sprintf('Code %s with value %d not found', tag, value));
    elseif length(dataNs) < occurrence
      disp(sprintf('Asked for %d codes %s with value %d, but found %d with this value (%d total)', ...
                   occurrence, tag, value, length(dataNs), length(eventNs) ));
    else
      tN = eventNs(dataNs(occurrence));
    end
  else
    % no value filtering
    tN = eventNs(occurrence);
  end
  if isempty(tN)
    time_us = [];
    return
  else
    time_us = events(tN).time_us;
  end
end



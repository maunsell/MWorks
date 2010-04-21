function val = mwGetEventValue(events, codec, tag, occurrence, ignoreMissingStr)
%mwGetEventValue: given tag, return event value
%
%  val = mwGetEventValue(events, codec, tag, occurrence, ignoreMissingStr)
%       occurrence: can be a number or 'all' in which case val is a
%       vector of values
%
%  MH 100115: created
%$Id$

if nargin < 4 || isempty(occurrence), occurrence = 1; end
if nargin < 5, ignoreMissingStr = []; end

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
    ignoreMissingStr
    disp(sprintf('Code not found (in getvalue): %s', tag));
  end
  val = [];
  return
elseif ischar(occurrence) && strcmp(occurrence, 'all')
  val = cat(2,events(eventNs).data);
  return
else
  val = events(eventNs(occurrence)).data;
  return
end

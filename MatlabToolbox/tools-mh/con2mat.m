function [allconditions, headerline] = con2mat (infile)
%CON2MAT (ps-utils): parse Cortex conditions file
%   [ALLCONDITIONS, HEADERLINE] = CON2MAT (INFILE)
%
%   reads the cortex conditions file into a matlab matrix, allconditions
%   empty fields in the con file are denoted by NaN in the matrix
%   headerline contains the entire con file first line Only numbers that
%   fall completely within the columns specified by the header line are
%   read correctly.  These columns are specified by the width of the
%   label in the header.  ie. if header is 'TEST0 TEST1 ...' only
%   numbers which fall within the five-char wide columns under the
%   labels are read.  Nothing that is below a space in the header line
%   is read.
%
%   created Summer, 1997  --MH
%
%   7/10/98 - "header line ending in newline" bug fixed (line 39)
%   11/1/99 - cleanups, now parses commented files correctly - MH
%
%   This should read the items file directly also, by changing the
%   filename.
%
%$Id: con2mat.m 125 2008-03-20 20:19:22Z vincent $

[fid, message] = fopen(infile,'rt');
if fid == -1,
   error(message);
end

%read header line
readingchars = 0;
field = 1;
headerfields = zeros(64,2);
% headerfields: rows correspond to fields, column one is start pos, two is firsdbstept space after start pos
%   first column in con file is numbered 1

while (1),    % repeat until a non-comment line is found
  buf = fgetl(fid);
  if buf == -1,
    error('Error reading file!');
  end
  
  % add a space to the end of buf to ensure it doesn't end in a newline
  buf(length(buf)+1) = ' ';
  firstfield = sscanf (buf, ' %s ');
  
  % if this line is not blank, check to see if it's the header.
  if length(firstfield) >= 1 &&  isletter(firstfield(1))
      break;
  end
end

headerline = buf;

[rows cols] = size(buf);

for character = 1:cols, 
   if readingchars == 0,
      if buf(character) == ' ',
         % read next character
      else
         % we've got a char!
         readingchars = 1;
         headerfields (field, 1) = character;
      end
   else
      if buf(character) == ' ',
         readingchars = 0;
         headerfields (field, 2) = character;
         field = field + 1;
      else
         %read next char
      end
   end % if readingchars
end  % for

rows = find (headerfields(:,1)~=0);
headerfields = headerfields (rows, :);
output = headerfields;

% read each line and insert in conditions matrix

[hrows, hcols] = size(headerfields);
hend = headerfields (hrows,2);
linec = 0;
allconditions = ones (1024, hrows) * NaN;

while 1,
   buf = fgetl(fid);
   % end of file?
   if ~ischar(buf),
     break;
   end
   if (length (buf) > 0),
     if (buf (1:2) ~= '//'),
       firstfield = sscanf (buf, ' %s ');
       if ~isempty (firstfield),   % if not a blank line
         linec = linec + 1;
         [ brows, bcols ] = size (buf);
         buf(1, bcols+1:hend) = zeros (1, hend - bcols);
         for i=1:hrows,
           temp = sscanf (buf(1,headerfields(i,1):headerfields(i,2)), ' %f ', 1);
           if ~isempty(temp)
             allconditions(linec,i) = temp;
           end
         end
       end
     end
   end
 end
 
% now remove unused lines at end of matrix
allconditions = allconditions (1:linec,1:hrows);

fclose(fid);






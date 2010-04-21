function c = strtokc(s,d);
%STRTOKC Find all tokens in string
% C = STRTOKC(S) where S is string and C a cell array of tokens.
% C = STRTOKC(S,D) to specify delimiter D (default = char(9))
%
% see STRTOK

if nargin < 2
 d= char(9);
end
 
ind = 1;
while length(s)
    [c{ind},s] = strtok(s,char(9));    
    ind = ind + 1;
end

return;

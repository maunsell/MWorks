function retval = debugSaveInputs(data_struct, input)
%save inputs for debugging in a different session

if nargin < 2 || isempty(input), input.count = 1; end  % first trial

save(sprintf('/Users/histed/Desktop/debugSaveInputs-%03d.mat', input.count), ...
     '*');

% display status in a fig win
figure(1);
outTxt = sprintf('Saved vars, count: %d', input.count);
title(outTxt);
drawnow
disp(outTxt); % also will get to console window


% return outputs
input.count = input.count+1;
retval = input;

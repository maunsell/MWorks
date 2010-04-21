function [retval] = testOnlineHist(data_struct, input)

figNum = 1;

% First call after pressing "play" in client
if nargin == 1
    % init input
    input.count = 0;
    input.reactTimesMs = repmat(NaN, [3000, 1]);
    
    firstTrialCodes = data_struct;
end

figH=figure;
text(evalc('disp(data_struct)'));


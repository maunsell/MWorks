function input = saveMatlabState(data_struct, input)

% compute whether to do export on this trial
nTrialsToSkip = 10;
maxSToSkip = 120;

%% initialize variables
nTrial = length(input.trialOutcomeCell);
thisTimeMs = input.holdStartsMs{nTrial};
if nTrial == 1
  input.lastTimeSavedMs = 0;
  input.savedDataName = sprintf('/Library/MonkeyWorks/DataFiles/%s.mat', ...
                                 datestr(now, 'yyyymmdd_HHMMSS'));
end

%% compute elapsed time
savedDiffS = (thisTimeMs - input.lastTimeSavedMs)/1000;
doSave = (mod(input.trialSinceReset, nTrialsToSkip) == 0 ...
          || savedDiffS > maxSToSkip);

%% save vars to disk
if doSave
  save(input.savedDataName, 'input');

  input.lastTimeSavedMs = thisTimeMs;
  disp(sprintf('Saved matlab variables to disk'));
end




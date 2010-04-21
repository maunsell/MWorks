function input = testUpload(data_struct, input)

figNum = 4;

% compute whether to do export on this trial
nTrialsToSkip = 10;
maxSToSkip = 120;

%% initialize variables
nTrial = length(input.trialOutcomeCell);
thisTimeMs = input.holdStartsMs{nTrial};
if nTrial == 1 || ~isfield(input, 'lastTimeFigExportedMs') 
  input.lastTimeFigExportedMs = 0;
end

%% compute elapsed time
savedDiffS = (thisTimeMs - input.lastTimeFigExportedMs)/1000;
doSave = (mod(input.trialSinceReset, nTrialsToSkip) == 0 ...
          || savedDiffS > maxSToSkip);

if doSave
  
  [retVal cnameStr] = unix('uname -n');
  machineStr = strtok(cnameStr, '.'); % part before the first dot
  
  % export
  fname = sprintf('/tmp/%s-figoutput.png', machineStr);
  exportfigPrint(figNum, fname, ...
                 'FileFormat', 'png', ...
                 'Size', 4*[2 3].*[4 3]./3, ...
                 'Resolution', 72);
  
  % copy to server
  cmdStr = sprintf(['chmod a+r %s ;' ...
                    'scp -pq %s histed.org:/home/histed/histed.org/behavior-updates &'], ...
                   fname, fname); % run in background so we don't tie up matlab process
  unix(cmdStr);
  disp(sprintf('Copied file %s', fname));
  
  input.lastTimeFigExportedMs = thisTimeMs;
end

function plotOnlineHist(data_struct, input)

figNum = 4;

%% draw figure
figH = figure(figNum);
clf;
spSz = {3,2};

% some data processing
nPts = length(input.holdTimesMs);
nTrial = length(input.trialOutcomeCell);
holdV = [input.holdTimesMs{:}];
successIx = strcmp(input.trialOutcomeCell, 'success');
failureIx = strcmp(input.trialOutcomeCell, 'failure');

% 1 - total hold time histogram
axH = subplot(spSz{:},1);
maxFail = max(holdV(find(failureIx)));
if maxFail > 2000
  maxX = 3000;
elseif maxFail > 2500
  maxX = 2500;
else
  maxX = 2000;
end
if nPts > 50
  binWidth = iqr(holdV)./nPts.^(1/3); % robust version of std
  nBins = ceil(maxX./binWidth);
else
  nBins = 10;
end
edges = linspace(0, maxX, nBins);
Ns = histc(holdV(find(successIx)), edges);
Nf = histc(holdV(find(failureIx)), edges);
if sum(Ns)+sum(Nf) > 0
  bH = bar(edges, [Ns(:),Nf(:)], 'stacked');
  set(bH, 'BarWidth', 1, ...
          'LineStyle', 'none');
end
hold on;
xLim = [0 maxX+50];
set(gca, 'XLim', xLim);
yLim = get(gca, 'YLim');

title('total hold times');

if ~isempty(input.tooFastTimeMs)
  yLim = get(gca, 'YLim');
  plot(input.tooFastTimeMs * [1 1], yLim, 'k--');
end

% 2 - react time CDF
axH = subplot(spSz{:},3);
cdfplot([input.reactTimesMs{:}]);
set(gca, 'XLim', [-1000 1000], ...
         'YLim', [0 1]);
title(sprintf('mean hold %4.1fms, react %4.1fms', ...
              mean([input.holdTimesMs{:}]), mean([input.reactTimesMs{:}])));

% 3 - react time PDF
axH = subplot(spSz{:},5);
nPts = length(input.reactTimesMs);
if nPts > 50
  binWidth = iqr([input.reactTimesMs{:}])./nPts.^(1/3); % robust version of std
  nBins = ceil(2000./binWidth);
else
  nBins = 10;
end
edges = linspace(-1000, 1000, nBins);
binSize = edges(2)-edges(1);
rV = [input.reactTimesMs{:}];
Ns = histc(rV(successIx), edges);
Nf = histc(rV(failureIx), edges);
if sum(Ns)+sum(Nf) > 0
  bH = bar(edges+binSize/2, [Nf(:),Ns(:)], 'stacked');
  set(bH, 'BarWidth', 1, ...
          'LineStyle', 'none');
  cMap = get(gcf, 'Colormap');
  % flip colors, keep blue on top of red, note flipped in bar.m above
  set(bH(1), 'FaceColor', [0.6 0 0]);
  set(bH(2), 'FaceColor', [0 0 0.6]);      
end

hold on;
yLim = get(gca, 'YLim');
plot([0 0], yLim, 'k');
set(gca, 'XLim', [-1010 1010]);
title('reaction times');

% 4 - smoothed perf curve
axH = subplot(spSz{:},2);
hold on;
plot(smooth(double(successIx), ceil(nTrial/10), 'lowess'));
lH = plot(smooth(double(successIx), nTrial, 'lowess'));
set(lH, 'Color', 'r', ...
        'LineWidth', 3);
lH2 = plot(smooth(double(successIx), 100, 'lowess'));
set(lH2, 'Color', 'k', ...
        'LineWidth', 2);
xlabel('trial number');
ylabel('pct correct');
set(gca, 'YLim', [0 1]);
title(sprintf('Correct %d, failed %d, total %d', ...
              sum(successIx), sum(failureIx), nTrial));


% 5 - trial length plot
axH = subplot(spSz{:},4);
holdStarts = [input.holdStartsMs{:}];
%pH=semilogy(diff(holdStarts)/1000);
hSDiffsSec = diff(holdStarts)/1000;
pH=plot(hSDiffsSec);
hold on;
set(pH, 'LineStyle', 'none', ...
        'Marker', 'x');
%plot(diff(holdStarts)/1000, 'x');
xlabel('trial number');
ylabel('trial start time diff (s)');
xLim = get(gca, 'XLim');
lH = plot(xLim, 20*[1 1], '--k');

nDiffs = length(hSDiffsSec);
fN = max(1, nDiffs-5);  % if first 6 trials, start at 1
title(sprintf('Last 6 (sec): %s', mat2str(round(hSDiffsSec(fN:end)))));
%plot([1 nTrial], 60*[1 1], 'k');
%plot([1 nTrial], 120*[1 1], 'k');

% 6 - hold times over time
axH = subplot(spSz{:}, 6);
hold on;
hH(1) = plot(smooth(holdV, 50, 'loess'));
hH(2) = plot(smooth(holdV, 250, 'loess'));
set(hH(2), 'Color', 'k', ...
           'LineWidth', 3);
hRange = [0 prctile(holdV, 95)];
set(gca, 'YLim', hRange);
ylabel('Hold time (ms)');
if nDiffs>0
  totalElapsedS = (input.holdStartsMs{end} - ...
                   input.holdStartsMs{1})/1000;

  %totalRewMs = sum([input.totalRewardTimesMs{successIx}]);
  totalRewMs = sum(cat(2,input.juiceTimesMsCell{:}));
  title(sprintf('Total elapsed: %dmin; reward %.1fsec', ...
                round(totalElapsedS/60), totalRewMs/1000));
end



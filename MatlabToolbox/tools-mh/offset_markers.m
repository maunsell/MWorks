function offset_markers(handles, x_offset_amt, y_offset_amt)
%OFFSET_MARKERS (ps-utils): if data values of lines overlap, offset them
%   OFFSET_MARKERS(HANDLES, X_OFFSET_AMT, Y_OFFSET_AMT)
%   This function iterates through all the x and y points in the lines
%   specified by HANDLES and if any overlap, offsets each by the amount (in
%   PlotUnits) specified by X/Y_OFFSET_AMT.
%
%   TODO: add a units parameter to allow offset specification in points (need
%   to add a plotunits -> points function too).
%
%   Only works with 2-d plots
%
% $Id: offset_markers.m 125 2008-03-20 20:19:22Z vincent $
nHandles = length(handles);
allVect = [];
% make a big vector of each of the X and Y data
% allVect is a matrix of size (number total points, 4)
%   columns: 1: xData value, 2: yData value, 3: index into handles
%            4: index into XData,YData arrays
for iH=1:nHandles
    tH = handles(iH);
    tXData = colvect(get(tH, 'XData'));
    tYData = colvect(get(tH, 'YData'));    
    nPts = length(tXData);
    assert(nPts == length(tYData));
    bothWithIx = cat(2,tXData,tYData, repmat(iH,nPts,1), (1:nPts)');
    allVect = cat(1,allVect, bothWithIx);
end
%now find identical points

% look for dups
[tUniquePts,all2uniqueIx,unique2allIx] = unique(allVect(:,[1,2]),'rows');
nUniques = length(tUniquePts);
for iUniq = 1:nUniques
    theseAllIx = find(unique2allIx == iUniq);
    nPtsWithThisVal = length(theseAllIx);
    if nPtsWithThisVal == 1
        % no offset
        continue
    else
        for iPWV = 2:nPtsWithThisVal
            allIx = theseAllIx(iPWV);
            handIx = allVect(allIx,3);
            tHand = handles(handIx);
            datIx = allVect(allIx, 4);
            
            % do offsets
            if x_offset_amt ~= 0
                tXData = colvect(get(tHand, 'XData'));
                tXData(datIx) = tXData(datIx) + x_offset_amt*(iPWV-1);
                set(tHand, 'XData', tXData);
            end
            if y_offset_amt ~= 0
                tYData = colvect(get(tHand, 'YData'));
                tYData(datIx) = tYData(datIx) + y_offset_amt*(iPWV-1);
                set(tHand, 'YData', tYData);
            end            
        end
    end
end


            

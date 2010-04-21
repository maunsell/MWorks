function debugRunScript(fname, countList)
%
% from MW MatlabWindow, call debugSaveVariables.  Then call this to test
% your function
%
%
% MH 100105

fH = str2func(fname);

nC = length(countList);
for iC = 1:nC
    tC = countList(iC);
    debugReadVariables(tC);
    whos
    
    feval(fH, data_struct, input);
end


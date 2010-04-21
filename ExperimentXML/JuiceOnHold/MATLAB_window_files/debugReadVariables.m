function debugReadVariables(count)

fname = sprintf('/Users/histed/Desktop/debugSaveInputs-%03d', count);
evalin('base', sprintf('load %s', fname));

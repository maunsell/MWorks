function rgbVec = colorspec2rgb(inColorSpec)
%COLORSPEC2RGB (Posit): convert Matlab colorSpec to rgb vector
%
%   rgbVec = COLORSPEC2RGB(inColorSpec)
%
%   Convert matlab's color specs (can be strings) to rgb vectors
%$Id: colorspec2rgb.m 125 2008-03-20 20:19:22Z vincent $

% make everything a cell array
if ischar(inColorSpec)
    inColorSpec = cellstr(inColorSpec);
elseif isnumeric(inColorSpec)
    assert(prod(size(inColorSpec)) == 3 || size(inColorSpec, 2) == 3, ...
           'Numeric inputs must have 3 columns: RGB');
    inColorSpec = num2cell(inColorSpec,2);
elseif iscell(inColorSpec)
    % do nothing
else
    error('Invalid input');
end

% get number of colors
nColors = length(inColorSpec);

for iC = 1:nColors
    tColorSpec = inColorSpec{iC};

    foundColor = false;
    if ischar(tColorSpec)
        switch tColorSpec
            case {'y', 'yellow'}
                tRgbVec = [1 1 0];
            case { 'm', 'magenta' }
                tRgbVec = [1 0 1];
            case { 'c', 'cyan' }
                tRgbVec = [0 1 1];
            case { 'r', 'red' }
                tRgbVec = [1 0 0];
            case { 'g', 'green' }
                tRgbVec = [0 1 0];
            case { 'b', 'blue' }
                tRgbVec = [0 0 1];
            case { 'w', 'white' }
                tRgbVec = [1 1 1];
            case { 'k', 'black' }
                tRgbVec = [0 0 0];
        end
        foundColor = true;
    elseif isnumeric(tColorSpec)
        assert(length(tColorSpec) == 3 ...
               && all(tColorSpec >= 0 & tColorSpec <= 1));
        tRgbVec = tColorSpec;
        foundColor = true;
    end

    if ~foundColor
        %try to use matlab
        warning('Fixed color %s not hardcoded', tColorSpec)
        lH = plot(0,0,...
                  'Color', tColorSpec, ...
                  'Visible', 'off');
        tRgbVec = get(lH, 'Color');
        delete(lH);
    end
    
    rgbVec{iC} = tRgbVec;   %#ok
end

if nColors == 1
    % remove cell
    rgbVec = rgbVec{:};
end



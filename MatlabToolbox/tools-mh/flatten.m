function result = flatten(array,siz)
%FLATTEN Flatten multidimensional array along last dim
% RESULT = FLATTEN(ARRAY)
% RESULT = FLATTEN(ARRAY,SIZ)

c = class(array);

nd = ndims(array);
si = size(array);

if nd < 3
    result = array;
    return;
end

if nargin < 2 ; 
    n = ceil(sqrt(si(end)));
    siz = n*[1 1];
end

carray = cell(siz);

for ii = 1:nd
    xx{ii}=1:si(ii);
end

for irow = 1:siz(1)
    for icol = 1:siz(2)
        ii = siz(2)*(irow-1)+icol;
        xx{end}=ii;
        if ii <= si(end)
            carray{irow,icol}=array(xx{:});
        else
            switch(c)
                case 'uint16' 
                    carray{irow,icol} = ones(si(1:end-1),c)*2^15;                    
                case {'single','double'}
                    carray{irow,icol} = zeros(si(1:end-1),c);
                    
            end
                
        end
    end
end

result = cell2mat(carray);

return;
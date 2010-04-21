function export_to_libsvm(filename, dataMat, rowLabels)
%EXPORT_TO_LIBSVM (ps-utils): write a text file that libsvm can read
%
%   EXPORT_TO_LIBSVM(filename, dataMat, rowLabels)
%
%$Id: export_to_libsvm.m 125 2008-03-20 20:19:22Z vincent $

fid = fopen(filename, 'w');

[nClasses, nDims] = size(dataMat);

for i = 1:nClasses
    % label
    fprintf(fid, '%d ', rowLabels(i));
    
    % now all values
    for j = 1:nDims
        fprintf(fid, '%d:%g ', j, dataMat(i,j));
    end
    
    % newline
    fprintf(fid, '\n');
end

fclose(fid);


    

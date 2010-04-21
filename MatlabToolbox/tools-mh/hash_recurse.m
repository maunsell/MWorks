function hash = hash_recurse(inArr)
%HASH_RECURSE (ps-utils): MD5 hash arrays, handles cell/struct recursively
%   hash = HASH_RECURSE(inArr)
%
%   Note that this is slow, don't call it in a loop.
%
%$Id: hash_recurse.m 125 2008-03-20 20:19:22Z vincent $

hashLen = 16;  % bytes
emptyHash = [125  234   54   43   63  172  142    0, ...  % hash_recurse(0)
             149  106   73   82  163  212  244  116];     % rowvect

if isempty(inArr)
    hash = emptyHash;
    return
    
elseif isnumeric(inArr) || ischar(inArr) || islogical(inArr)
    % just hash it
    inArr = inArr(:);  % make it a vector
    % convert strings and logicals into uint8 format
    if ischar(inArr) || islogical(inArr);
        inArr = uint8(inArr);
    else % convert everything else into uint8 format without loss of data
        inArr = typecast(inArr,'uint8');
    end
    method = 'MD5';

    % create hash
    x=java.security.MessageDigest.getInstance(method);
    x.update(inArr);
    hash = typecast(x.digest,'uint8')';  % row vect
    return
    
elseif iscell(inArr)
    % hash all its elements
    inArr = inArr(:);
    h = repmat(NaN,hashLen,length(inArr));
    for iE=1:length(inArr)
        h(1:hashLen,iE) = hash_recurse(inArr{iE});
    end
    hash = hash_recurse(h);
    return
    % one idea for speeding up: take all the elements in the cell array of
    % similar class, concat them, then call hash_recurse only once on them.  
elseif isstruct(inArr)
    h1 = hash_recurse(struct2cell(inArr));
    h2 = hash_recurse(fieldnames(inArr));
    hash = hash_recurse([h1 h2]);
elseif isa(inArr, 'function_handle')
    % hash its string representation.
    % * Does not ensure it does the same thing, but then neither do function
    % handles.  Matlab runs whatever you've got on disk at the time the
    % function handle is called (as of R14)
    % * we don't handle non-scalar function handles (deprecated in R14)
    hash = hash_recurse(func2str(inArr));
else
    error('Don''t know how to hash type: %s', class(inArr));
end

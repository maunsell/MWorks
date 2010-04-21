function [outData, outKey] = memory_cache(functionStr, ...
                                          inArg2, inArg3, inArg4, inArg5)
%MEMORY_CACHE (posit): keep data in memory for speed
%
%   MEMORY_CACHE('clear')
%   MEMORY_CACHE('set', cacheData, cacheId, outdatedKey, allowDupsStr)
%   cachedData = MEMORY_CACHE('get', cacheId, allowMissingStr [,outdatedKey])
%   [cachedData,cachedOutdatedKey] = MEMORY_CACHE('get', cacheId, ...)
%   bytes = MEMORY_CACHE('bytesused') 
%       Return memory bytes used by cache data and keys
%
%   There are two bits of index data associated with each cache entry: cacheId
%   and outdatedKey.  Both can be arbitrary arrays and comparisons are done
%   after hashing with HASH_RECURSE
%
%   cacheId identifies the type of data; it is used to retrieve the data 
%       and only one cache entry with a given cacheId is allowed.
%   outdatedKey is used to tell whether the data associated with a given
%       cacheId is out of date; use some key here that will change whenever
%       the underlying data changes (like a time stamp on a file?)
%       on 'get', if outdatedKey is different from saved, this is a miss
%           and [] is returned/error results (according to allowMissing)
%       on 'set', if outdatedKey is different from saved, the cached
%           data is replaced
%
%   allowDupsStr: if equal to 'allowDups', then ignore the attempt to store
%       data with the same key and Id.  If not equal, raise an error on this.
%   allowMissingStr: if equal to 'allowMissing', then return an empty matrix
%       if the ID is not found.  If not equal raise an error on missing Id.
%
%   Note that if you cache many data items, running this function can take a
%   long time due to id comparisons.
%
%   Example:
%   --------    
% $$$ %%% check mem cache %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% $$$ cacheKey = {fileName}; outdatedKey = {crunched_dir(dataFileName)}; 
% $$$ cDat = memory_cache('get', cacheKey, 'allowMissing', outdatedKey);
% $$$ if ~isempty(cDat), [v1, v2] = deal(cDat{:}); return; end
% $$$ %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% $$$ 
% $$$ your code here
% $$$ 
% $$$ %%% store data in cache %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% $$$ memory_cache('set', {v1, v2}, cacheKey,outdatedKey,'allowDups');
% $$$ %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   
%$Id: memory_cache.m 482 2009-04-13 15:56:24Z histed $

%To flush the cache when the mfile is changed
    
%   %%% check cache
%   persistent mFileChangedFlag
%   outdatedKey = {mFileChangedFlag};
%   cachedDat = memory_cache('get', cacheKey, 'allowMissing', outdatedKey);
%   if ~isempty(cachedDat)  % cache hit
%       [pvalues,nTests,areaNames,statsZ,clusterHashes] = deal(cachedDat{:});
%       return
%   end  
%   %%% else go on
%   
%   -------- your code goes here
%
%   %%% store data in cache
%   mFileChangedFlag = 'mFileInMemory';
%   outdatedKey = {mFileChangedFlag};
%   memory_cache('set', {pvalues,nTests,areaNames,statsZ,clusterHashes}, ...
%                cacheKey, outdatedKey, 'allowDups');
%   %%%    
%   -------- example endn

    
    
% Notes:
%   I iterate through the cache ids both on getting and setting (because I
%   want to make sure there are no duplicates when setting)
% 
%   I allow Ids to be arbitrary data to be flexible.  I could convert them to
%   strings and use strpcmp for matching, which is faster but probably
%   requires code in each function that calls this one 

persistent theCacheData theOutdatedKeys theIds
% data is cell array, keys are numeric arrs of hashes size (nKeys,hashLen)

if nargin < 1 || ~ischar(functionStr)
    error('Invalid arguments');
end

switch functionStr
    case 'set'
        % process args
        inData = inArg2;
        inId = hash_recurse(inArg3);
        inOutdatedKey = hash_recurse(inArg4);        
        allowDups = false; % default
        if nargin > 4, allowDups = strcmpi(inArg5, 'allowDups'); end
        if nargin > 5, error('Too many arguments'); end
        
        no = subWhichCacheIdNo(theIds, inId);
        
        if ~isempty(no) % one match
            cacheNo = no;
            
            % check if not outdated            
            storedKey = theOutdatedKeys(cacheNo,:);
            if all(storedKey == inOutdatedKey)
                % dup data found
                if allowDups
                    % data is already there
                    if ~(isequalwithequalnans(inData, theCacheData{no}))
                        error('Duplicate key/id found and data differs!');
                    end
                else
                    error('Trying to store duplicate data in cache');
                end
                isOutdated = true;
            else
                isOutdated = false;
            end
        else
            % no entry
            cacheNo = length(theIds) + 1;
            isOutdated = false;
        end
        
        % store data
        hashLen = length(inId);
        theCacheData{cacheNo} = inData;
        theIds(cacheNo,1:hashLen) = inId;
        theOutdatedKeys(cacheNo,1:hashLen) = inOutdatedKey;

        % display some status in a warning
        totalBytes = subBytesUsed(theIds, theCacheData, theOutdatedKeys);
        if isOutdated, dupStr = '(dup)'; else dupStr = ''; end


        idStr = pp_hash_key(inArg3, 24); % pretty print the id
        warningStr ...
            = sprintf('Memory cache total: %sb, stored id%s: %s', ...
                      num2str_metric(chop(totalBytes, 2)), ...  
                      dupStr, ...
                      idStr);

        if warningStr(end) == sprintf('\n'), 
            warningStr = warningStr(1:end-1);  % strip EOL 
        end
        warning('POSIT:memorycache', warningStr);
    
    case 'get'
        inId = hash_recurse(inArg2);
        hashLen = length(inId);
        allowMissing = false; % default
        if nargin > 2, 
            allowMissing = strcmpi(inArg3, 'allowMissing');
        end
        outdatedKey = []; % default
        if nargin > 3, outdatedKey = hash_recurse(inArg4); end
        if nargin > 4, error('Too many inputs'); end

        no = subWhichCacheIdNo(theIds, inId);
        if isempty(no)
            if ~allowMissing
                error('in ''get'': Id not found (allowMissing not set)');
            end
            outData = [];
            if nargout > 1, outKey = []; end
            return
        else
            if ~(length(no) == 1)
                error('Cache inconsistency');
            end
        end
        
        % got here means ID match; check outdated key if it exists
        if ~isempty(outdatedKey)
            cachedOutdatedKey = theOutdatedKeys(no,:);
            if ~all(outdatedKey == cachedOutdatedKey)
                % mismatch
                if ~allowMissing
                    error('''get'': OutdatedKey mismatch (allowMissing unset');
                else
                    outData = [];
                    if ~(nargout == 1)
                        error('outdatedKey both input and output');
                    end
                    return
                end
            end
        end
        
        % got here means OutdatedKey match or not checked (not passed in)
        
        outData = theCacheData{no};
        if nargout > 1, outKey = theOutdatedKeys(no,:); end
        
    case 'clear'
        totalBytes = subBytesUsed(theIds, theCacheData);
        warningStr ...
            = sprintf('Clearing cache: %sb total mem freed', ...
                      num2str_metric(chop(totalBytes, 2)));

        clear theCacheData theIds

        warning('POSIT:memorycache', warningStr);
        
        
    case 'bytesused'
        totalBytes = subBytesUsed(theIds, theCacheData);
        outData = totalBytes;
otherwise
    error('Invalid argument: ''%s''', functionStr);
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function no = subWhichCacheIdNo(theCacheIds, idToMatch)

if isempty(theCacheIds), no = []; return; end
    
nIds = size(theCacheIds,1);
hashLen = length(idToMatch);
% old cell array code
% $$$ isMatch = false(1, nIds);
% $$$ for i=1:nIds
% $$$     isMatch(i) = isequalwithequalnans(theCacheIds{i}, idToMatch);
% $$$ end
isMatch = all(theCacheIds == repmat(idToMatch,nIds,1), 2);
no = find(isMatch);

if length(no) > 1
    error('Cache inconsistency: Duplicate ids');
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function bytesUsed = subBytesUsed(theCacheIds, theCacheData, ...
                                   theOutdatedKeys)  %#ok
wS = whos('theCacheIds', 'theCacheData', 'theOutdatedKeys');
bytesUsed = sum(cat(1, wS(:).bytes));

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


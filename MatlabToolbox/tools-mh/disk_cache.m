function [outData, outKey, varargout] = disk_cache(functionStr, varargin)
%DISK_CACHE (posit): temporary disk caches
%
%   * Writing/reading data in cache:
%   DISK_CACHE('set', cacheData, cacheId, outdatedKey, ...
%                     allowDupsStr, dataClass)
%   cachedData = DISK_CACHE('get', cacheId, allowMissingStr)
%   cachedData = DISK_CACHE('get', cacheId, allowMissingStr, dataClass)    
%   cachedData = DISK_CACHE('get', cacheId, allowMissingStr, dataClass, ...
%                                  outdatedKey)
%   [cachedData,cachedOutdatedKey] = DISK_CACHE('get', cacheId, ...)
%
%   * Manipulate cache:    
%   DISK_CACHE('clear')
%   DISK_CACHE('clear', dataClass)  % Clears only data in this class
%   DISK_CACHE('list')
%   DISK_CACHE('list', dataClass)   % lists only data in this class    
%   [cacheIds, cacheOutdatedKeys, cacheClasses] = DISK_CACHE('getcacheindex')
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
%   dataClass: a string which groups data into classes.  This string is used
%       as the data file name if specified.  Data in different classes
%       can collide if the key is the same
%
%   allowDupsStr: if equal to 'allowDups', then ignore the attempt to store
%       data with the same key and Id.  If not equal, raise an error on this.
%   allowMissingStr: if equal to 'allowMissing', then return an empty matrix
%       if the ID is not found.  If not equal raise an error on missing Id.
%
%   For an example: do DBTYPE disk_cache
%   Todo (bug): data in different classes should not collide
%
%   See also: MEMORY_CACHE    
%
%$Id: disk_cache.m 164 2008-04-07 04:32:20Z histed $

%   Example:
%
% $$$ %%% check disk cache %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% $$$ cClass = 'typeofdata'; cacheKey = {inputVar1}; outdatedKey = {}; 
% $$$ cDat = disk_cache('get', cacheKey, 'allowMissing', cClass, outdatedKey);
% $$$ if ~isempty(cDat), [v1, v2] = deal(cDat{:}); return; end
% $$$ %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% $$$ 
% $$$ your code here
% $$$ 
% $$$ %%% store data in cache %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% $$$ disk_cache('set', {v1, v2}, cacheKey,outdatedKey,'allowDups',cClass);
% $$$ %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% dataClass is used only for user classification purposes.  It is not used
% to find index entries at all.
    
if nargin < 1 || ~ischar(functionStr)
    error('Invalid arguments');
end

% cache dir name
dirs = directories;
cacheDir = dirs.diskcache;

delIxKey = repmat(NaN,1,16);  % hashLen for MD5 is 16bytes 

switch functionStr
    case 'set'
        % process args
        if nargin <= 3, error('Must specify data, cacheId, outdatedKey'); end
        inData = varargin{1};
        inFullId = varargin{2};
        inOutdatedKey = hash_recurse(varargin{3});        

        allowDups = false; % default
        if nargin > 4, allowDups = strcmpi(varargin{4}, 'allowDups'); end

        dataClass = 'unspecifiedClass'; 
        if nargin > 5, dataClass = varargin{5}; end

        if nargin > 6, error('Too many arguments'); end
        assert(isstr(dataClass));

        inId = hash_recurse(inFullId);
        hashLen = length(inId);

        
        [diskIds, diskOutdatedKeys, diskDataClasses] ...
            = subGetIndexData(cacheDir);
        
        no = subWhichCacheIdNo(diskIds, inId);
        
        if length(no) == 1  % one match
            cacheNo = no;
            
            % check if not outdated            
            storedKey = diskOutdatedKeys(cacheNo,hashLen);
            if all(storedKey == inOutdatedKey)
                % dup data found
                if allowDups
                    % data is already there
                    % no check to see if the existing data matches
                    %assert(~isequalwithequalnans(inData, ...
                    %                             diskCacheData{no}), ...
                    %       'Duplicate key/id found but data is different!');
                else
                    error('Trying to store duplicate data in cache');
                end
                isOutdated = true;
            else
                isOutdated = false;
            end
        else
            % no entry, find the first deleted
            firstDelNo = subWhichCacheIdNo(diskIds,delIxKey,true);
            if ~isempty(firstDelNo)
                % found one
                assert((strcmp(diskIds(firstDelNo), delIxKey) ...
                        && strcmp(diskOutdatedKeys(firstDelNo),delIxKey) ...
                        && strcmp(diskDataClasses(firstDelNo),delIxKey)), ...
                       'Internal cache bug');
                cacheNo = firstDelNo;
            else
                % no empties, add to end
                cacheNo = size(diskIds,1) + 1;                
            end

            isOutdated = false;
        end
        
        % store data
        subSaveDataByNum(cacheDir, dataClass, inId, cacheNo, inOutdatedKey, ...
                         inData);

        % update index
        diskIds(cacheNo,1:hashLen) = inId;
        diskOutdatedKeys(cacheNo,1:hashLen) = inOutdatedKey;
        diskDataClasses{cacheNo} = dataClass;
        subSetIndexData(cacheDir, diskIds, diskOutdatedKeys, diskDataClasses);

        % display some status in a warning
        if isOutdated, dupStr = '(dup)'; else dupStr = ''; end
        w = whos('inData');
        warningStr ...
            = sprintf('Cached to disk %sb (before zip)%s, class %s id %s', ...
                      num2str_metric(w.bytes, 3), ...
                      dupStr, ...
                      pp_hash_key(dataClass, 16), ...
                      pp_hash_key(inFullId, 16));

        if warningStr(end) == sprintf('\n'), 
            warningStr = warningStr(1:end-1);  % strip EOL 
        end
        warning('POSIT:diskcache', warningStr);
    
  case 'get'
    % Note: we ignore data class when retrieving
        inId = hash_recurse(varargin{1});
        hashLen = length(inId);
        allowMissing = false; % default
        if nargin > 2, 
            allowMissing = strcmpi(varargin{2}, 'allowMissing');
        end

        dataClass = ''; % default
        if nargin > 3, dataClass = varargin{3}; end

        outdatedKey = []; % default
        if nargin > 4, outdatedKey = hash_recurse(varargin{4}); end

        if nargin > 5, error('Too many inputs'); end
        
        %%%%%%%%%%%%%%%%

        [diskIds, diskOutdatedKeys, diskDataClasses] ...
            = subGetIndexData(cacheDir);
        no = subWhichCacheIdNo(diskIds, inId);

        if isempty(no)
            if ~allowMissing
                error('in ''get'': Id not found (allowMissing not set)');
            end
            outData = [];
            if nargout > 1, outKey = []; end
            return
        else
            assert(length(no) == 1, 'Cache inconsistency');
        end
        
        % got here means ID match; check outdated key if it exists
        if ~isempty(outdatedKey)
            cachedOutdatedKey = diskOutdatedKeys(no,1:hashLen);
            if ~all(outdatedKey == cachedOutdatedKey)
                % mismatch
                if ~allowMissing
                    error('''get'': OutdatedKey mismatch (allowMissing unset');
                else
                    outData = [];
                    assert(nargout == 1, 'outdatedKey both input and output');
                    return
                end
            end
        end
        % got here means OutdatedKey match or not checked (not passed in)
        
        % use class from index
        inClass = diskDataClasses{no};
        
        outData = subLoadDataByNum(cacheDir, inClass, inId, no, ...
                                   diskOutdatedKeys(no,1:hashLen));
        if nargout > 1, outKey = diskOutdatedKeys(no,1:hashLen); end
        
    case 'clear'
        dataClass = ''; % default, means delete all
        if nargin == 2, dataClass = varargin{1}; end
        if nargin > 2, error('Too many arguments to ''clear'''); end

        if isempty(dataClass)
            deleteMask = '_PSCACHE_data-*.mat';            
        else
            deleteMask = sprintf('_PSCACHE_data-class_%s-*.mat', dataClass);
        end

        deleteFileName = fullfile(cacheDir, deleteMask);
        d = dir(deleteFileName);
        nFiles = length(d);
        disp(sprintf('Clearing cache: %d data files (ids) to delete', ...
                     nFiles));

        disp('C-c to abort, RET to continue');
        pause;
        
        % deal with index
        if isempty(dataClass)
            % clear mem index cache
            subSetIndexData(cacheDir, [], [], {});        
            memory_cache('clear');
        else
            % remove this class's entries
            [diskIds, diskOutdatedKeys, diskDataClasses] ...
                = subGetIndexData(cacheDir);
            delIx = strcmp(diskDataClasses, dataClass);

            hashLen = length(hash_recurse(0));

            nToDel = sum(delIx);
            diskIds(delIx,1:hashLen) = repmat(delIxKey(:)', [nToDel,1]);
            diskOutdatedKeys(delIx,1:hashLen) = repmat(delIxKey(:)', [nToDel,1]);
            [diskDataClasses{delIx}] = deal(delIxKey);
            
            % rewrite index
            subSetIndexData(cacheDir, diskIds, ...
                            diskOutdatedKeys, ...
                            diskDataClasses);
        end

        delete(deleteFileName);
        disp('Success.');
    case 'list'
        dataClass = ''; % default: list all
        if nargin == 2, dataClass = varargin{1}; end
        if nargin > 2, error('Too many arguments to ''list'''); end
        
        % get data from index
        [diskIds, diskOutdatedKeys, diskDataClasses] ...
            = subGetIndexData(cacheDir);
        w = whos('diskIds', 'diskOutdatedKeys', 'diskDataClasses');
        indexSize = sum(cat(1,w.bytes));

        % restrict by class if desired
        if ~isempty(dataClass)
            selIx = strcmp(diskDataClasses, dataClass);
        else
            selIx = true(1,length(diskDataClasses));
        end
        filtIds = diskIds(selIx,:);
        filtOutdatedKeys = diskOutdatedKeys(selIx,:);            
        filtDataClasses = diskDataClasses(selIx);
        
        % pretty printing here
        nEntries = size(filtIds,1);
        disp('Class                Size    ID');
        disp('-------------------------------------------------------------'); 
        for i=1:nEntries
            tId = filtIds(i,:);
            if all(isnan(tId))
                % deleted, skip it
                sz(i) = 0;
                continue
                %idStr = 'deleted';
                %tClass = 'deleted';
            else
                idStr = reshape(lower(dec2hex(tId))',1,[]);
                no = subWhichCacheIdNo(diskIds, tId);
                tClass = filtDataClasses{i};
                sz(i) = subGetDataSize(cacheDir, filtDataClasses{i}, no);
            end
            
            tStr{i} = sprintf('%-30.30s %4sb   %s', ...
                              tClass, ...
                              num2str_metric(chop(sz(i),2)), ...
                              idStr);
        end
        if nEntries == 0
            disp('*** No data cached yet'); 
            sz = 0; indexSize = 0;
        else
            % remove blank lines (deleted entries)
            tStr = cellstr(cat(1,tStr{:}));
            disp(char(tStr));
        end
        
        % display summary sizes
        disp(sprintf('Total size of listed data: %sb; index is %sb', ...
                     num2str_metric(chop(sum(sz),3)), ...
                     num2str_metric(chop(indexSize,3))));
        return
  
    case 'getcacheindex'
        % get data from index
        [diskIds, diskOutdatedKeys, diskDataClasses] ...
            = subGetIndexData(cacheDir);
        outData = diskIds;
        outKey = diskOutdatedKeys;
        varargout{1} = diskDataClasses;
        return
    
    otherwise
      error('Unknown functionStr: %s', functionStr);
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function no = subWhichCacheIdNo(theCacheIds, idToMatch, allowDups, ...
                                theClasses, restrictToClass)

if nargin < 3, allowDups = false; end
if nargin < 4, theClasses = []; end
if nargin < 5, restrictToClass = []; end

nIds = size(theCacheIds,1);
if nIds == 0
    % no index at all
    no = []; 
else
    if ~isempty(restrictToClass)
        desIx = strcmp(theClasses, restrictToClass);
    else
        desIx = true(nIds,1);
    end
    
    no = find( (all((theCacheIds == repmat(idToMatch,[nIds,1])), 2) ...
                & desIx));
    nFound = length(no);
    if allowDups
        % allow dups only used when finding deleted placeholder key
        if nFound > 1
            % return the first one
            no = no(1); 
        end
    else
        if nFound > 1
            error('Cache inconsistency: Duplicate ids');
        end
        % do nothing, the single match found is returned
    end
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [diskCacheIds, diskOutdatedKeys, diskDataClasses] ...
        = subGetIndexData(cacheDir)
% This function and the next contain all the functions for storing the index
% data.  Note that the index data is cached in memory for speed

%%% check mem cache
persistent mFileChangedFlag
memCacheKey = mfilename;
memOutdatedKey = mFileChangedFlag;
cachedDat = memory_cache('get', memCacheKey, 'allowMissing', memOutdatedKey);
if ~isempty(cachedDat)  % cache hit
    [diskCacheIds, diskOutdatedKeys, diskDataClasses, cacheIndexDir] ...
        = deal(cachedDat{:});
    if strcmp(cacheIndexDir, cacheDir)
        % cache hit, return data
        return
    else
        % dir mismatch
        disp('disk_cache: Index in mem is for a different cache dir, clearing');
        memory_cache('clear');
        % just continue
    end
end  
%%% else go on

% if we got here, there is no data in mem, read it
xFileName = fullfile(cacheDir,'_PSCACHE_indexdata.mat');
if exist(xFileName, 'file')
    ds = load(xFileName);

    diskCacheIds = ds.diskCacheIds;
    diskOutdatedKeys = ds.diskOutdatedKeys;
    diskDataClasses = ds.diskDataClasses;

else
    %initialize cache
    diskCacheIds = [];
    diskOutdatedKeys = [];
    diskDataClasses = {};
end


%%% store data in mem cache
wstate = warning('off', 'POSIT:memorycache'); 
indexDat = {diskCacheIds, diskOutdatedKeys, diskDataClasses, cacheDir};
memory_cache('set', indexDat, memCacheKey, now+cputime, 'allowDups');
warning(wstate);
%%%    


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function subSetIndexData(cacheDir, diskCacheIds, diskOutdatedKeys, ...
                         diskDataClasses)

xFileName = fullfile(cacheDir,'_PSCACHE_indexdata');

save(xFileName, '-v6', 'diskCacheIds', 'diskOutdatedKeys', 'diskDataClasses');

%%% store data in mem cache too
memCacheKey = mfilename;
mFileChangedFlag = 'mFileInMemory';
memOutdatedKey = mFileChangedFlag;
wstate = warning('off', 'POSIT:memorycache'); 
indexDat = {diskCacheIds, diskOutdatedKeys, diskDataClasses, cacheDir};
memory_cache('set', indexDat, memCacheKey, now, 'allowDups');
warning(wstate);
%%%    


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% the below functions are used to save/load data from disk.  If I wanted to
% move away from the v6 .MAT format at some point, I just need to replace
% these four

function fName = subCacheFileName(cacheDir, className, idNum)
fName = fullfile(cacheDir, ...
                 sprintf('_PSCACHE_data-class_%s-id_%08d.mat', ...
                         className, idNum));

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function subSaveDataByNum(cacheDir, className, cacheId, idNum, ...
                      cacheOutdatedKey, cacheData);
% Note: each data entry is saved in a separate file

assert(isvarname(className), 'Class name must be a valid var name');
cacheFileName = subCacheFileName(cacheDir, className, idNum);

% always save without compression: slower by a factor of 10 for large files
save(cacheFileName, '-v6', ...
     'className', 'cacheId', 'cacheOutdatedKey', 'cacheData');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [dat] = subLoadDataByNum(cacheDir, className, cacheId, idNum, ...
                                  cacheOutdatedKey);

assert(isvarname(className), 'Class name must be a valid var name');
cacheFileName = subCacheFileName(cacheDir, className, idNum);

ds = load(cacheFileName);
assert(all(ds.cacheOutdatedKey == cacheOutdatedKey));
assert(all(ds.cacheId == cacheId));

dat = ds.cacheData;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function outSize = subGetDataSize(cacheDir, className, idNum)
cacheFileName = subCacheFileName(cacheDir, className, idNum);
d = dir(cacheFileName);
if ~isempty(d)
    outSize = d.bytes;
else
    error('index inconsistency: file not found?');
end



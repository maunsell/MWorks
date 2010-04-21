function realname = real_opt_name(thisname, altname_stropt)
% private function for stropt_defaults

% thisname: string, name to check; altname_stropt: stropt structure where
% each name is the real option name and value is cellstr of alt names
% if not found, return empty

if isempty(altname_stropt), realname = thisname; return; end

realnames = stropt_names(altname_stropt);  % repeat checking in stropt_names

%% cache check
persistent optNameIndex inputAltnameStropt
if isequalwithequalnans(inputAltnameStropt, altname_stropt)
    % This was the last altname_stropt, don't need to make index
else
    % save input stropt
    inputAltnameStropt = altname_stropt;
    
    % build index
    nRealNames = length(realnames);
    allAltNames = stropt_get(altname_stropt, realnames); % cell vector, each
                                                         % element is a list of
                                                         % altnames
    % convert single altname strings to cells
    singleIx = cellfun('isclass', allAltNames, 'char');
    wrappedAlts = mat2cell(allAltNames,1,ones(1,length(allAltNames)));
    allAltNames(singleIx) = wrappedAlts(singleIx);

    nAltNames = cellfun('length',allAltNames); % vector of length nRealNames
    optNameIndex = cell(sum(nAltNames),2); % cell matrix: first row is optname,
                                           % second is realname
                                           % fill in the index structure
    curOffset = 1;
    for iReal=1:nRealNames
        for iAlt = 1:nAltNames(iReal)
            optNameIndex(curOffset,1:2) = { allAltNames{iReal}{iAlt}, ...
                                            realnames{iReal} };
            curOffset = curOffset+1;
        end
    end
end

% when we get here, we have a real optNameIndex, either from cache or computed.
% now find match
indexLoc = strcmp(optNameIndex(:,1), thisname);
if any(indexLoc)
    % got a match
    realname = optNameIndex{indexLoc, 2};
else
    % no alternative name, return the input
    realname = thisname;
end


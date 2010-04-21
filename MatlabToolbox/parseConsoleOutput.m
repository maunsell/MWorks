function l = parseConsoleOutput

fname = '~/Desktop/console.txt';

fid = fopen(fname, 'r');

% read lines
lines = {};
while 1
    tL = fgetl(fid);
    if tL == -1
        break
    else
        lines{end+1} = tL;
    end
end
nLines = length(lines);

% parse and reformat each line
%lines{1}

lastTs = [];
for iL = 1:nLines
    t = regexp(lines{iL}, ...
        '([0-9]*:[0-9]*:[0-9]*): *(.*) *\(([0-9]*)\)', 'tokens');

    if ~isempty(t)
        tTok = t{1};

        msg = tTok{2};  
        timeUs = str2num(tTok{3});
        
        justS = floor(timeUs/10^6);
        justMs = floor( (timeUs-justS*10^6)/10^3);
        justUs = floor( (timeUs-justS*10^6-justMs*10^3));
        
        if ~isempty(lastTs)
            thisDiff = timeUs - lastTs;
        else
            thisDiff = 0;
        end
        lastTs = timeUs;
        
        diffS = floor(thisDiff/10^6);
        diffMs = floor((thisDiff-diffS*10^6)/10^3);
        diffUs = floor(thisDiff-diffS*10^6-diffMs*10^3);
        
        
        fprintf(1, '%14.0f %5d:%03d:%03d abs, %5d:%03d:%03dus diff: %s\n', ...
            timeUs, justS, justMs, justUs, ...
            diffS, diffMs, diffUs, msg);
    end
end

if nargout > 0
    l = lines;
end



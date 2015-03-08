function [cint, cdos] = parallel_dos_import_csv(fileprefix)
% PARALLEL_DOS_IMPORT_CSV - Imports a multi-window DOS simulation from a CSV
% and stitches the results together using a fourth order accurate finite 
% difference method. Returns the final interval and relative DOS.
% 
% Inputs:
% fileprefix - Prefix to the CSV files containing DOS/histogram data.
% 
% Outputs:
% cint - Vector of bins over which the DOS was collected.
% cdos - Vector of relative density of states.

% Read in headers. Identify appropriate columns.
headers = textread(sprintf('%s.dos.csv',fileprefix), '%s', 1, 'delimiter', '\n');
headers = strsplit(headers{1},',');
if length(headers{end})  == 0
    headers(end) = [];
end

% Find id's, intervals and dos start.
colid  = find(strcmpi('identifier', headers) == 1); % Identifiers.
colimin = find(strcmpi('interval min', headers) == 1); % Interval min.
colimax = find(strcmpi('interval max', headers) == 1); % Interval max.
coldos = find(strcmpi('DOS', headers) == 1); % DOS begin.
colbcnt = find(strcmpi('bin count', headers) == 1); % Bin count.

% Import data 
A = csvread(sprintf('%s.dos.csv',fileprefix), 1);

% Clean up last column if need be. 
if(length(find(A(:,end) == 0)) == size(A,1))
    A(:,end) = [];
end

% Find all unique windows.
windows = unique(A(:,colid));

% If bin count is empty. Get it from hist file.
bincount = zeros(length(windows),1);
if(isempty(colbcnt))
    histheaders = textread(sprintf('%s.hist.csv',fileprefix), '%s', 1, 'delimiter', '\n');
    histheaders = strsplit(histheaders{1}, ',');
    colbcnt = find(strcmpi('bin count', histheaders) == 1);
    B = csvread(sprintf('%s.hist.csv',fileprefix), 1);
    for i=1:length(windows)
        j = find(B(:,colid) == windows(i),1,'last');
        bincount(i) = B(j,colbcnt);
    end
else % Get it from A.
    for i=length(windows)
        j = find(A(:,colid) == windows(i),1,'last');
        bincount(i) = A(j,colbcnt);
    end        
end 

% Get last elements associated with windows.
wlast = zeros(length(windows),1);
for i=1:length(windows)
    wlast(i) = find(A(:,colid) == windows(i), 1, 'last');
end

% Define intervals. 
intervals = zeros(length(windows), max(bincount));
for i=1:length(windows)
    x = (A(wlast(i),colimax)-A(wlast(i),colimin))/bincount(i);
    app = (A(wlast(i),colimin):x:(A(wlast(i),colimax))-x)';
    intervals(i,1:length(app)) = app;
end

% Pull all DOS data.
dos = zeros(length(windows), max(bincount));
for i=1:length(windows)
    dos(i,:) = A(wlast(i),coldos:end)-min(A(wlast(i),coldos:end));
end

cint = intervals(1,1:bincount(1));
cdos = dos(1,1:bincount(1));
for k=2:length(windows)
    tint = intervals(k,1:bincount(k));
    tdos = dos(k,1:bincount(k));
    
    % Define function derivative handles.
    df = @(dx,y,idx) (1/12*y(idx-2)-2/3*y(idx-1)+2/3*y(idx+1)-1/12*y(idx+2))/dx;
    
    % Use 5 point 1st order F.D. to match derivatives. Start 5 points in.
    dd = Inf; 
    di = [];
    for i=find(cint == min(tint))+5:length(cint)-5
        % Calculate derivative for cumulative dos.
        dx = cint(i)-cint(i-1);
        cdf = df(dx,cdos,i);
        
        % Find matching point and derivative for dos2.
        j = find(tint == cint(i));
        tdf = df(dx,tdos,j);
        
        % Calculate absolute value of derivative difference and store lowest
        % one.
        if abs(cdf-tdf) < dd
            dd = abs(cdf-tdf);
            di = [i, j];
        end
    end
    
    % merge two vectors. 
    cdos(di(1)+1:end) = []; % strip off end.
    cint(di(1):end) = []; % trim parameter inteval too.
    tdos = tdos - tdos(di(2)) + cdos(di(1)); % Rescale.
    cdos = [cdos(1:end-1), tdos(di(2):end)];
    cint = [cint, tint(di(2):end)];
   
   fprintf('Merged window %3i at %10.3f. df = %g \n', k, cint(di(1)), dd);
end

 % Re-scale final dos.
 cdos = cdos - min(cdos);
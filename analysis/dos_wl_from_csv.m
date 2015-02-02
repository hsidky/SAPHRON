function [lnge, his] = dos_wl_from_csv(filename, row, scale, N)

    A = csvread(filename,1);
    if row <= 0
        lnge = A(end+row,2:end)';
    else
        lnge = A(row,2:end)';
    end
    
    if length(lnge) > N
        his = lnge(1:N); 
        lnge = lnge((N+1):end);
    end
    
    if scale
        lnge = lnge - min(lnge);
    end
    
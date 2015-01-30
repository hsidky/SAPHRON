function lnge = dos_wl_from_csv(filename, row, scale)

    A = csvread(filename,1);
    if row <= 0
        lnge = A(end+row,2:end)';
    else
        lnge = A(row,2:end)';
    end
    
    if scale
        lnge = lnge - min(lnge);
    end
    
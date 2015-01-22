function [x,y,z,ux,uy,uz] = positions_vectors_csv(filename, cols, n, iter)
    A = csvread(filename, 1);
    
    first = n*(iter-1)+1;
    last  = first + n-1;  
    x = A(first:last,cols(1));
    y = A(first:last,cols(2));
    z = A(first:last,cols(3));
    ux = A(first:last,cols(4));
    uy = A(first:last,cols(5));
    uz = A(first:last,cols(6));
end
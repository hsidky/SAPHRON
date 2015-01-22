function th = angle_from_vec(x, uy, uz, n)

th = zeros(n,1);
for i=1:n
    idx = find(x == i);
    th(i) = acosd(sqrt(mean(uy(idx).^2)/(mean(uy(idx).^2)+mean(uz(idx).^2))));
end

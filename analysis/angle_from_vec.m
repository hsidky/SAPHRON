function th = angle_from_vec(x, ux, uy, uz, n)

th = zeros(n,1);
for i=1:n
    idx = find(x == i);
    u = [ux(idx), uy(idx), uz(idx)];
    Q = 3/2*((u'*u-1/3*eye(3))/length(u));
    [v,~] = eigs(Q);
    th(i) = acosd(dot([1,0,0],abs(v(:,1))'));
end
    
    


%{
idx = find(uz < 0);
uy(idx) = -uy(idx);
uz(idx) = -uz(idx);


th = zeros(n,1);
for i=1:n
    idx = find(x == i);
    th(i) = acosd(dot([1,0],[mean(abs(uy(idx))),mean(abs(uz(idx)))]));
end
%}
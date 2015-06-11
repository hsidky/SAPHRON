function [th,s2] = angle_from_vec(x, ux, uy, uz, n)

th = zeros(n,1);
s2 = zeros(n,1);
for i=1:n
    idx = find(x == i);
    u = [ux(idx), uy(idx), uz(idx)];
    
    Q = zeros(3);
    for j=1:size(u,1)
        Q = Q + u(j,:)'*u(j,:) - eye(3,3)/3;
    end
    Q = 3/(2*size(u,1))*Q;
    
    [v,s] = eigs(Q);
    [~,l] = max(diag(s));
    theta = 23.6244*normpdf(i,16,3);
    s2(i) = s(l,l);
    th(i) = 3/2*dot([0,sin(theta),cos(theta)],abs(v(:,l))')^2-1/2;
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
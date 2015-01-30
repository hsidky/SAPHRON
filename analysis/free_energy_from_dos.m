function [F, lnZ] = free_energy_from_dos(dos, E, kbT)
    F = zeros(length(kbT), 1);
    lnZ = zeros(length(kbT), 1);
    
    for i=1:length(kbT)
        lnZ(i) = max(dos-E/kbT(i)) + log(sum(exp(dos-E/kbT(i)-max(dos-E/kbT(i)))));
        F(i) = -kbT(i)*lnZ(i);
    end
end
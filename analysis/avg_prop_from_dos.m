function avg = avg_prop_from_dos(dos, prop, E, kbT)
    avg = zeros(length(kbT), 1);
    for i=1:length(avg)
        avg(i) = sum(prop.*exp(dos-E/kbT(i)-max(dos-E/kbT(i))))/sum(exp(dos-E/kbT(i)-max(dos-E/kbT(i))));
    end
end
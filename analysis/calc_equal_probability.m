function p = calc_equal_probability(f, x1, x2)

fobj = @(x)objective_function(f,x);

p = fminbnd(fobj, x1, x2, optimset('display', 'iter'));

function r = objective_function(f,b)
y = f(b);
[pks, locs]= findpeaks(y, 'MinPeakWidth', 10, 'MinPeakDistance', 100, 'SortStr', 'desc');

if length(pks) == 1
    error('peak_identify::There must be two identifiable peaks at the starting guess.');
end

% Find minimum between peaks 
[~, mloc] = min(y(min(locs):max(locs)));

mloc = mloc + min(locs);

% Integrate between peaks until ends 
i1 = trapz(y(min(locs):mloc));
i2 = trapz(y(mloc:max(locs)));

r = abs(i1 - i2);
    
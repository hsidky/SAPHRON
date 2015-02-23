function bopt = calc_equal_probability(fxn, xrng, brng)
% CALC_EQUAL_PROBABILITY - Applies the equal area rule to calculate phase
% equilibrium between two states. Expects the parameter bounds to exhibit 
% bimodal probability distribution. The routine identifies the two most 
% prominent peaks and the minimum (valley) between them and integrates
% between the bounds and the valley.
% 
% Inputs: 
% fxn  - Function handle f(x,b) to a probability distribution where x is the
%        independent variable and b is an adjustable parameter. 
% xrng - Independent parameter range over which to integrate fnx [xmin, xmax].
% brng - Bounds on the value of independent parameter [bmin, bmax].
% 
% Outputs:
% bopt - The adjustable parameter, bopt, at which the equal area rule is
%        satisfied.

fobj = @(b) objective_function(fxn,b,xrng);
bopt = fminbnd(fobj, brng(1), brng(2), optimset('TolX', 1e-7));

function r = objective_function(f, b, xrng)
x = linspace(xrng(1),xrng(2),1000)';
y = f(x,b);

mindx = 0.01*diff(xrng);
minxw = 0.01*diff(xrng);

% Get peaks
[~, xpks]= findpeaks(y, x, 'MinPeakWidth', minxw, 'MinPeakDistance', mindx, 'SortStr', 'desc');

if length(xpks) ~= 2
    error('peak_identify::There must be two identifiable peaks at the starting guess.');
end

[~,lloc] = min(abs(min(xpks)-x));
[~,uloc] = min(abs(max(xpks)-x));

% Find minimum between peaks 
[~, mloc] = min(y(lloc:uloc));
mloc = mloc + lloc;

% Integrate between peaks until ends 
i1 = integral(@(x)f(x,b), x(1), x(mloc));
i2 = integral(@(x)f(x,b), x(mloc), x(end));

fprintf('\n');
fprintf('  Identified peaks: %.5f       %.5f \n',x(lloc),x(uloc));
fprintf('  Peak integrals:   %.5f       %.5f \n', i1, i2);
fprintf('  Minimum between peaks:   %.5f \n', x(mloc));
r = abs(i1 - i2);
    
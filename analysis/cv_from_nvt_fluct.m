% cv_from_nvt - Hythem Sidky (c) 2015
%
% Changes: 
% 2015/01/17 Initial file.

function [cv] = cv_from_nvt_fluct(U, T, n)
% CV_FROM_NVT_FLUCT - Calculates the constant-volume heat capacity, Cv, from 
% NVT Ensemble energy data based on stat-mech fluctuation equation.
%
% Inputs:
% U - Column vector or matrix of energy (see 'n' below). 
% T - Column vector of temperatures corresponding to each column of U.
% 
% Optional:
%  n     - Number of particles in model. This is to be specified only if the
%         energy is recorded on a per particle basis. It is needed to
%         perform the correct calculation. 
% Outputs:
% cv    - A vector containing Cv values. 

if n 
    cv = var(U.*n)./(T'.^2*n);
else
    cv = var(U)./T'.^2;
end

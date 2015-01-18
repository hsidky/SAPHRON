% cv_from_nvt_csv - Hythem Sidky (c) 2015
%
% Changes: 
% 2015/01/17 Initial file.

function [cv] = cv_from_nvt_csv(files, rows, col, T, n)
% CV_FROM_NVT_CSV Calculates the constant-volume heat capacity, Cv, from 
% NVT Ensemble energy data based on stat-mech fluctuation equation. This 
% routine pulls in data from CSV file(s) and calls the routine cv_from_nvt
% to perform the actual calculation.
% 
% Inputs:
% files - A single or array of file structs to the CSV files.
%         Ex: files = dir('*.csv'); 
% rows  - Either a single integer indicating the row at which to begin
%         reading data, or a 2 element vector with start and end rows. Both
%         numbers are offset from zero.
% col   - The column containing the energy data, offset from zero. 
% T     - Column vector the length of files containing the temperatures for
%         each of the 
% 
% Optional:
% n     - Number of particles in model. This is to be specified only if the
%         energy is recorded on a per particle basis. It is needed to
%         perform the correct calculation. 
%
% Outputs:
% cv    - A vector containing Cv values. 

% Loop through and collect data.
if nargin < 5
    n = 0; 
end

U = [];
for i=1:length(files)
    file = files(i);
    % Only if file contains data
    if file.bytes > 0
        if length(rows) == 1
            U(:,i) = csvread(files(i).name, rows, col);
        else
            U(:,i) = csvread(files(i).name, rows(1), col, [rows(1), col, rows(2), col]);
        end        
    end
end

cv = cv_from_nvt(U,T,n);
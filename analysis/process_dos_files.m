function process_dos_files(prefixes)

for i=1:length(prefixes)
    prefix = prefixes{i};
    fprintf('*** Processing File with prefix %s ***\n', prefix);
    try 
        [cint, cdos] = parallel_dos_import_csv(prefix);
        save(sprintf('%s.mat',prefix), 'cint', 'cdos');
        fprintf('*** File processed successfully ***\n\n');
    catch
        fprintf('*** ERROR: processing file prefix %s ***\n\n', prefix);
    end    
end
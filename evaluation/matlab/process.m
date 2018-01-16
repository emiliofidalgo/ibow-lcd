function process(directory, prev, cons_loops, inliers)
    % Getting dataset information and loading files
    loops_filename = strcat(directory, 'loops_000.txt');
    loops_file = load(loops_filename);
    
    % Reading info file    
    info_filename = strcat(directory, 'info.json');
    fid = fopen(info_filename);
    raw = fread(fid,inf);
    str = char(raw');
    fclose(fid);
    json_info = jsondecode(str);
    
    % Ground truth    
    gt_filename = json_info.gt_file;
    gt_file = load(gt_filename);
    
    % Coordinate file    
    coords_filename = json_info.coords_file;
    %coords_file = load(coords_filename);

    % Processing the resulting file to transform the format
    loops_trans_file = detect_loops(loops_file, prev, cons_loops, inliers);
    compute_PR(loops_trans_file, gt_file, 10, false);
end
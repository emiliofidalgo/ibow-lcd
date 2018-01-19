% Script for obtaining the required results for IROS'18
base_dir = '/home/emilio/Escritorio/ibow-lcd/';
gt_neigh = 10;
compensate = false;

% Configuring subpaths
addpath('AcademicFigures/');

% Obtaining CityCenter results
curr_dir = strcat(base_dir, 'CityCentre/');
[PR_CC, imgvssize_CC, imgvstime_CC] = process(curr_dir, gt_neigh, compensate);

curr_dir = strcat(base_dir, 'NewCollege/');
[PR_NC, imgvssize_NC, imgvstime_NC] = process(curr_dir, gt_neigh, compensate);

curr_dir = strcat(base_dir, 'Lip6In/');
[PR_L6I, imgvssize_L6I, imgvstime_L6I] = process(curr_dir, gt_neigh, compensate);

curr_dir = strcat(base_dir, 'Lip6Out/');
[PR_L6O, imgvssize_L6O, imgvstime_L6O] = process(curr_dir, gt_neigh, compensate);

% P/R curves
afigure;
hold on;
plot(PR_CC.R, PR_CC.P, '-o', 'MarkerIndices', length(PR_CC.P));
plot(PR_NC.R, PR_NC.P, '-*', 'MarkerIndices', length(PR_NC.P));
plot(PR_L6I.R, PR_L6I.P, '-x', 'MarkerIndices', length(PR_L6I.P));
plot(PR_L6O.R, PR_L6O.P, '--s', 'MarkerIndices', length(PR_L6O.P));
xlabel('Recall');
ylabel('Precision');
xlim([0., 1.02]);
ylim([0., 1.02]);
legend('CC', 'NC', 'L6I', 'L6O', 'Location', 'SouthWest');
hold off;
print('-depsc', strcat(base_dir, 'PR_curves'));

% Images vs Size
afigure;
hold on;
plot(imgvssize_CC.img, imgvssize_CC.size, '-o', 'MarkerIndices', length(imgvssize_CC.size));
plot(imgvssize_NC.img, imgvssize_NC.size, '-*', 'MarkerIndices', length(imgvssize_NC.size));
plot(imgvssize_L6I.img, imgvssize_L6I.size, '-*', 'MarkerIndices', length(imgvssize_L6I.size));
plot(imgvssize_L6O.img, imgvssize_L6O.size, '--s', 'MarkerIndices', length(imgvssize_L6O.size));
xlabel('Time Index');
ylabel('Vocabulary Size (Words)');
legend('CC', 'NC', 'L6I', 'L6O', 'Location', 'NorthWest');
hold off;
print('-depsc', strcat(base_dir, 'imgs_vs_size'));

% Images vs Time
afigure;
hold on;
plot(imgvstime_CC.img, imgvstime_CC.time, '-o', 'MarkerIndices', length(imgvstime_CC.time));
plot(imgvstime_NC.img, imgvstime_NC.time, '-*', 'MarkerIndices', length(imgvstime_NC.time));
plot(imgvstime_L6I.img, imgvstime_L6I.time, '-*', 'MarkerIndices', length(imgvstime_L6I.time));
plot(imgvstime_L6O.img, imgvstime_L6O.time, '--s', 'MarkerIndices', length(imgvstime_L6O.time));
xlabel('Time Index');
ylabel('Avg. Time (ms)');
legend('CC', 'NC', 'L6I', 'L6O', 'Location', 'NorthWest');
hold off;
print('-depsc', strcat(base_dir, 'imgs_vs_time'));

% Showing summaries
disp('----- Summary CC -----');
disp(['Max P: ', num2str(PR_CC.P_max)]);
disp(['Max R: ', num2str(PR_CC.R_max)]);
disp(['Max VWords: ', num2str(imgvssize_CC.size(end))]);
disp(['Avg. Time: ', num2str(mean(imgvstime_CC.time))]);
disp(['Std. Time: ', num2str(std(imgvstime_CC.time))]);

disp('----- Summary NC -----');
disp(['Max P: ', num2str(PR_NC.P_max)]);
disp(['Max R: ', num2str(PR_NC.R_max)]);
disp(['Max VWords: ', num2str(imgvssize_NC.size(end))]);
disp(['Avg. Time: ', num2str(mean(imgvstime_NC.time))]);
disp(['Std. Time: ', num2str(std(imgvstime_NC.time))]);

disp('----- Summary L6I -----');
disp(['Max P: ', num2str(PR_L6I.P_max)]);
disp(['Max R: ', num2str(PR_L6I.R_max)]);
disp(['Max VWords: ', num2str(imgvssize_L6I.size(end))]);
disp(['Avg. Time: ', num2str(mean(imgvstime_L6I.time))]);
disp(['Std. Time: ', num2str(std(imgvstime_L6I.time))]);

disp('----- Summary L6O -----');
disp(['Max P: ', num2str(PR_L6O.P_max)]);
disp(['Max R: ', num2str(PR_L6O.R_max)]);
disp(['Max VWords: ', num2str(imgvssize_L6O.size(end))]);
disp(['Avg. Time: ', num2str(mean(imgvstime_L6O.time))]);
disp(['Std. Time: ', num2str(std(imgvstime_L6O.time))]);

close all;
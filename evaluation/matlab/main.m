% Script for obtaining the required results for IROS'18
base_dir = '/home/emilio/Escritorio/ibow-lcd/';
gt_neigh = 10;
compensate = false;

% Configuring subpaths
addpath('AcademicFigures/');

% Obtaining CityCenter results
curr_dir = strcat(base_dir, 'CityCentre/');
[PR_CC, imgvssize_CC, imgvstime_CC] = process(curr_dir, gt_neigh, compensate);

% P/R curves
afigure;
hold on;
plot(PR_CC.R, PR_CC.P, '-o', 'MarkerIndices', length(PR_CC.P));
xlabel('Recall');
ylabel('Precision');
xlim([0., 1.02]);
ylim([0., 1.02]);
legend('CC', 'Location', 'SouthWest');
hold off;
print('-depsc', strcat(base_dir, 'PR_curves'));

% Images vs Size
afigure;
hold on;
plot(imgvssize_CC.img, imgvssize_CC.size, '-o', 'MarkerIndices', length(imgvssize_CC.size));
xlabel('Time Index');
ylabel('Vocabulary Size (Words)');
legend('CC', 'Location', 'NorthWest');
hold off;
print('-depsc', strcat(base_dir, 'imgs_vs_size'));

% Images vs Time
afigure;
hold on;
plot(imgvstime_CC.img, imgvstime_CC.time, '-o', 'MarkerIndices', length(imgvstime_CC.time));
xlabel('Time Index');
ylabel('Avg. Time (ms)');
legend('CC', 'Location', 'NorthWest');
hold off;
print('-depsc', strcat(base_dir, 'imgs_vs_time'));

% Showing summaries
disp('----- Summary CC -----');
disp(['Max P: ', num2str(PR_CC.P_max)]);
disp(['Max R: ', num2str(PR_CC.R_max)]);
disp(['Max VWords: ', num2str(imgvssize_CC.size(end))]);
disp(['Avg. Time: ', num2str(mean(imgvstime_CC.time))]);
disp(['Std. Time: ', num2str(std(imgvstime_CC.time))]);

close all;
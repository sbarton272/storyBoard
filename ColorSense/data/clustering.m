%% Clustering on sensor data
% Read in data
clear all
close all

FID = fopen('analyzeColorSense.txt');
DATA = textscan(FID, '%s\t%d\t%d\t%d');
fclose(FID);

DATA = double([DATA{1,2}, DATA{1,3}, DATA{1,4}]); % ignore label

opts = statset('Display','final');
[idx,ctrs] = kmeans(DATA,4, 'replicates',5,'Options',opts);

% plot   
X = DATA(:,1);
Y = DATA(:,2);
Z = DATA(:,3);
scatter3(X,Y,Z,'r.');
hold on
X = ctrs(:,1);
Y = ctrs(:,2);
Z = ctrs(:,3);
scatter3(X,Y,Z,'kx');

disp(ctrs);

%   749.9200  502.8300  804.7500
%   727.9500  388.0200  737.4700
%   768.9100  352.8800  763.3800
%   771.0300  411.5700  703.1000
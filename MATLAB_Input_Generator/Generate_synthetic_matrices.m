outdir = '../synthetic_matrices/';

m = 8000;
b = 4;

% banded -1:1
A = zeros(m,m);
for i = -1:1
   diagonal = ones(m-abs(i),1); 
   A = A + diag(diagonal,i);
end
band1 = strcat(outdir,'A_band_-1_1.mat');
save(band1,'A');


% banded -2:2
A = zeros(m,m);
for i = -2:2
   diagonal = ones(m-abs(i),1); 
   A = A + diag(diagonal,i);
end
band2 = strcat(outdir,'A_band_-2_2.mat');
save(band2,'A');


% banded -4:4
A = zeros(m,m);
for i = -4:4
   diagonal = ones(m-abs(i),1); 
   A = A + diag(diagonal,i);
end
band4 = strcat(outdir,'A_band_-4_4.mat');
save(band4,'A');


% banded -8:8
A = zeros(m,m);
for i = -8:8
   diagonal = ones(m-abs(i),1); 
   A = A + diag(diagonal,i);
end
band8 = strcat(outdir,'A_band_-8_8.mat');
save(band8,'A');


% banded -16:16
A = zeros(m,m);
for i = -16:16
   diagonal = ones(m-abs(i),1); 
   A = A + diag(diagonal,i);
end
band16 = strcat(outdir,'A_band_-16_16.mat');
save(band16,'A');


% banded -32:32
A = zeros(m,m);
for i = -32:32
   diagonal = ones(m-abs(i),1); 
   A = A + diag(diagonal,i);
end
band32 = strcat(outdir,'A_band_-32_32.mat');
save(band32,'A');


% diagonal
diagonal = ones(m,1);
A = diag(diagonal);
diagonal = strcat(outdir,'A_diagonal.mat');
save(diagonal,'A');


% random 0.0001
density = 0.0001;
A = spones(sprand(m,m,density));
rand0001 = strcat(outdir,'A_rand_0.0001.mat');
save(rand0001,'A');


% random 0.001
density = 0.001;
A = spones(sprand(m,m,density));
rand001 = strcat(outdir,'A_rand_0.001.mat');
save(rand001,'A');


% random 0.01
density = 0.01;
A = spones(sprand(m,m,density));
rand01 = strcat(outdir,'A_rand_0.01.mat');
save(rand01,'A');


% random 0.1
density = 0.1;
A = spones(sprand(m,m,density));
rand1 = strcat(outdir,'A_rand_0.1.mat');
save(rand1,'A');


% random 0.2
density = 0.2;
A = spones(sprand(m,m,density));
rand2 = strcat(outdir,'A_rand_0.2.mat');
save(rand2,'A');


% random 0.3
density = 0.3;
A = spones(sprand(m,m,density));
rand3 = strcat(outdir,'A_rand_0.3.mat');
save(rand3,'A');


% random 0.4
density = 0.4;
A = spones(sprand(m,m,density));
rand4 = strcat(outdir,'A_rand_0.4.mat');
save(rand4,'A');


% random 0.5
density = 0.5;
A = spones(sprand(m,m,density));
rand5 = strcat(outdir,'A_rand_0.5.mat');
save(rand5,'A');


% random 0.6
density = 0.6;
A = spones(sprand(m,m,density));
rand6 = strcat(outdir,'A_rand_0.6.mat');
save(rand6,'A');



%----------------------------
% B = load('A_rand_10.mat','A');
% C = B.A;
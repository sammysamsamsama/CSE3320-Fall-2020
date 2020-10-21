compiled using these commands:
   g++ nguyen_6565.c -o nguyen_6565 -g
   g++ insertionSort.c -o sort -g
run using this command:
   ./nguyen_6565
The program will read from all_month.csv,
store time, latitude, longitude, depth, and mag

sort by lat in 1 ps, approx 17 sec.
system() move data to 1ps.csv

distribute data into 0.dat and 1.dat
sort by lat in 2 ps, approx 4 sec.
merge 0.dat and 1.dat into 2.dat
system() move 2.dat to 2ps.csv

distribute data into 0.dat, 1.dat, 2.dat, and 3.dat
sort by lat in 4 ps, approx 1 sec.
merge .dat files into 6.dat
system() move 6.dat to 4ps.csv

distribute data into 0.dat - 9.dat
sort by lat in 10 ps, 0~1 sec.
merge 0.dat - 9.dat into 18.dat
system() move 18.dat into 10ps.csv
system() rm *.dat

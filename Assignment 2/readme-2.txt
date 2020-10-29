compiled using this command:
   g++ -pthread nguyen_6565.c -o nguyen_6565 -g
run using this command:
   ./nguyen_6565
The program will read from all_month.csv,
store time, latitude, longitude, depth, and mag

sort by lat in 1 thread, approx 16 sec.

sort by lat in 2 threads, approx 4 sec.
merge

sort by lat in 4 threads, approx 2 sec.
merge

sort by lat in 10 threads, approx 1 sec.
merge

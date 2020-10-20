all: nguyen_6565 sort merge

nguyen_6565:
	g++ nguyen_6565.c -o nguyen_6565 -g

sort:
	g++ insertionSort.c -o sort -g

merge:
	g++ merge.c -o merge -g

clean:
	rm -f nguyen_6565
	rm -f merge
	rm -f sort
	find . -name "*.dat" -exec rm -rf {} \;

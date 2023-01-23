default: shelob.c
	clang -Ofast -fopenmp=libomp shelob.c -o shelob
	strip --strip-all shelob

debug: shelob.c
	clang -g -fopenmp=libomp shelob.c -o shelob

clean:
	rm -f *~ shelob *.core

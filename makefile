main:
	gcc main.c -lleptonica -ltesseract -lpthread -O2 -o main
	#gcc -O2 -c main.c -lleptonica -ltesseract -o main.o
	#gcc -o main main.o

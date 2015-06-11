all:
	@gcc -std=c99 -c BookFile.c
	@gcc -std=c99 -c main.c
	@gcc -g main.o BookFile.o -o program
run:
	@./program
clean:
	rm -f *.o main
debug:
	valgrind --tool=memcheck --leak-check=yes ./program
zip:
	zip -l Source.zip  ./*.c ./*.h ./Makefile

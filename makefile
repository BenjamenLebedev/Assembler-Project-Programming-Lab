all: mid 

mid: mid.o vector.o t_unit.o back.o frontend_analysis.o frontend.o assembler.o pre.o
	gcc -ansi -pedantic -Wall -o mid mid.o vector.o t_unit.o back.o frontend_analysis.o frontend.o assembler.o pre.o -lm

mid.o : mid.c vector.h ast_structs.h
	gcc -ansi -pedantic -Wall -c mid.c

vector.o : vector.c vector.h
	gcc -ansi -pedantic -Wall -c vector.c

t_unit.o : t_unit.c t_unit.h
	gcc -ansi -pedantic -Wall -c t_unit.c	

back.o : back.c back.h
	gcc -ansi -pedantic -Wall -c back.c	

frontend_analysis.o : frontend_analysis.c 
	gcc -ansi -pedantic -Wall -c frontend_analysis.c	

frontend.o : frontend.c 
	gcc -ansi -pedantic -Wall -c frontend.c			

assembler.o : assembler.c 
	gcc -ansi -pedantic -Wall -c assembler.c		

pre.o : pre.c pre.h
	gcc -ansi -pedantic -Wall -c pre.c		

clean:
	rm *.o mid 
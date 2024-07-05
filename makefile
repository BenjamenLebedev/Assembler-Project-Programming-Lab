# Compiler
CC = gcc
CFLAGS = -Wall -ansi -pedantic -g
FLAGS = -Wall -pedantic -g
ASAN = -fsanitize=address
FOLDERNAME = Assembler-Project
SRCS = ${shell ls ./Source_Files/*.c}
OBJS = $(SRCS:.c=.o)
LIB = ./Header_Files/*.h 
OUT = ${shell ls *.am *.ob *.ent *.ext *.txt}
ARGS = attempt1
# Executable
TARGET = assembler

# Default target
all: $(TARGET)

# Compile source files into object files
%.o: %.c $(LIB)
	$(CC) $(FLAGS) $(ASAN) $ -c $< -o $@

# Link object files into executable
$(TARGET): $(OBJS) $(LIB)
	$(CC) $(FLAGS) $(ASAN) $(OBJS) -o $@

# Clean up object files and executable
clean:
	rm -f $(OBJS) $(TARGET) $(OUT)

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET) $(ARGS)
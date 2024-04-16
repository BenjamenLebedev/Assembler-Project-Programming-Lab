# Compiler
CC = gcc
CFLAGS = -Wall -ansi -pedantic -g
FOLDERNAME = Assembler-Project
SRCS = ${shell ls ./Source_Files/*.c}
OBJS = $(SRCS:.c=.o)
LIB = ./Header_Files/*.h 
OUT = ${shell ls *.am *.ob *.ent *.ext *.txt}
# Executable
TARGET = mid

# Default target
all: $(TARGET)

# Compile source files into object files
%.o: %.c $(LIB)
	$(CC) $(CFLAGS) $ -c $< -o $@

# Link object files into executable
$(TARGET): $(OBJS) $(LIB)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Clean up object files and executable
clean:
	rm -f $(OBJS) $(TARGET) $(OUT)
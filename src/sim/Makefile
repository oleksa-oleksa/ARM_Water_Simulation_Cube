#personal information
CC = gcc -Wall -Wextra -Wstrict-aliasing -pedantic -Werror -std=c11 -ggdb
PRG = simulation_test

CFILES += main.c
CFILES += particle.c
CFILES += particle_linux_display.c

CFLAGS += -D_BSD_SOURCE
CFLAGS += -D_DEFAULT_SOURCE

CFLAGS += -DDEBUG_LEVEL=4		#change for different debugg message outputs

LFLAGS += -L/usr/X11/lib 
LFLAGS += -lX11
LFLAGS +=  -lstdc++

INC += .




INC_PARAMS=$(foreach d, $(INC), -I$d)
VPATH = $(INC)
OBJ = $(CFILES:%.c=%.o)

$(PRG): $(OBJ)
	$(CC) $(INC_PARAMS) -o $(PRG) $(OBJ) -lm  $(LFLAGS)

%.o: %.c
	$(CC) $(INC_PARAMS) -c $(CFLAGS) $<

all: clean $(PRG) run

clean:
	rm $(OBJ) $(PRG) easter_cmd.srcl -f

run:
	./$(PRG)
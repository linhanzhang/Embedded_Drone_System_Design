CC=gcc
CFLAGS = -g -Wextra -Wall -Werror -pthread -lm
EXEC = ./pc-terminal
INCLUDIES = pc_terminal.h 
SRC_OBJ = ../comms/packet.o ./console_io.o ./serial_io.o ../comms/packet_tx.o ../comms/packet_rx.o ../comms/packet_builder.o ./joystick.o flight_packet_rx.o

default: $(SRC_OBJ)
	$(CC) $(CFLAGS) pc_terminal.c -o $(EXEC) $^

%.o: %.c
	$(CC) -o $@ -c $^ $(CFLAGS)

clean:
	rm $(EXEC) $(SRC_OBJ)

run: default
	$(EXEC)

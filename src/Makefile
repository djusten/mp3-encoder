# Created by Diogo Justen (diogojusten@gmail.com) in November 2017
# Copyright (C) 2017 Diogo Justen. All rights reserved.

.PHONY = all clean

INCLUDES = -I.
LDFLAGS = -static -Wall -Werror -ggdb
OBJS = mp3_encoder_main.c mp3_encoder.o

LIBS = -lpthread -lmp3lame

all: mp3_encoder

mp3_encoder: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) $(INCLUDES)

mp3_encoder_main.o: mp3_encoder_main.c

mp3_encoder.o: mp3_encoder.c mp3_encoder.o

clean:
	rm -f *~ *.o mp3_encoder

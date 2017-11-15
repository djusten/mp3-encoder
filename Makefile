# Created by Diogo Justen (diogojusten@gmail.com) in November 2017
# Copyright (C) 2017 Diogo Justen. All rights reserved.

DIR = src
VERSION=1.0

#CFLAGS="$(LIBDNP3_CDEFS)"

all: #test
	for dir in $(DIR); do $(MAKE) CFLAGS=$(CFLAGS) -C $$dir $@ || exit $$?; done

#test:
#	$(MAKE) CFLAGS=$(CFLAGS) -C ./tests test LIBINCLUDE_DIR=$(HOST_DIR)/usr/include/

clean:
	for dir in $(DIR); do $(MAKE) -C $$dir $@; done

#EOF

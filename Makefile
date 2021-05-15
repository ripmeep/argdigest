CC=gcc
SOURCE_DIR=src/
SOURCES=argdigest.c
HEADERS=argdigest.h
CFLAGS=-fPIC -shared
OUT_DIR=lib/
OUT=libargdigest.so
LIBLOC=/usr/lib/
INCLUDE_LOC=/usr/include/

all: $(SOURCE_DIR)/$(SOURCES)
	mkdir $(OUT_DIR)
	$(CC) $(SOURCE_DIR)$(SOURCES) $(CFLAGS) -o $(OUT_DIR)$(OUT)
	cp $(OUT_DIR)$(OUT) $(LIBLOC)
	cp $(SOURCE_DIR)$(HEADERS) $(INCLUDE_LOC)

clean:
	rm -f $(LIBLOC)$(OUT)
	rm -f -r $(OUT_DIR)
	rm -f $(INCLUDE_LOC)$(HEADERS)

# makefile

RESULT_FILE=result
SOURCE=mavlink_udp.c
INCLUDE_PATH=$(shell pwd)/include/common
	
$(RESULT_FILE): $(SOURCE)
	gcc -std=c99 -I $(INCLUDE_PATH) -o $(RESULT_FILE) $(SOURCE)

.PHONY: clean
clean:
	rm $(RESULT_FILE)
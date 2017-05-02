CC = g++
CFLAGS = -c -Wall -pedantic
SHAREDFLAG = -shared
STAND:=  -std=c++0x
APP = mysvchost
APP_SOURCES = logger.cpp
APP_OBJECTS = $(APP_SOURCES:.c=.o)

$(APP): $(APP_OBJECTS)
	$(CC) $(APP_OBJECTS) $(STAND) -o $@
	
.c.o:
	$(CC) $(CFLAGS) $(STAND) $< -o $@

.PHONY: clean
clean:
	del $(APP_OBJECTS) $(LIB_OBJECTS)

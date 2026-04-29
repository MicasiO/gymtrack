TARGET = gymtrack

CC = gcc

TESTFLAGS = -g -fsanitize=address -lncursesw -lform -lm -Wall
FLAGS =  -lncursesw -lform -lm 

SOURCES = main.c create.c utils.c form.c menu.c routine.c active.c exercise.c stats.c serializer.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) -o $(TARGET) $(FLAGS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(TARGET)

test:
	$(CC) $(SOURCES) -o $(TARGET) $(TESTFLAGS)

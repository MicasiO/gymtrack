FLAGS = -g -fsanitize=address -lncursesw -lform -lm
FILES = main.c create.c utils.c form.c menu.c routine.c active.c exercise.c stats.c

gdb: compile
	gdb ./gymtrack

compile: 
	gcc -o gymtrack $(FLAGS) $(FILES)

run: compile
	./gymtrack

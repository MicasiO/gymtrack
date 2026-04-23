FLAGS = -g -fsanitize=address -lncursesw -lform
FILES = main.c create.c utils.c form.c menu.c routine.c active.c

gdb: compile
	gdb ./gymtrack

compile: 
	gcc -o gymtrack $(FLAGS) $(FILES)

run: compile
	./gymtrack

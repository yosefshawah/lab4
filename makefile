all: hexedit task4
hexedit:
	gcc -Wall -g -m32 task1.c -o hexedit

task4: 
	gcc -fno-pie -fno-stack-protector -m32 task4.c -o task4

.PHONY: clean
	
clean:
	rm -f hexedit task4

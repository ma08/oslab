a.out: parent.o child.out
	./a.out
child.out: child.c
	gcc -o child.out child.c
parent.o: parent.c
	gcc parent.c
clean:
	rm a.out child.out
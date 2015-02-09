child.out: child.c a.out
	gcc -o child.out child.c
a.out: parent.c
	gcc parent.c
clean:
	rm a.out child.out

all: hashtable

hashtable: hashtable.c hash.c
	cc -Wall -fstrict-aliasing -fkeep-inline-functions -fno-stack-protector -pedantic -ansi -O4 hash.c hashtable.c -S

.PHONY: clean
clean:
	rm -f *.o
	rm -f hashtable

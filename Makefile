all:
	gcc -I./include src/drwlm.c -o drwlm -lcpg -lcorosync_common

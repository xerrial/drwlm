all:
	gcc src/drwlm.c -o drwlm -lcpg -lcorosync_common

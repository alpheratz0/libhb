/*
   
	cc -std=c99 -pedantic -Wall -Wextra -Os -s \
   			-o stadium_print stadium_print.c -ljq -lhb

*/
#include <hb/stadium.h>

int
main(int argc, char **argv)
{
	struct hb_stadium *stadium;
	if (argc <= 1)
		return 1
	stadium = hb_stadium_from_file(argv[1]);
	if (!stadium)
		return 1;
	hb_stadium_print(stadium);
	hb_stadium_free(stadium);
	return 0;
}

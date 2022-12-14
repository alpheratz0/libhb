#include <hb/stadium.h>

int
main(int argc, char **argv)
{
	struct hb_stadium *stadium;
	stadium = hb_stadium_from_file(
		argc > 1 ? argv[1] : "stadiums/big.hbs");
	if (!stadium)
		return 1;
	hb_stadium_print(stadium);
	hb_stadium_free(stadium);
	return 0;
}

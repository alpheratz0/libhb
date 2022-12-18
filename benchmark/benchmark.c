#include <stdio.h>
#include <time.h>
#include <hb/stadium.h>

#define benchmark(fn) \
do { \
	clock_t s, e; \
	s = clock(); \
	fn(); \
	e = clock(); \
	printf("%-40s %.1fms\n", #fn, ((float)(e-s))/(CLOCKS_PER_SEC/1000)); \
} while (0)

static void
parse_stadium_and_free(const char *p)
{
	struct hb_stadium *s;
	s = hb_stadium_from_file(p);
	hb_stadium_free(s);
}

static void
parse_big_stadium(void) {
	parse_stadium_and_free("stadiums/big.json"); }

static void
parse_fish_hunt_stadium(void) {
	parse_stadium_and_free("stadiums/fish_hunt.json"); }

int
main(void)
{
	printf("\n");
	benchmark(parse_big_stadium);
	benchmark(parse_fish_hunt_stadium);
	return 0;
}

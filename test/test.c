#include <hb/stadium.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static struct hb_stadium *
_test_load(const char *path)
{
	printf("[test] %40s\n", path);
	return hb_stadium_from_file(path);
}

static void
test_invalid_json(void)
{
	assert(NULL == _test_load("test_invalid.json"));
}

static void
test_name_missing(void)
{
	assert(NULL == _test_load("test_name_missing.json"));
}

static void
test_name(void)
{
	struct hb_stadium *s;
	assert(NULL != (s = _test_load("test_name.json")));
	assert(!strcmp(s->name, "test_name"));
	hb_stadium_free(s);
}

int
main(void)
{
	test_invalid_json();
	test_name_missing();
	test_name();
	return 0;
}

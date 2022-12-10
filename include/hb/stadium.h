#ifndef __LIBHB_STADIUM_H__
#define __LIBHB_STADIUM_H__

struct hb_stadium {
	char           *name;
	float   camera_width;
	float  camera_height;
};

extern struct hb_stadium *
hb_stadium_parse(const char *in);

extern struct hb_stadium *
hb_stadium_from_file(const char *file);

extern void
hb_stadium_free(struct hb_stadium *s);

#endif

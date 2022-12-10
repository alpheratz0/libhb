#ifndef __LIBHB_STADIUM_H__
#define __LIBHB_STADIUM_H__

#include <stdbool.h>

enum hb_camera_follow {
	HB_CAMERA_FOLLOW_PLAYER,
	HB_CAMERA_FOLLOW_BALL
};

struct hb_stadium {
	char                              *name;
	float                      camera_width;
	float                     camera_height;
	float                    max_view_width;
	enum hb_camera_follow     camera_follow;
	float                    spawn_distance;
	bool                      can_be_stored;
};

extern struct hb_stadium *
hb_stadium_parse(const char *in);

extern struct hb_stadium *
hb_stadium_from_file(const char *file);

extern void
hb_stadium_free(struct hb_stadium *s);

#endif

#ifndef __LIBHB_STADIUM_H__
#define __LIBHB_STADIUM_H__

#include <stdbool.h>

enum hb_camera_follow {
	HB_CAMERA_FOLLOW_PLAYER,
	HB_CAMERA_FOLLOW_BALL
};

enum hb_kick_off_reset {
	HB_KICK_OFF_RESET_PARTIAL,
	HB_KICK_OFF_RESET_FULL
};

struct hb_stadium {
	char                              *name;
	float                      camera_width;
	float                     camera_height;
	float                    max_view_width;
	enum hb_camera_follow     camera_follow;
	float                    spawn_distance;
	bool                      can_be_stored;
	enum hb_kick_off_reset   kick_off_reset;
};

extern struct hb_stadium *
hb_stadium_parse(const char *in);

extern struct hb_stadium *
hb_stadium_from_file(const char *file);

extern void
hb_stadium_free(struct hb_stadium *s);

#endif

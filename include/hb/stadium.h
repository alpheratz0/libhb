#ifndef __LIBHB_STADIUM_H__
#define __LIBHB_STADIUM_H__

#include <stdbool.h>
#include <stdint.h>

enum hb_camera_follow {
	HB_CAMERA_FOLLOW_PLAYER,
	HB_CAMERA_FOLLOW_BALL
};

enum hb_kick_off_reset {
	HB_KICK_OFF_RESET_PARTIAL,
	HB_KICK_OFF_RESET_FULL
};

enum hb_background_type {
	HB_BACKGROUND_TYPE_NONE,
	HB_BACKGROUND_TYPE_GRASS,
	HB_BACKGROUND_TYPE_HOCKEY
};

struct hb_background {
	enum hb_background_type            type;
	float                             width;
	float                            height;
	float                   kick_off_radius;
	float                     corner_radius;
	float                         goal_line;
	uint32_t                          color;
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
	struct hb_background                *bg;
};

extern struct hb_stadium *
hb_stadium_parse(const char *in);

extern struct hb_stadium *
hb_stadium_from_file(const char *file);

extern void
hb_stadium_free(struct hb_stadium *s);

#endif

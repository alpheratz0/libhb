#ifndef __LIBHB_STADIUM_H__
#define __LIBHB_STADIUM_H__

#include <stdbool.h>
#include <stdint.h>
#include <hb/background.h>
#include <hb/trait.h>
#include <hb/vertex.h>
#include <hb/segment.h>
#include <hb/goal.h>
#include <hb/disc.h>
#include <hb/plane.h>
#include <hb/joint.h>
#include <hb/point.h>

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
	struct hb_background                *bg;
	struct hb_trait                **traits;
	struct hb_vertex             **vertexes;
	struct hb_segment            **segments;
	struct hb_goal                  **goals;
	struct hb_disc                  **discs;
	struct hb_plane                **planes;
	struct hb_joint                **joints;
	struct hb_disc            *ball_physics;
	struct hb_point      **red_spawn_points;
	struct hb_point     **blue_spawn_points;
};

extern struct hb_stadium *
hb_stadium_parse(const char *in);

extern struct hb_stadium *
hb_stadium_from_file(const char *file);

extern char *
hb_stadium_to_json(const struct hb_stadium *s);

extern void
hb_stadium_free(struct hb_stadium *s);

#endif

#include "include/hb/stadium.h"
#include <hb/stadium.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jv.h>

static long
_get_file_size(FILE *fp)
{
	long   current_pos;
	long     file_size;

	current_pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, current_pos, SEEK_SET);

	return file_size;
}

static void
_err_unmatched_property_type(const char *prop_name,
                             jv_kind expected, jv_kind got)
{
	fprintf(stderr, "hb_stadium: unmatched type "
		"for property \"%s\" expected %s got %s\n",
		prop_name, jv_kind_name(expected), jv_kind_name(got));
}

static void
_err_unmatched_property_value(const char *prop_name,
                              const char *valid_values, const char *got_value)
{
	fprintf(stderr, "hb_stadium: unmatched value "
		"for property \"%s\" expected %s got %s\n",
		prop_name, valid_values, got_value);
}

extern struct hb_stadium *
hb_stadium_parse(const char *in)
{
	struct hb_stadium *s;
	jv root;

	/////////////setup
	s = calloc(1, sizeof(struct hb_stadium));
	root = jv_parse(in);

	/////////////name
	{
		jv            name;
		jv_kind  name_kind;

		name = jv_object_get(jv_copy(root), jv_string("name"));
		name_kind = jv_get_kind(name);

		if (name_kind == JV_KIND_STRING) {
			s->name = strdup(jv_string_value(name));
		} else {
			_err_unmatched_property_type("name", JV_KIND_STRING, name_kind);
			goto err;
		}
	}

	/////////////cameraWidth & cameraHeight
	{
		jv             camera_width;
		jv            camera_height;
		jv_kind   camera_width_kind;
		jv_kind  camera_height_kind;

		camera_width = jv_object_get(jv_copy(root), jv_string("cameraWidth"));
		camera_height = jv_object_get(jv_copy(root), jv_string("cameraHeight"));

		camera_width_kind = jv_get_kind(camera_width);
		camera_height_kind = jv_get_kind(camera_height);

		if (camera_width_kind == JV_KIND_NUMBER &&
				camera_height_kind == JV_KIND_NUMBER) {
			s->camera_width = jv_number_value(camera_width);
			s->camera_height = jv_number_value(camera_height);
		} else if (camera_width_kind != JV_KIND_INVALID) {
			_err_unmatched_property_type("cameraWidth",
					JV_KIND_NUMBER, camera_width_kind);
			goto err;
		} else if (camera_height_kind != JV_KIND_INVALID) {
			_err_unmatched_property_type("cameraHeight",
					JV_KIND_NUMBER, camera_height_kind);
			goto err;
		} else {
			s->camera_width = s->camera_height = 0.0f;
		}
	}

	/////////////maxViewWidth
	{
		jv             max_view_width;
		jv_kind   max_view_width_kind;

		max_view_width = jv_object_get(jv_copy(root), jv_string("maxViewWidth"));
		max_view_width_kind = jv_get_kind(max_view_width);

		if (max_view_width_kind == JV_KIND_NUMBER) {
			s->max_view_width = jv_number_value(max_view_width);
		} else if (max_view_width_kind == JV_KIND_INVALID) {
			s->max_view_width = 0.0f;
		} else {
			_err_unmatched_property_type("maxViewWidth",
					JV_KIND_NUMBER, max_view_width_kind);
			goto err;
		}
	}

	/////////////cameraFollow
	{
		jv                  camera_follow;
		jv_kind        camera_follow_kind;
		const char     *camera_follow_str;

		camera_follow = jv_object_get(jv_copy(root), jv_string("cameraFollow"));
		camera_follow_kind = jv_get_kind(camera_follow);

		if (camera_follow_kind == JV_KIND_STRING) {
			camera_follow_str = jv_string_value(camera_follow);
			if (!strcmp(camera_follow_str, "ball")) {
				s->camera_follow = HB_CAMERA_FOLLOW_BALL;
			} else if (!strcmp(camera_follow_str, "player")) {
				s->camera_follow = HB_CAMERA_FOLLOW_PLAYER;
			} else {
				_err_unmatched_property_value("cameraFollow",
						"[ball, player]", camera_follow_str);
				goto err;
			}
		} else if (camera_follow_kind == JV_KIND_INVALID) {
			s->camera_follow = HB_CAMERA_FOLLOW_BALL;
		} else {
			_err_unmatched_property_type("cameraFollow",
					JV_KIND_STRING, camera_follow_kind);
			goto err;
		}
	}

	/////////////spawnDistance
	{
		jv                  spawn_distance;
		jv_kind        spawn_distance_kind;

		spawn_distance = jv_object_get(jv_copy(root), jv_string("spawnDistance"));
		spawn_distance_kind = jv_get_kind(spawn_distance);

		if (spawn_distance_kind == JV_KIND_NUMBER) {
			s->spawn_distance = jv_number_value(spawn_distance);
		} else if (spawn_distance_kind == JV_KIND_INVALID) {
			s->spawn_distance = 0.0f;
		} else {
			_err_unmatched_property_type("spawnDistance",
					JV_KIND_NUMBER, spawn_distance_kind);
			goto err;
		}
	}

	/////////////canBeStored
	{
		jv                  can_be_stored;
		jv_kind        can_be_stored_kind;

		can_be_stored = jv_object_get(jv_copy(root), jv_string("canBeStored"));
		can_be_stored_kind = jv_get_kind(can_be_stored);

		if (can_be_stored_kind == JV_KIND_TRUE ||
				can_be_stored_kind == JV_KIND_FALSE) {
			s->can_be_stored = can_be_stored_kind == JV_KIND_TRUE;
		} else if (can_be_stored_kind == JV_KIND_INVALID) {
			s->can_be_stored = true;
		} else {
			_err_unmatched_property_value("canBeStored",
					"[true, false]", jv_kind_name(can_be_stored_kind));
			goto err;
		}
	}

	/////////////kickOffReset
	{
		jv                  kick_off_reset;
		jv_kind        kick_off_reset_kind;
		const char     *kick_off_reset_str;

		kick_off_reset = jv_object_get(jv_copy(root), jv_string("kickOffReset"));
		kick_off_reset_kind = jv_get_kind(kick_off_reset);

		if (kick_off_reset_kind == JV_KIND_STRING) {
			kick_off_reset_str = jv_string_value(kick_off_reset);
			if (!strcmp(kick_off_reset_str, "partial")) {
				s->kick_off_reset = HB_KICK_OFF_RESET_PARTIAL;
			} else if (!strcmp(kick_off_reset_str, "full")) {
				s->kick_off_reset = HB_KICK_OFF_RESET_FULL;
			} else {
				_err_unmatched_property_value("kickOffReset",
						"[partial, full]", kick_off_reset_str);
				goto err;
			}
		} else if (kick_off_reset_kind == JV_KIND_INVALID) {
			s->kick_off_reset = HB_KICK_OFF_RESET_PARTIAL;
		} else {
			_err_unmatched_property_type("kickOffReset",
					JV_KIND_STRING, kick_off_reset_kind);
			goto err;
		}
	}

	/////////////bg
	{
		jv                  bg;
		jv_kind        bg_kind;

		s->bg = malloc(sizeof(struct hb_background));

		bg = jv_object_get(jv_copy(root), jv_string("bg"));
		bg_kind = jv_get_kind(bg);

		if (bg_kind == JV_KIND_OBJECT) {
			/////////////type
			{
				jv                  type;
				jv_kind        type_kind;
				const char     *type_str;

				type = jv_object_get(jv_copy(bg), jv_string("type"));
				type_kind = jv_get_kind(type);

				if (type_kind == JV_KIND_STRING) {
					type_str = jv_string_value(type);
					if (!strcmp(type_str, "none")) {
						s->bg->type = HB_BACKGROUND_TYPE_NONE;
					} else if (!strcmp(type_str, "grass")) {
						s->bg->type = HB_BACKGROUND_TYPE_GRASS;
					} else if (!strcmp(type_str, "hockey")) {
						s->bg->type = HB_BACKGROUND_TYPE_NONE;
					} else {
						_err_unmatched_property_value("bg.type",
								"[none, grass, hockey]", type_str);
						goto err;
					}
				} else if (type_kind == JV_KIND_INVALID) {
					s->bg->type = HB_BACKGROUND_TYPE_NONE;
				} else {
					_err_unmatched_property_type("bg.type",
							JV_KIND_STRING, type_kind);
					goto err;
				}
			}

			/////////////width
			{
				jv                  width;
				jv_kind        width_kind;

				width = jv_object_get(jv_copy(bg), jv_string("width"));
				width_kind = jv_get_kind(width);

				if (width_kind == JV_KIND_NUMBER) {
					s->bg->width = jv_number_value(width);
				} else if (width_kind == JV_KIND_INVALID) {
					s->bg->width = 0.0f;
				} else {
					_err_unmatched_property_type("bg.width",
							JV_KIND_NUMBER, width_kind);
					goto err;
				}
			}

			/////////////height
			{
				jv                  height;
				jv_kind        height_kind;

				height = jv_object_get(jv_copy(bg), jv_string("height"));
				height_kind = jv_get_kind(height);

				if (height_kind == JV_KIND_NUMBER) {
					s->bg->height = jv_number_value(height);
				} else if (height_kind == JV_KIND_INVALID) {
					s->bg->height = 0.0f;
				} else {
					_err_unmatched_property_type("bg.height",
							JV_KIND_NUMBER, height_kind);
					goto err;
				}
			}
		} else if (bg_kind == JV_KIND_INVALID) {
			s->bg->type = HB_BACKGROUND_TYPE_NONE;
			s->bg->width = 0.0f;
			s->bg->height = 0.0f;
			s->bg->kick_off_radius = 0.0f;
			s->bg->corner_radius = 0.0f;
			s->bg->goal_line = 0.0f;
			s->bg->color = 0x718c5a;
		} else {
			_err_unmatched_property_type("bg",
					JV_KIND_OBJECT, bg_kind);
			goto err;
		}
	}

	jv_free(root);
	return s;

err:
	hb_stadium_free(s);
	jv_free(root);

	return NULL;
}

extern struct hb_stadium *
hb_stadium_from_file(const char *file)
{
	FILE                         *fp;
	long                   file_size;
	char                   *raw_data;
	struct hb_stadium             *s;

	fp = fopen(file, "r");
	file_size = _get_file_size(fp);
	raw_data = malloc(file_size + 1);

	fread(raw_data, 1, file_size, fp);
	raw_data[file_size] = '\0';
	s = hb_stadium_parse(raw_data);

	fclose(fp);
	free(raw_data);

	return s;
}

extern void
hb_stadium_free(struct hb_stadium *s)
{
	if (s->name)
		free(s->name);
	if (s->bg)
		free(s->bg);
	free(s);
}

static const char *
_hb_camera_follow_to_string(enum hb_camera_follow cf)
{
	switch (cf) {
	case HB_CAMERA_FOLLOW_BALL: return "ball";
	case HB_CAMERA_FOLLOW_PLAYER: return "player";
	default: return "unknown";
	}
}

static const char *
_hb_kick_off_reset_to_string(enum hb_kick_off_reset kor)
{
	switch (kor) {
		case HB_KICK_OFF_RESET_FULL: return "full";
		case HB_KICK_OFF_RESET_PARTIAL: return "partial";
		default: return "unknown";
	}
}

static const char *
_hb_bool_yes_no_to_string(bool b)
{
	if (b) return "yes";
	return "no";
}

static const char *
_hb_background_type_to_string(enum hb_background_type bt)
{
	switch (bt) {
		case HB_BACKGROUND_TYPE_NONE: return "none";
		case HB_BACKGROUND_TYPE_GRASS: return "grass";
		case HB_BACKGROUND_TYPE_HOCKEY: return "hockey";
		default: return "unknown";
	}
}

extern const char *
hb_stadium_print(struct hb_stadium *s)
{
	printf("Name: %s\n", s->name);
	printf("CameraWidth: %.2f\n", s->camera_width);
	printf("CameraHeight: %.2f\n", s->camera_height);
	printf("MaxViewWidth: %.2f\n", s->max_view_width);
	printf("CameraFollow: %s\n", _hb_camera_follow_to_string(s->camera_follow));
	printf("SpawnDistance: %.2f\n", s->spawn_distance);
	printf("CanBeStored: %s\n", _hb_bool_yes_no_to_string(s->can_be_stored));
	printf("KickOffReset: %s\n", _hb_kick_off_reset_to_string(s->kick_off_reset));
	printf("Background.Type: %s\n", _hb_background_type_to_string(s->bg->type));
	printf("Background.Width: %.2f\n", s->bg->width);
	printf("Background.Height: %.2f\n", s->bg->height);
}

int
main(void)
{
	struct hb_stadium *big;
	big = hb_stadium_from_file("stadiums/big.hbs");
	hb_stadium_print(big);
	return 0;
}

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

		if (name_kind != JV_KIND_STRING) {
			_err_unmatched_property_type("name", JV_KIND_STRING, name_kind);
			goto err;
		}

		s->name = strdup(jv_string_value(name));
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

		if (camera_width_kind != JV_KIND_INVALID &&
				camera_width_kind != JV_KIND_NUMBER) {
			_err_unmatched_property_type("cameraWidth", JV_KIND_NUMBER, camera_width_kind);
			goto err;
		}

		if (camera_height_kind != JV_KIND_INVALID &&
				camera_height_kind != JV_KIND_NUMBER) {
			_err_unmatched_property_type("cameraHeight", JV_KIND_NUMBER, camera_height_kind);
			goto err;
		}

		if (camera_width_kind != JV_KIND_NUMBER ||
				camera_height_kind != JV_KIND_NUMBER) {
			s->camera_width = s->camera_height = 0.0f;
		} else {
			s->camera_width = jv_number_value(camera_width);
			s->camera_height = jv_number_value(camera_height);
		}
	}

	/////////////maxViewWidth
	{
		jv             max_view_width;
		jv_kind   max_view_width_kind;

		max_view_width = jv_object_get(jv_copy(root), jv_string("maxViewWidth"));
		max_view_width_kind = jv_get_kind(max_view_width);

		if (max_view_width_kind != JV_KIND_INVALID &&
				max_view_width_kind != JV_KIND_NUMBER) {
			_err_unmatched_property_type("maxViewWidth", JV_KIND_NUMBER, max_view_width_kind);
			goto err;
		}

		if (max_view_width_kind != JV_KIND_NUMBER) {
			s->max_view_width = 0.0f;
		} else {
			s->max_view_width = jv_number_value(max_view_width);
		}
	}

	/////////////cameraFollow
	{
		jv                  camera_follow;
		jv_kind        camera_follow_kind;
		const char     *camera_follow_str;

		camera_follow = jv_object_get(jv_copy(root), jv_string("cameraFollow"));
		camera_follow_kind = jv_get_kind(camera_follow);

		if (camera_follow_kind != JV_KIND_INVALID &&
				camera_follow_kind != JV_KIND_STRING) {
			_err_unmatched_property_type("cameraFollow", JV_KIND_STRING, camera_follow_kind);
			goto err;
		}

		if (camera_follow_kind != JV_KIND_STRING) {
			s->camera_follow = HB_CAMERA_FOLLOW_BALL;
		} else {
			camera_follow_str = jv_string_value(camera_follow);
			if (!strcmp(camera_follow_str, "ball")) s->camera_follow = HB_CAMERA_FOLLOW_BALL;
			else if (!strcmp(camera_follow_str, "player")) s->camera_follow = HB_CAMERA_FOLLOW_PLAYER;
			else {
				_err_unmatched_property_value("cameraFollow", "[ball, player]", camera_follow_str);
				goto err;
			}
		}
	}

	/////////////spawnDistance
	{
		jv                  spawn_distance;
		jv_kind        spawn_distance_kind;

		spawn_distance = jv_object_get(jv_copy(root), jv_string("spawnDistance"));
		spawn_distance_kind = jv_get_kind(spawn_distance);

		if (spawn_distance_kind != JV_KIND_INVALID &&
				spawn_distance_kind != JV_KIND_NUMBER) {
			_err_unmatched_property_type("spawnDistance", JV_KIND_NUMBER, spawn_distance_kind);
			goto err;
		}

		if (spawn_distance_kind != JV_KIND_NUMBER) {
			s->spawn_distance = 0.0f;
		} else {
			s->spawn_distance = jv_number_value(spawn_distance);
		}
	}

	/////////////canBeStored
	{
		jv                  can_be_stored;
		jv_kind        can_be_stored_kind;

		can_be_stored = jv_object_get(jv_copy(root), jv_string("canBeStored"));
		can_be_stored_kind = jv_get_kind(can_be_stored);

		if (can_be_stored_kind != JV_KIND_INVALID &&
				can_be_stored_kind != JV_KIND_TRUE &&
				can_be_stored_kind != JV_KIND_FALSE) {
			_err_unmatched_property_value("canBeStored", "[true, false]", jv_kind_name(can_be_stored_kind));
			goto err;
		}

		s->can_be_stored = can_be_stored_kind != JV_KIND_FALSE;
	}

	/////////////kickOffReset
	{
		jv                  kick_off_reset;
		jv_kind        kick_off_reset_kind;
		const char     *kick_off_reset_str;

		kick_off_reset = jv_object_get(jv_copy(root), jv_string("kickOffReset"));
		kick_off_reset_kind = jv_get_kind(kick_off_reset);

		if (kick_off_reset_kind != JV_KIND_INVALID &&
				kick_off_reset_kind != JV_KIND_STRING) {
			_err_unmatched_property_type("kickOffReset", JV_KIND_STRING, kick_off_reset_kind);
			goto err;
		}

		if (kick_off_reset_kind != JV_KIND_STRING) {
			s->kick_off_reset = HB_KICK_OFF_RESET_PARTIAL;
		} else {
			kick_off_reset_str = jv_string_value(kick_off_reset);
			if (!strcmp(kick_off_reset_str, "partial")) s->kick_off_reset = HB_KICK_OFF_RESET_PARTIAL;
			else if (!strcmp(kick_off_reset_str, "full")) s->kick_off_reset = HB_KICK_OFF_RESET_FULL;
			else {
				_err_unmatched_property_value("kickOffReset", "[partial, full]", kick_off_reset_str);
				goto err;
			}
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
	free(s);
}

int
main(void)
{
	struct hb_stadium *big;

	big = hb_stadium_from_file("stadiums/big.hbs");

	printf("Name: %s\n", big->name);
	printf("CameraWidth: %f\n", big->camera_width);
	printf("CameraHeight: %f\n", big->camera_height);
	printf("MaxViewWidth: %f\n", big->max_view_width);
	printf("CameraFollow: %s\n", big->camera_follow == HB_CAMERA_FOLLOW_BALL ? "ball" : "player");
	printf("SpawnDistance: %f\n", big->spawn_distance);
	printf("CanBeStored: %s\n", big->can_be_stored ? "yes" : "no");
	printf("KickOffReset: %s\n", big->kick_off_reset == HB_KICK_OFF_RESET_FULL ? "full" : "partial");

	return 0;
}

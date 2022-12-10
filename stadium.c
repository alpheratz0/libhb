#include <hb/stadium.h>
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
	FILE          *fp;
	long    file_size;
	char    *raw_data;

	fp = fopen(file, "r");
	file_size = _get_file_size(fp);
	raw_data = malloc(file_size + 1);

	fread(raw_data, 1, file_size, fp);
	raw_data[file_size] = '\0';

	fclose(fp);

	return hb_stadium_parse(raw_data);
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

	return 0;
}

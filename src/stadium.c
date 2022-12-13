#include <hb/stadium.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <jv.h>

static const float      HB_F_ZERO         = 0.0f;
static const bool       HB_B_TRUE         = true;

static int _hb_jv_parse_string(jv from, char **to, const char *fallback);
static int _hb_jv_parse_number(jv from, float *to, const float *fallback);
static int _hb_jv_parse_boolean(jv from, bool *to, const bool *fallback);
static int _hb_jv_parse_camera_follow(jv from, enum hb_camera_follow *to, enum hb_camera_follow *fallback);
static int _hb_jv_parse_kick_off_reset(jv from, enum hb_kick_off_reset *to, enum hb_kick_off_reset *fallback);
static int _hb_jv_parse_bg_type(jv from, enum hb_background_type *to, enum hb_background_type *fallback);
static int _hb_jv_parse_color(jv from, uint32_t *to, uint32_t *fallback);
static int _hb_jv_parse_bg(jv from, struct hb_background *to);

static int _hb_jv_parse_string_and_free(jv from, char **to, const char *fallback);
static int _hb_jv_parse_number_and_free(jv from, float *to, const float *fallback);
static int _hb_jv_parse_boolean_and_free(jv from, bool *to, const bool *fallback);
static int _hb_jv_parse_camera_follow_and_free(jv from, enum hb_camera_follow *to, enum hb_camera_follow *fallback);
static int _hb_jv_parse_kick_off_reset_and_free(jv from, enum hb_kick_off_reset *to, enum hb_kick_off_reset *fallback);
static int _hb_jv_parse_bg_type_and_free(jv from, enum hb_background_type *to, enum hb_background_type *fallback);
static int _hb_jv_parse_color_and_free(jv from, uint32_t *to, uint32_t *fallback);
static int _hb_jv_parse_bg_and_free(jv from, struct hb_background *to);

//////////////////////////////////////////////
//////////////////PARSE///////////////////////
//////////////////////////////////////////////

static int
_hb_jv_parse_string(jv from, char **to, const char *fallback)
{
	switch (jv_get_kind(from)) {
	case JV_KIND_STRING:
		*to = strdup(jv_string_value(from));
		return 0;
	case JV_KIND_INVALID:
		if (fallback == NULL)
			return -1;
		*to = strdup(fallback);
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_number(jv from, float *to, const float *fallback)
{
	switch (jv_get_kind(from)) {
	case JV_KIND_NUMBER:
		*to = jv_number_value(from);
		return 0;
	case JV_KIND_INVALID:
		if (fallback == NULL)
			return -1;
		*to = *fallback;
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_boolean(jv from, bool *to, const bool *fallback)
{
	switch (jv_get_kind(from)) {
	case JV_KIND_FALSE:
		*to = false;
		return 0;
	case JV_KIND_TRUE:
		*to = true;
		return 0;
	case JV_KIND_INVALID:
		if (NULL == fallback)
			return -1;
		*to = *fallback;
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_camera_follow(jv from, enum hb_camera_follow *to,
		enum hb_camera_follow *fallback)
{
	const char *camera_follow_str;
	switch (jv_get_kind(from)) {
	case JV_KIND_STRING:
		camera_follow_str = jv_string_value(from);
		if (!strcmp(camera_follow_str, "ball")) *to = HB_CAMERA_FOLLOW_BALL;
		else if (!strcmp(camera_follow_str, "player")) *to = HB_CAMERA_FOLLOW_PLAYER;
		else return -1;
		return 0;
	case JV_KIND_INVALID:
		if (fallback == NULL)
			return -1;
		*to = *fallback;
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_kick_off_reset(jv from, enum hb_kick_off_reset *to,
		enum hb_kick_off_reset *fallback)
{
	const char *kick_off_reset_str;
	switch (jv_get_kind(from)) {
	case JV_KIND_STRING:
		kick_off_reset_str = jv_string_value(from);
		if (!strcmp(kick_off_reset_str, "partial")) *to = HB_KICK_OFF_RESET_PARTIAL;
		else if (!strcmp(kick_off_reset_str, "full")) *to = HB_KICK_OFF_RESET_FULL;
		else return -1;
		return 0;
	case JV_KIND_INVALID:
		if (fallback == NULL)
			return -1;
		*to = *fallback;
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_bg_type(jv from, enum hb_background_type *to,
		enum hb_background_type *fallback)
{
	const char *type_str;
	switch (jv_get_kind(from)) {
	case JV_KIND_STRING:
		type_str = jv_string_value(from);
		if (!strcmp(type_str, "grass")) *to = HB_BACKGROUND_TYPE_GRASS;
		else if (!strcmp(type_str, "hockey")) *to = HB_BACKGROUND_TYPE_HOCKEY;
		else *to = HB_BACKGROUND_TYPE_NONE;
		return 0;
	case JV_KIND_INVALID:
		if (fallback == NULL)
			return -1;
		*to = *fallback;
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_color(jv from, uint32_t *to, uint32_t *fallback)
{
	jv_kind kind;
	const char *color_str;
	char *color_str_end;

	kind = jv_get_kind(from);

	if (kind == JV_KIND_INVALID) {
		if (NULL == fallback)
			return -1;
		*to = *fallback;
		return 0;
	}

	if (kind == JV_KIND_STRING) {
		color_str = jv_string_value(from);
		if (!strcmp(color_str, "transparent")) *to = 0x00000000;
		else {
			*to = strtol(color_str, &color_str_end, 16);
			if (color_str_end - color_str > 8 ||
					color_str_end[0] != '\0')
				return -1;
		}
		return 0;
	}

	if (kind == JV_KIND_ARRAY) {
		if (jv_array_length(jv_copy(from)) != 3)
			return -1;
		*to = 0xff << 24;
		jv_array_foreach(from, index, value) {
			if (jv_get_kind(value) != JV_KIND_NUMBER)
				return -1;
			*to |= ((int)(jv_number_value(value)) & 0xff) << (8*(2-index));
			jv_free(value);
		}
		return 0;
	}

	return -1;
}

static int
_hb_jv_parse_bg(jv from, struct hb_background *to)
{
	jv_kind kind;

	kind = jv_get_kind(from);

	if (kind == JV_KIND_INVALID) {
		to->type = HB_BACKGROUND_TYPE_NONE;
		to->width = 0.0f;
		to->height = 0.0f;
		to->kick_off_radius = 0.0f;
		to->corner_radius = 0.0f;
		to->goal_line = 0.0f;
		to->color = 0xff718c5a;
		return 0;
	}

	if (kind != JV_KIND_OBJECT)
		return -1;

	/////////////type
	{
		jv type;
		enum hb_background_type fallback_type;
		fallback_type = HB_BACKGROUND_TYPE_NONE;
		type = jv_object_get(jv_copy(from), jv_string("type"));
		if (_hb_jv_parse_bg_type_and_free(type, &to->type, &fallback_type) < 0)
			return -1;
	}

	/////////////width
	{
		jv width;
		width = jv_object_get(jv_copy(from), jv_string("width"));
		if (_hb_jv_parse_number_and_free(width, &to->width, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////height
	{
		jv height;
		height = jv_object_get(jv_copy(from), jv_string("height"));
		if (_hb_jv_parse_number_and_free(height, &to->height, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////kickOffRadius
	{
		jv kick_off_radius;
		kick_off_radius = jv_object_get(jv_copy(from), jv_string("kickOffRadius"));
		if (_hb_jv_parse_number_and_free(kick_off_radius, &to->kick_off_radius, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////cornerRadius
	{
		jv corner_radius;
		corner_radius = jv_object_get(jv_copy(from), jv_string("cornerRadius"));
		if (_hb_jv_parse_number_and_free(corner_radius, &to->corner_radius, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////goalLine
	{
		jv goal_line;
		goal_line = jv_object_get(jv_copy(from), jv_string("goalLine"));
		if (_hb_jv_parse_number_and_free(goal_line, &to->goal_line, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////color
	{
		jv color;
		uint32_t fallback_color;
		fallback_color = 0xff718c5a;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (_hb_jv_parse_color_and_free(color, &to->color, &fallback_color) < 0)
			return -1;
	}

	return 0;
}

//////////////////////////////////////////////
//////////////PARSE + JV_FREE/////////////////
//////////////////////////////////////////////

static int
_hb_jv_parse_string_and_free(jv from, char **to, const char *fallback)
{
	int ret;
	ret = _hb_jv_parse_string(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_number_and_free(jv from, float *to, const float *fallback)
{
	int ret;
	ret = _hb_jv_parse_number(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_boolean_and_free(jv from, bool *to, const bool *fallback)
{
	int ret;
	ret = _hb_jv_parse_boolean(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_camera_follow_and_free(jv from, enum hb_camera_follow *to,
		enum hb_camera_follow *fallback)
{
	int ret;
	ret = _hb_jv_parse_camera_follow(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_kick_off_reset_and_free(jv from, enum hb_kick_off_reset *to,
		enum hb_kick_off_reset *fallback)
{
	int ret;
	ret = _hb_jv_parse_kick_off_reset(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_bg_type_and_free(jv from, enum hb_background_type *to,
		enum hb_background_type *fallback)
{
	int ret;
	ret = _hb_jv_parse_bg_type(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_color_and_free(jv from, uint32_t *to, uint32_t *fallback)
{
	int ret;
	ret = _hb_jv_parse_color(from, to, fallback);
	jv_free(from);
	return ret;
}

static int
_hb_jv_parse_bg_and_free(jv from, struct hb_background *to)
{
	int ret;
	ret = _hb_jv_parse_bg(from, to);
	jv_free(from);
	return ret;
}

extern struct hb_stadium *
hb_stadium_parse(const char *in)
{
	struct hb_stadium *s;
	jv root;

	/////////////setup
	s = calloc(1, sizeof(struct hb_stadium));
	root = jv_parse(in);

	if (jv_get_kind(root) != JV_KIND_OBJECT)
		goto err;

	/////////////name
	{
		jv name;
		name = jv_object_get(jv_copy(root), jv_string("name"));
		if (_hb_jv_parse_string_and_free(name, &s->name, NULL) < 0)
			goto err;
	}

	/////////////cameraWidth
	{
		jv cam_width;
		cam_width = jv_object_get(jv_copy(root), jv_string("cameraWidth"));
		if (_hb_jv_parse_number_and_free(cam_width, &s->camera_width, &HB_F_ZERO) < 0)
			goto err;
	}

	/////////////cameraHeight
	{
		jv cam_height;
		cam_height = jv_object_get(jv_copy(root), jv_string("cameraHeight"));
		if (_hb_jv_parse_number_and_free(cam_height, &s->camera_height, &HB_F_ZERO) < 0)
			goto err;
	}

	/////////////maxViewWidth
	{
		jv max_view_width;
		max_view_width = jv_object_get(jv_copy(root), jv_string("maxViewWidth"));
		if (_hb_jv_parse_number_and_free(max_view_width, &s->max_view_width, &HB_F_ZERO) < 0)
			goto err;
	}

	/////////////cameraFollow
	{
		jv camera_follow;
		enum hb_camera_follow fallback_camera_follow;
		fallback_camera_follow = HB_CAMERA_FOLLOW_BALL;
		camera_follow = jv_object_get(jv_copy(root), jv_string("cameraFollow"));
		if (_hb_jv_parse_camera_follow_and_free(camera_follow,
					&s->camera_follow, &fallback_camera_follow) < 0)
			goto err;
	}

	/////////////spawnDistance
	{
		jv spawn_distance;
		spawn_distance = jv_object_get(jv_copy(root), jv_string("spawnDistance"));
		if (_hb_jv_parse_number_and_free(spawn_distance, &s->spawn_distance, &HB_F_ZERO) < 0)
			goto err;
	}

	/////////////canBeStored
	{
		jv can_be_stored;
		can_be_stored = jv_object_get(jv_copy(root), jv_string("canBeStored"));
		if (_hb_jv_parse_boolean_and_free(can_be_stored, &s->can_be_stored, &HB_B_TRUE) < 0)
			goto err;
	}

	/////////////kickOffReset
	{
		jv kick_off_reset;
		enum hb_kick_off_reset fallback_kick_off_reset;
		fallback_kick_off_reset = HB_KICK_OFF_RESET_PARTIAL;
		kick_off_reset = jv_object_get(jv_copy(root), jv_string("kickOffReset"));
		if (_hb_jv_parse_kick_off_reset_and_free(kick_off_reset,
					&s->kick_off_reset, &fallback_kick_off_reset) < 0)
			goto err;
	}

	/////////////bg
	{
		jv bg;
		bg = jv_object_get(jv_copy(root), jv_string("bg"));
		s->bg = malloc(sizeof(struct hb_background));
		if (_hb_jv_parse_bg_and_free(bg, s->bg) < 0)
			goto err;
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
	FILE *fp;
	long file_size;
	char *raw_data;
	struct hb_stadium *s;

	if (NULL == (fp = fopen(file, "r")))
		return NULL;

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
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
	struct hb_trait **trait;
	struct hb_vertex **vertex;
	struct hb_segment **segment;
	struct hb_goal **goal;
	struct hb_disc **disc;
	struct hb_plane **plane;
	struct hb_joint **joint;
	struct hb_point **point;

	if (s->name)
		free(s->name);
	if (s->bg)
		free(s->bg);
	if (s->traits) {
		trait = s->traits;
		while (*trait) {
			free((*trait)->name);
			free(*trait);
			trait++;
		}
		free(s->traits);
	}
	if (s->vertexes) {
		vertex = s->vertexes;
		while (*vertex) {
			free(*vertex);
			vertex++;
		}
		free(s->vertexes);
	}
	if (s->segments) {
		segment = s->segments;
		while (*segment) {
			free(*segment);
			segment++;
		}
		free(s->segments);
	}
	if (s->goals) {
		goal = s->goals;
		while (*goal) {
			free(*goal);
			goal++;
		}
		free(s->goals);
	}
	if (s->discs) {
		if (s->discs[0] != NULL) s->ball_physics = NULL;
		disc = s->discs;
		while (*disc) {
			free(*disc);
			disc++;
		}
		free(s->discs);
	}
	if (s->planes) {
		plane = s->planes;
		while (*plane) {
			free(*plane);
			plane++;
		}
		free(s->planes);
	}
	if (s->joints) {
		joint = s->joints;
		while (*joint) {
			free(*joint);
			joint++;
		}
		free(s->joints);
	}
	if (s->red_spawn_points) {
		point = s->red_spawn_points;
		while (*point) {
			free(*point);
			point++;
		}
		free(s->red_spawn_points);
	}
	if (s->blue_spawn_points) {
		point = s->blue_spawn_points;
		while (*point) {
			free(*point);
			point++;
		}
		free(s->blue_spawn_points);
	}

	free(s);
}

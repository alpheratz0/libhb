#include <hb/stadium.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <jv.h>

static const float      HB_F_ZERO         = 0.0f;
static const bool       HB_B_TRUE         = true;
static const float      HB_V2_ZERO[2]     = { 0.0f, 0.0f };

static int _hb_jv_parse_string(jv from, char **to, const char *fallback);
static int _hb_jv_parse_number(jv from, float *to, const float *fallback);
static int _hb_jv_parse_boolean(jv from, bool *to, const bool *fallback);
static int _hb_jv_parse_camera_follow(jv from, enum hb_camera_follow *to, const enum hb_camera_follow *fallback);
static int _hb_jv_parse_kick_off_reset(jv from, enum hb_kick_off_reset *to, const enum hb_kick_off_reset *fallback);
static int _hb_jv_parse_bg_type(jv from, enum hb_background_type *to, const enum hb_background_type *fallback);
static int _hb_jv_parse_color(jv from, uint32_t *to, const uint32_t *fallback);
static int _hb_jv_parse_bg(jv from, struct hb_background **to);
static int _hb_jv_parse_collision_flag(jv from, enum hb_collision_flags *to, const enum hb_collision_flags *fallback);
static int _hb_jv_parse_collision_flags(jv from, enum hb_collision_flags *to, const enum hb_collision_flags *fallback);
static int _hb_jv_parse_trait(jv from, jv name, struct hb_trait **to);
static int _hb_jv_parse_trait_list(jv from, struct hb_trait ***to);
static int _hb_jv_find_trait_by_name(jv from, struct hb_trait **to, struct hb_trait **traits);
static int _hb_jv_parse_vertex(jv from, struct hb_vertex **to, struct hb_trait **traits);
static int _hb_jv_parse_vertex_list(jv from, struct hb_vertex ***to, struct hb_trait **traits);
static int _hb_jv_parse_segment(jv from, struct hb_segment **to, struct hb_vertex **vertexes, struct hb_trait **traits);
static int _hb_jv_parse_segment_list(jv from, struct hb_segment ***to, struct hb_vertex **vertexes, struct hb_trait **traits);
static int _hb_jv_parse_vec2(jv from, float to[2], const float fallback[2]);
static int _hb_jv_parse_team(jv from, enum hb_team *to, const enum hb_team *fallback);
static int _hb_jv_parse_goal(jv from, struct hb_goal **to);
static int _hb_jv_parse_goal_list(jv from, struct hb_goal ***to);
static int _hb_jv_parse_ball_physics(jv from, struct hb_disc **to);
static int _hb_jv_parse_disc(jv from, struct hb_disc **to, struct hb_trait **traits);
static int _hb_jv_parse_disc_list(jv from, struct hb_disc ***to, struct hb_trait **traits, struct hb_disc **ball_physics);
static int _hb_jv_parse_plane(jv from, struct hb_plane **to, struct hb_trait **traits);
static int _hb_jv_parse_plane_list(jv from, struct hb_plane ***to, struct hb_trait **traits);
static int _hb_jv_parse_joint_length(jv from, struct hb_joint_length *to);
static int _hb_jv_parse_joint_strength(jv from, struct hb_joint_strength *to);
static int _hb_jv_parse_joint(jv from, struct hb_joint **to, struct hb_disc **discs);
static int _hb_jv_parse_joint_list(jv from, struct hb_joint ***to, struct hb_disc **discs);
static int _hb_jv_parse_point(jv from, struct hb_point **to);
static int _hb_jv_parse_point_list(jv from, struct hb_point ***to);
static int _hb_jv_parse_player_physics(jv from, struct hb_player_physics **to);

static int _hb_jv_parse_string_and_free(jv from, char **to, const char *fallback);
static int _hb_jv_parse_number_and_free(jv from, float *to, const float *fallback);
static int _hb_jv_parse_boolean_and_free(jv from, bool *to, const bool *fallback);
static int _hb_jv_parse_camera_follow_and_free(jv from, enum hb_camera_follow *to, const enum hb_camera_follow *fallback);
static int _hb_jv_parse_kick_off_reset_and_free(jv from, enum hb_kick_off_reset *to, const enum hb_kick_off_reset *fallback);
static int _hb_jv_parse_bg_type_and_free(jv from, enum hb_background_type *to, const enum hb_background_type *fallback);
static int _hb_jv_parse_color_and_free(jv from, uint32_t *to, const uint32_t *fallback);
static int _hb_jv_parse_bg_and_free(jv from, struct hb_background **to);
static int _hb_jv_parse_collision_flag_and_free(jv from, enum hb_collision_flags *to, const enum hb_collision_flags *fallback);
static int _hb_jv_parse_collision_flags_and_free(jv from, enum hb_collision_flags *to, const enum hb_collision_flags *fallback);
static int _hb_jv_parse_trait_and_free(jv from, jv name, struct hb_trait **to);
static int _hb_jv_parse_trait_list_and_free(jv from, struct hb_trait ***to);
static int _hb_jv_find_trait_by_name_and_free(jv from, struct hb_trait **to, struct hb_trait **traits);
static int _hb_jv_parse_vertex_and_free(jv from, struct hb_vertex **to, struct hb_trait **traits);
static int _hb_jv_parse_vertex_list_and_free(jv from, struct hb_vertex ***to, struct hb_trait **traits);
static int _hb_jv_parse_segment_and_free(jv from, struct hb_segment **to, struct hb_vertex **vertexes, struct hb_trait **traits);
static int _hb_jv_parse_segment_list_and_free(jv from, struct hb_segment ***to, struct hb_vertex **vertexes, struct hb_trait **traits);
static int _hb_jv_parse_vec2_and_free(jv from, float to[2], const float fallback[2]);
static int _hb_jv_parse_team_and_free(jv from, enum hb_team *to, const enum hb_team *fallback);
static int _hb_jv_parse_goal_and_free(jv from, struct hb_goal **to);
static int _hb_jv_parse_goal_list_and_free(jv from, struct hb_goal ***to);
static int _hb_jv_parse_ball_physics_and_free(jv from, struct hb_disc **to);
static int _hb_jv_parse_disc_and_free(jv from, struct hb_disc **to, struct hb_trait **traits);
static int _hb_jv_parse_disc_list_and_free(jv from, struct hb_disc ***to, struct hb_trait **traits, struct hb_disc **ball_physics);
static int _hb_jv_parse_plane_and_free(jv from, struct hb_plane **to, struct hb_trait **traits);
static int _hb_jv_parse_plane_list_and_free(jv from, struct hb_plane ***to, struct hb_trait **traits);
static int _hb_jv_parse_joint_length_and_free(jv from, struct hb_joint_length *to);
static int _hb_jv_parse_joint_strength_and_free(jv from, struct hb_joint_strength *to);
static int _hb_jv_parse_joint_and_free(jv from, struct hb_joint **to, struct hb_disc **discs);
static int _hb_jv_parse_joint_list_and_free(jv from, struct hb_joint ***to, struct hb_disc **discs);
static int _hb_jv_parse_point_and_free(jv from, struct hb_point **to);
static int _hb_jv_parse_point_list_and_free(jv from, struct hb_point ***to);
static int _hb_jv_parse_player_physics_and_free(jv from, struct hb_player_physics **to);

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
		const enum hb_camera_follow *fallback)
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
		const enum hb_kick_off_reset *fallback)
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
		const enum hb_background_type *fallback)
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
_hb_jv_parse_color(jv from, uint32_t *to, const uint32_t *fallback)
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
			*to |= 0xff << 24;
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
_hb_jv_parse_bg(jv from, struct hb_background **to)
{
	struct hb_background *bg;
	jv_kind kind;

	kind = jv_get_kind(from);
	bg = *to = malloc(sizeof(struct hb_background));

	if (kind == JV_KIND_INVALID) {
		bg->type = HB_BACKGROUND_TYPE_NONE;
		bg->width = 0.0f;
		bg->height = 0.0f;
		bg->kick_off_radius = 0.0f;
		bg->corner_radius = 0.0f;
		bg->goal_line = 0.0f;
		bg->color = 0xff718c5a;
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
		if (_hb_jv_parse_bg_type_and_free(type, &bg->type, &fallback_type) < 0)
			return -1;
	}

	/////////////width
	{
		jv width;
		width = jv_object_get(jv_copy(from), jv_string("width"));
		if (_hb_jv_parse_number_and_free(width, &bg->width, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////height
	{
		jv height;
		height = jv_object_get(jv_copy(from), jv_string("height"));
		if (_hb_jv_parse_number_and_free(height, &bg->height, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////kickOffRadius
	{
		jv kick_off_radius;
		kick_off_radius = jv_object_get(jv_copy(from), jv_string("kickOffRadius"));
		if (_hb_jv_parse_number_and_free(kick_off_radius, &bg->kick_off_radius, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////cornerRadius
	{
		jv corner_radius;
		corner_radius = jv_object_get(jv_copy(from), jv_string("cornerRadius"));
		if (_hb_jv_parse_number_and_free(corner_radius, &bg->corner_radius, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////goalLine
	{
		jv goal_line;
		goal_line = jv_object_get(jv_copy(from), jv_string("goalLine"));
		if (_hb_jv_parse_number_and_free(goal_line, &bg->goal_line, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////color
	{
		jv color;
		uint32_t fallback_color;
		fallback_color = 0xff718c5a;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (_hb_jv_parse_color_and_free(color, &bg->color, &fallback_color) < 0)
			return -1;
	}

	return 0;
}

static int
_hb_jv_parse_collision_flag(jv from, enum hb_collision_flags *to,
		const enum hb_collision_flags *fallback)
{
	const char *str;

	if (jv_get_kind(from) == JV_KIND_INVALID) {
		if (NULL == fallback)
			return -1;
		*to = *fallback;
		return 0;
	}

	if (jv_get_kind(from) != JV_KIND_STRING)
		return -1;

	str = jv_string_value(from);
	if (!strcmp(str, "ball")) *to = HB_COLLISION_BALL;
	else if (!strcmp(str, "red")) *to = HB_COLLISION_RED;
	else if (!strcmp(str, "blue")) *to = HB_COLLISION_BLUE;
	else if (!strcmp(str, "redKO")) *to = HB_COLLISION_RED_KO;
	else if (!strcmp(str, "blueKO")) *to = HB_COLLISION_BLUE_KO;
	else if (!strcmp(str, "wall")) *to = HB_COLLISION_WALL;
	else if (!strcmp(str, "all")) *to = HB_COLLISION_ALL;
	else if (!strcmp(str, "kick")) *to = HB_COLLISION_KICK;
	else if (!strcmp(str, "score")) *to = HB_COLLISION_SCORE;
	else if (!strcmp(str, "c0")) *to = HB_COLLISION_C0;
	else if (!strcmp(str, "c1")) *to = HB_COLLISION_C1;
	else if (!strcmp(str, "c2")) *to = HB_COLLISION_C2;
	else if (!strcmp(str, "c3")) *to = HB_COLLISION_C3;
	else *to = 0;
	return 0;
}

static int
_hb_jv_parse_collision_flags(jv from, enum hb_collision_flags *to,
		const enum hb_collision_flags *fallback)
{
	enum hb_collision_flags flag;
	switch (jv_get_kind(from)) {
	case JV_KIND_ARRAY:
		jv_array_foreach(from, index, value) {
			if (_hb_jv_parse_collision_flag_and_free(value, &flag, NULL) < 0)
				return -1;
			*to |= flag;
		}
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
_hb_jv_parse_trait(jv from, jv name, struct hb_trait **to)
{
	struct hb_trait *trait;

	if (jv_get_kind(name) != JV_KIND_STRING ||
			jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	trait = *to = calloc(1, sizeof(struct hb_trait));
	trait->name = strdup(jv_string_value(name));

	/////////////curve
	{
		jv curve;
		curve = jv_object_get(jv_copy(from), jv_string("curve"));
		if (jv_get_kind(curve) != JV_KIND_INVALID) {
			if (_hb_jv_parse_number_and_free(curve, &trait->curve, NULL) < 0)
				return -1;
			trait->has_curve = true;
		}
	}

	/////////////damping
	{
		jv damping;
		damping = jv_object_get(jv_copy(from), jv_string("damping"));
		if (jv_get_kind(damping) != JV_KIND_INVALID) {
			if (_hb_jv_parse_number_and_free(damping, &trait->damping, NULL) < 0)
				return -1;
			trait->has_damping = true;
		}
	}

	/////////////invMass
	{
		jv inv_mass;
		inv_mass = jv_object_get(jv_copy(from), jv_string("invMass"));
		if (jv_get_kind(inv_mass) != JV_KIND_INVALID) {
			if (_hb_jv_parse_number_and_free(inv_mass, &trait->inv_mass, NULL) < 0)
				return -1;
			trait->has_inv_mass = true;
		}
	}

	/////////////radius
	{
		jv radius;
		radius = jv_object_get(jv_copy(from), jv_string("radius"));
		if (jv_get_kind(radius) != JV_KIND_INVALID) {
			if (_hb_jv_parse_number_and_free(radius, &trait->radius, NULL) < 0)
				return -1;
			trait->has_radius = true;
		}
	}

	/////////////bCoef
	{
		jv b_coef;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (jv_get_kind(b_coef) != JV_KIND_INVALID) {
			if (_hb_jv_parse_number_and_free(b_coef, &trait->b_coef, NULL) < 0)
				return -1;
			trait->has_b_coef = true;
		}
	}

	/////////////color
	{
		jv color;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (jv_get_kind(color) != JV_KIND_INVALID) {
			if (_hb_jv_parse_color_and_free(color, &trait->color, NULL) < 0)
				return -1;
			trait->has_color = true;
		}
	}

	/////////////vis
	{
		jv vis;
		vis = jv_object_get(jv_copy(from), jv_string("vis"));
		if (jv_get_kind(vis) != JV_KIND_INVALID) {
			if (_hb_jv_parse_boolean_and_free(vis, &trait->vis, NULL) < 0)
				return -1;
			trait->has_vis = true;
		}
	}

	/////////////cGroup
	{
		jv c_group;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (jv_get_kind(c_group) != JV_KIND_INVALID) {
			if (_hb_jv_parse_collision_flags_and_free(c_group, &trait->c_group, NULL) < 0)
				return -1;
			trait->has_c_group = true;
		}
	}

	/////////////cMask
	{
		jv c_mask;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (jv_get_kind(c_mask) != JV_KIND_INVALID) {
			if (_hb_jv_parse_collision_flags_and_free(c_mask, &trait->c_mask, NULL) < 0)
				return -1;
			trait->has_c_mask = true;
		}
	}

	return 0;
}

static int
_hb_jv_parse_trait_list(jv from, struct hb_trait ***to)
{
	int count, index;

	switch (jv_get_kind(from)) {
	case JV_KIND_OBJECT:
		index = 0;
		count = jv_object_length(jv_copy(from));
		*to = calloc(count + 1, sizeof(struct hb_trait *));
		jv_object_foreach(from, name, trait) {
			if (_hb_jv_parse_trait_and_free(trait, name, &((*to)[index++])) < 0)
				return -1;
		}
		return 0;
	case JV_KIND_INVALID:
		*to = calloc(1, sizeof(struct hb_trait *));
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_find_trait_by_name(jv from, struct hb_trait **to, struct hb_trait **traits)
{
	const char *str;
	switch (jv_get_kind(from)) {
	case JV_KIND_STRING:
		str = jv_string_value(from);
		for (; *traits; ++traits) {
			if (!strcmp((*traits)->name, str)) {
				*to = *traits;
				return 0;
			}
		} /* FALLTHROUGH */
	case JV_KIND_INVALID:
		*to = NULL;
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_vertex(jv from, struct hb_vertex **to,
		struct hb_trait **traits)
{
	struct hb_vertex *vert;
	struct hb_trait *vert_trait;

	if (jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	vert = *to = calloc(1, sizeof(struct hb_vertex));

	/////////////x
	{
		jv x;
		x = jv_object_get(jv_copy(from), jv_string("x"));
		if (_hb_jv_parse_number_and_free(x, &vert->x, NULL) < 0)
			return -1;
	}

	/////////////y
	{
		jv y;
		y = jv_object_get(jv_copy(from), jv_string("y"));
		if (_hb_jv_parse_number_and_free(y, &vert->y, NULL) < 0)
			return -1;
	}

	/////////////trait
	{
		jv trait;
		trait = jv_object_get(jv_copy(from), jv_string("trait"));
		if (_hb_jv_find_trait_by_name_and_free(trait, &vert_trait, traits) < 0)
			return -1;
	}

	/////////////bCoef
	{
		jv b_coef;
		float fallback_b_coef;
		fallback_b_coef = 1.0f;
		if (NULL != vert_trait && vert_trait->has_b_coef)
			fallback_b_coef = vert_trait->b_coef;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (_hb_jv_parse_number_and_free(b_coef, &vert->b_coef, &fallback_b_coef) < 0)
			return -1;
	}

	/////////////cGroup
	{
		jv c_group;
		enum hb_collision_flags fallback_c_group;
		fallback_c_group = HB_COLLISION_WALL;
		if (NULL != vert_trait && vert_trait->has_c_group)
			fallback_c_group = vert_trait->c_group;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (_hb_jv_parse_collision_flags_and_free(c_group, &vert->c_group, &fallback_c_group) < 0)
			return -1;
	}

	/////////////cMask
	{
		jv c_mask;
		enum hb_collision_flags fallback_c_mask;
		fallback_c_mask = HB_COLLISION_ALL;
		if (NULL != vert_trait && vert_trait->has_c_mask)
			fallback_c_mask = vert_trait->c_mask;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (_hb_jv_parse_collision_flags_and_free(c_mask, &vert->c_mask, &fallback_c_mask) < 0)
			return -1;
	}

	return 0;
}

static int
_hb_jv_parse_vertex_list(jv from, struct hb_vertex ***to,
		struct hb_trait **traits)
{
	int count;

	switch (jv_get_kind(from)) {
	case JV_KIND_ARRAY:
		count = jv_array_length(jv_copy(from));
		*to = calloc(count + 1, sizeof(struct hb_vertex *));
		jv_array_foreach(from, index, vertex) {
			if (_hb_jv_parse_vertex_and_free(vertex, &((*to)[index]), traits) < 0)
				return -1;
		}
		return 0;
	case JV_KIND_INVALID:
		*to = calloc(1, sizeof(struct hb_vertex *));
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_segment(jv from, struct hb_segment **to,
		struct hb_vertex **vertexes, struct hb_trait **traits)
{
	struct hb_segment *segm;
	struct hb_trait *segm_trait;
	int vert_count;

	if (jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	vert_count = 0;
	while (*vertexes) {
		++vert_count;
		++vertexes;
	}

	segm = *to = calloc(1, sizeof(struct hb_segment));

	/////////////v0
	{
		jv v0;
		float f;
		v0 = jv_object_get(jv_copy(from), jv_string("v0"));
		if (_hb_jv_parse_number_and_free(v0, &f, NULL) < 0)
			return -1;
		segm->v0 = f;
		if (segm->v0 < 0 || segm->v0 >= vert_count)
			return -1;
	}

	/////////////v1
	{
		jv v1;
		float f;
		v1 = jv_object_get(jv_copy(from), jv_string("v1"));
		if (_hb_jv_parse_number_and_free(v1, &f, NULL) < 0)
			return -1;
		segm->v1 = f;
		if (segm->v1 < 0 || segm->v1 >= vert_count)
			return -1;
	}

	/////////////trait
	{
		jv trait;
		trait = jv_object_get(jv_copy(from), jv_string("trait"));
		if (_hb_jv_find_trait_by_name_and_free(trait, &segm_trait, traits) < 0)
			return -1;
	}

	/////////////bCoef
	{
		jv b_coef;
		float fallback_b_coef;
		fallback_b_coef = 1.0f;
		if (NULL != segm_trait && segm_trait->has_b_coef)
			fallback_b_coef = segm_trait->b_coef;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (_hb_jv_parse_number_and_free(b_coef, &segm->b_coef, &fallback_b_coef) < 0)
			return -1;
	}

	/////////////curve
	{
		jv curve;
		curve = jv_object_get(jv_copy(from), jv_string("curve"));
		if (_hb_jv_parse_number_and_free(curve, &segm->curve, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////bias
	{
		jv bias;
		bias = jv_object_get(jv_copy(from), jv_string("bias"));
		if (_hb_jv_parse_number_and_free(bias, &segm->bias, &HB_F_ZERO) < 0)
			return -1;
	}

	/////////////cGroup
	{
		jv c_group;
		enum hb_collision_flags fallback_c_group;
		fallback_c_group = HB_COLLISION_WALL;
		if (NULL != segm_trait && segm_trait->has_c_group)
			fallback_c_group = segm_trait->c_group;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (_hb_jv_parse_collision_flags_and_free(c_group, &segm->c_group, &fallback_c_group) < 0)
			return -1;
	}

	/////////////cMask
	{
		jv c_mask;
		enum hb_collision_flags fallback_c_mask;
		fallback_c_mask = HB_COLLISION_ALL;
		if (NULL != segm_trait && segm_trait->has_c_mask)
			fallback_c_mask = segm_trait->c_mask;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (_hb_jv_parse_collision_flags_and_free(c_mask, &segm->c_mask, &fallback_c_mask) < 0)
			return -1;
	}

	/////////////vis
	{
		jv vis;
		bool fallback_vis;
		fallback_vis = true;
		if (NULL != segm_trait && segm_trait->has_vis)
			fallback_vis = segm_trait->vis;
		vis = jv_object_get(jv_copy(from), jv_string("vis"));
		if (_hb_jv_parse_boolean_and_free(vis, &segm->vis, &fallback_vis) < 0)
			return -1;
	}

	/////////////color
	{
		jv color;
		uint32_t fallback_color;
		fallback_color = 0xff000000;
		if (NULL != segm_trait && segm_trait->has_color)
			fallback_color = segm_trait->color;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (_hb_jv_parse_color_and_free(color, &segm->color, &fallback_color) < 0)
			return -1;
	}

	return 0;
}

static int
_hb_jv_parse_segment_list(jv from, struct hb_segment ***to,
		struct hb_vertex **vertexes, struct hb_trait **traits)
{
	int count;

	switch (jv_get_kind(from)) {
	case JV_KIND_ARRAY:
		count = jv_array_length(jv_copy(from));
		*to = calloc(count + 1, sizeof(struct hb_segment *));
		jv_array_foreach(from, index, segment) {
			if (_hb_jv_parse_segment_and_free(segment, &((*to)[index]), vertexes, traits) < 0)
				return -1;
		}
		return 0;
	case JV_KIND_INVALID:
		*to = calloc(1, sizeof(struct hb_segment *));
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_vec2(jv from, float to[2], const float fallback[2])
{
	switch (jv_get_kind(from)) {
	case JV_KIND_ARRAY:
		if (jv_array_length(jv_copy(from)) != 2)
			return -1;
		jv_array_foreach(from, index, v) {
			if (_hb_jv_parse_number_and_free(v, &to[index], NULL) < 0)
				return -1;
		}
		return 0;
	case JV_KIND_INVALID:
		if (fallback == NULL)
			return -1;
		to[0] = fallback[0];
		to[1] = fallback[1];
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_team(jv from, enum hb_team *to, const enum hb_team *fallback)
{
	const char *str;
	switch (jv_get_kind(from)) {
	case JV_KIND_STRING:
		str = jv_string_value(from);
		if (!strcmp(str, "red")) *to = HB_TEAM_RED;
		else if (!strcmp(str, "blue")) *to = HB_TEAM_BLUE;
		else if (!strcmp(str, "spect")) *to = HB_TEAM_SPECTATOR;
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
_hb_jv_parse_goal(jv from, struct hb_goal **to)
{
	struct hb_goal *goal;

	if (jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	goal = *to = calloc(1, sizeof(struct hb_goal));

	/////////////p0
	{
		jv p0;
		p0 = jv_object_get(jv_copy(from), jv_string("p0"));
		if (_hb_jv_parse_vec2_and_free(p0, goal->p0, NULL) < 0)
			return -1;
	}

	/////////////p1
	{
		jv p1;
		p1 = jv_object_get(jv_copy(from), jv_string("p1"));
		if (_hb_jv_parse_vec2_and_free(p1, goal->p1, NULL) < 0)
			return -1;
	}

	/////////////team
	{
		jv team;
		team = jv_object_get(jv_copy(from), jv_string("team"));
		if (_hb_jv_parse_team_and_free(team, &goal->team, NULL) < 0 ||
				goal->team == HB_TEAM_SPECTATOR)
			return -1;
	}

	return 0;
}

static int
_hb_jv_parse_goal_list(jv from, struct hb_goal ***to)
{
	int count;

	switch (jv_get_kind(from)) {
	case JV_KIND_ARRAY:
		count = jv_array_length(jv_copy(from));
		*to = calloc(count + 1, sizeof(struct hb_goal *));
		jv_array_foreach(from, index, goal) {
			if (_hb_jv_parse_goal_and_free(goal, &((*to)[index])) < 0)
				return -1;
		}
		return 0;
	case JV_KIND_INVALID:
		*to = calloc(1, sizeof(struct hb_goal *));
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_ball_physics(jv from, struct hb_disc **to)
{
	const char *ball_physics_str;
	struct hb_disc *ball_physics;
	jv_kind kind;

	kind = jv_get_kind(from);

	if (kind != JV_KIND_OBJECT
			&& kind != JV_KIND_STRING
			&& kind != JV_KIND_INVALID)
		return -1;

	if (kind == JV_KIND_INVALID) {
		ball_physics = *to = calloc(1, sizeof(struct hb_disc));
		ball_physics->radius = 10.0f;
		ball_physics->b_coef = 0.5f;
		ball_physics->inv_mass = 1.0f;
		ball_physics->damping = 0.99f;
		ball_physics->color = 0xffffffff;
		ball_physics->c_mask = HB_COLLISION_ALL;
		ball_physics->c_group = HB_COLLISION_KICK | HB_COLLISION_SCORE | HB_COLLISION_BALL;
		ball_physics->speed[0] = ball_physics->speed[1] = 0.0f;
		return 0;
	}

	if (kind == JV_KIND_STRING) {
		ball_physics_str = jv_string_value(from);
		if (strcmp(ball_physics_str, "disc0")) return -1;
		*to = NULL;
		return 0;
	}

	ball_physics = *to = calloc(1, sizeof(struct hb_disc));

	/////////////pos
	{
		jv pos;
		pos = jv_object_get(jv_copy(from), jv_string("pos"));
		if (_hb_jv_parse_vec2_and_free(pos, ball_physics->pos, HB_V2_ZERO) < 0)
			return -1;
	}

	/////////////speed
	{
		jv speed;
		speed = jv_object_get(jv_copy(from), jv_string("speed"));
		if (_hb_jv_parse_vec2_and_free(speed, ball_physics->speed, HB_V2_ZERO) < 0)
			return -1;
	}

	/////////////gravity
	{
		jv gravity;
		gravity = jv_object_get(jv_copy(from), jv_string("gravity"));
		if (_hb_jv_parse_vec2_and_free(gravity, ball_physics->gravity, HB_V2_ZERO) < 0)
			return -1;
	}

	/////////////radius
	{
		jv radius;
		float fallback_radius;
		fallback_radius = 10.0f;
		radius = jv_object_get(jv_copy(from), jv_string("radius"));
		if (_hb_jv_parse_number_and_free(radius, &ball_physics->radius, &fallback_radius) < 0)
			return -1;
	}

	/////////////invMass
	{
		jv inv_mass;
		float fallback_inv_mass;
		fallback_inv_mass = 1.0f;
		inv_mass = jv_object_get(jv_copy(from), jv_string("invMass"));
		if (_hb_jv_parse_number_and_free(inv_mass, &ball_physics->inv_mass, &fallback_inv_mass) < 0)
			return -1;
	}

	/////////////damping
	{
		jv damping;
		float fallback_damping;
		fallback_damping = 0.99f;
		damping = jv_object_get(jv_copy(from), jv_string("damping"));
		if (_hb_jv_parse_number_and_free(damping, &ball_physics->damping, &fallback_damping) < 0)
			return -1;
	}

	/////////////color
	{
		jv color;
		uint32_t fallback_color;
		fallback_color = 0xffffffff;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (_hb_jv_parse_color_and_free(color, &ball_physics->color, &fallback_color) < 0)
			return -1;
	}

	/////////////bCoef
	{
		jv b_coef;
		float fallback_b_coef;
		fallback_b_coef = 0.5f;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (_hb_jv_parse_number_and_free(b_coef, &ball_physics->b_coef, &fallback_b_coef) < 0)
			return -1;
	}

	/////////////cMask
	{
		jv c_mask;
		enum hb_collision_flags fallback_c_mask;
		fallback_c_mask = HB_COLLISION_ALL;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (_hb_jv_parse_collision_flags_and_free(c_mask, &ball_physics->c_mask, &fallback_c_mask) < 0)
			return -1;
	}

	/////////////cGroup
	{
		jv c_group;
		enum hb_collision_flags fallback_c_group;
		fallback_c_group = HB_COLLISION_KICK | HB_COLLISION_SCORE | HB_COLLISION_BALL;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (_hb_jv_parse_collision_flags_and_free(c_group, &ball_physics->c_group, &fallback_c_group) < 0)
			return -1;
		ball_physics->c_group |= HB_COLLISION_KICK | HB_COLLISION_SCORE | HB_COLLISION_BALL;
	}

	return 0;
}

static int
_hb_jv_parse_disc(jv from, struct hb_disc **to,
		struct hb_trait **traits)
{
	struct hb_disc *disc;
	struct hb_trait *disc_trait;

	if (jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	disc = *to = calloc(1, sizeof(struct hb_disc));

	/////////////pos
	{
		jv pos;
		pos = jv_object_get(jv_copy(from), jv_string("pos"));
		if (_hb_jv_parse_vec2_and_free(pos, disc->pos, HB_V2_ZERO) < 0)
			return -1;
	}

	/////////////speed
	{
		jv speed;
		speed = jv_object_get(jv_copy(from), jv_string("speed"));
		if (_hb_jv_parse_vec2_and_free(speed, disc->speed, HB_V2_ZERO) < 0)
			return -1;
	}

	/////////////gravity
	{
		jv gravity;
		gravity = jv_object_get(jv_copy(from), jv_string("gravity"));
		if (_hb_jv_parse_vec2_and_free(gravity, disc->gravity, HB_V2_ZERO) < 0)
			return -1;
	}

	/////////////trait
	{
		jv trait;
		trait = jv_object_get(jv_copy(from), jv_string("trait"));
		if (_hb_jv_find_trait_by_name_and_free(trait, &disc_trait, traits) < 0)
			return -1;
	}

	/////////////radius
	{
		jv radius;
		float fallback_radius;
		fallback_radius = 10.0f;
		radius = jv_object_get(jv_copy(from), jv_string("radius"));
		if (disc_trait != NULL && disc_trait->has_radius)
			fallback_radius = disc_trait->radius;
		if (_hb_jv_parse_number_and_free(radius, &disc->radius, &fallback_radius) < 0)
			return -1;
	}

	/////////////invMass
	{
		jv inv_mass;
		float fallback_inv_mass;
		fallback_inv_mass = 1.0f;
		inv_mass = jv_object_get(jv_copy(from), jv_string("invMass"));
		if (disc_trait != NULL && disc_trait->has_inv_mass)
			fallback_inv_mass = disc_trait->inv_mass;
		if (_hb_jv_parse_number_and_free(inv_mass, &disc->inv_mass, &fallback_inv_mass) < 0)
			return -1;
	}

	/////////////damping
	{
		jv damping;
		float fallback_damping;
		fallback_damping =  0.99;
		damping = jv_object_get(jv_copy(from), jv_string("damping"));
		if (_hb_jv_parse_number_and_free(damping, &disc->damping, &fallback_damping) < 0)
			return -1;
	}

	/////////////color
	{
		jv color;
		uint32_t fallback_color;
		fallback_color = 0xffffffff;
		if (NULL != disc_trait && disc_trait->has_color)
			fallback_color = disc_trait->color;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (_hb_jv_parse_color_and_free(color, &disc->color, &fallback_color) < 0)
			return -1;
	}

	/////////////bCoef
	{
		jv b_coef;
		float fallback_b_coef;
		fallback_b_coef = 0.5f;
		if (NULL != disc_trait && disc_trait->has_b_coef)
			fallback_b_coef = disc_trait->b_coef;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (_hb_jv_parse_number_and_free(b_coef, &disc->b_coef, &fallback_b_coef) < 0)
			return -1;
	}

	/////////////cMask
	{
		jv c_mask;
		enum hb_collision_flags fallback_c_mask;
		fallback_c_mask = HB_COLLISION_ALL;
		if (NULL != disc_trait && disc_trait->has_c_mask)
			fallback_c_mask = disc_trait->c_mask;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (_hb_jv_parse_collision_flags_and_free(c_mask, &disc->c_mask, &fallback_c_mask) < 0)
			return -1;
	}

	/////////////cGroup
	{
		jv c_group;
		enum hb_collision_flags fallback_c_group;
		fallback_c_group = HB_COLLISION_ALL;
		if (NULL != disc_trait && disc_trait->has_c_group)
			fallback_c_group = disc_trait->c_group;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (_hb_jv_parse_collision_flags_and_free(c_group, &disc->c_group, &fallback_c_group) < 0)
			return -1;
	}

	return 0;
}

static int
_hb_jv_parse_disc_list(jv from, struct hb_disc ***to,
		struct hb_trait **traits, struct hb_disc **ball_physics)
{
	int count;

	switch (jv_get_kind(from)) {
	case JV_KIND_ARRAY:
		count = jv_array_length(jv_copy(from));
		if (*ball_physics == NULL && count == 0) return -1;
		if (*ball_physics != NULL) ++count;
		*to = calloc(count + 1, sizeof(struct hb_disc *));
		if (*ball_physics != NULL) (*to)[0] = *ball_physics;
		jv_array_foreach(from, index, disc) {
			if (*ball_physics == NULL && index == 0) {
				if (_hb_jv_parse_ball_physics_and_free(disc, &((*to)[0])) < 0)
					return -1;
			} else if (_hb_jv_parse_disc_and_free(disc,
						&((*to)[index + (*ball_physics != NULL)]), traits) < 0) {
				return -1;
			}
		}
		if (*ball_physics == NULL) *ball_physics = (*to)[0];
		return 0;
	case JV_KIND_INVALID:
		if (ball_physics == NULL)
			return -1;
		*to = calloc(2, sizeof(struct hb_disc *));
		(*to)[0] = *ball_physics;
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_plane(jv from, struct hb_plane **to, struct hb_trait **traits)
{
	struct hb_plane *plane;
	struct hb_trait *plane_trait;

	if (jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	plane = *to = calloc(1, sizeof(struct hb_plane));

	/////////////normal
	{
		jv normal;
		normal = jv_object_get(jv_copy(from), jv_string("normal"));
		if (_hb_jv_parse_vec2_and_free(normal, plane->normal, NULL) < 0)
			return -1;

	}

	/////////////dist
	{
		jv dist;
		dist = jv_object_get(jv_copy(from), jv_string("dist"));
		if (_hb_jv_parse_number_and_free(dist, &plane->dist, NULL) < 0)
			return -1;
	}

	/////////////trait
	{
		jv trait;
		trait = jv_object_get(jv_copy(from), jv_string("trait"));
		if (_hb_jv_find_trait_by_name_and_free(trait, &plane_trait, traits) < 0)
			return -1;
	}

	/////////////bCoef
	{
		jv b_coef;
		float fallback_b_coef;
		fallback_b_coef = 1.0f;
		if (NULL != plane_trait && plane_trait->has_b_coef)
			fallback_b_coef = plane_trait->b_coef;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (_hb_jv_parse_number_and_free(b_coef, &plane->b_coef, &fallback_b_coef) < 0)
			return -1;
	}

	/////////////cMask
	{
		jv c_mask;
		enum hb_collision_flags fallback_c_mask;
		fallback_c_mask = HB_COLLISION_ALL;
		if (NULL != plane_trait && plane_trait->has_c_mask)
			fallback_c_mask = plane_trait->c_mask;
		c_mask = jv_object_get(jv_copy(from), jv_string("cMask"));
		if (_hb_jv_parse_collision_flags_and_free(c_mask, &plane->c_mask, &fallback_c_mask) < 0)
			return -1;
	}

	/////////////cGroup
	{
		jv c_group;
		enum hb_collision_flags fallback_c_group;
		fallback_c_group = HB_COLLISION_WALL;
		if (NULL != plane_trait && plane_trait->has_c_group)
			fallback_c_group = plane_trait->c_group;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (_hb_jv_parse_collision_flags_and_free(c_group, &plane->c_group, &fallback_c_group) < 0)
			return -1;
	}

	return 0;
}

static int
_hb_jv_parse_plane_list(jv from, struct hb_plane ***to, struct hb_trait **traits)
{
	int count;

	switch (jv_get_kind(from)) {
	case JV_KIND_ARRAY:
		count = jv_array_length(jv_copy(from));
		*to = calloc(count + 1, sizeof(struct hb_plane *));
		jv_array_foreach(from, index, plane) {
			if (_hb_jv_parse_plane_and_free(plane, &((*to)[index]), traits) < 0)
				return -1;
		}
		return 0;
	case JV_KIND_INVALID:
		*to = calloc(1, sizeof(struct hb_plane *));
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_joint_length(jv from, struct hb_joint_length *to)
{
	switch (jv_get_kind(from)) {
	case JV_KIND_INVALID:
	case JV_KIND_NULL:
		to->kind = HB_JOINT_LENGTH_AUTO;
		return 0;
	case JV_KIND_NUMBER:
		to->kind = HB_JOINT_LENGTH_FIXED;
		if (_hb_jv_parse_number(from, &to->val.f, NULL) < 0)
			return -1;
		return 0;
	case JV_KIND_ARRAY:
		to->kind = HB_JOINT_LENGTH_RANGE;
		if (_hb_jv_parse_vec2(from, to->val.range, NULL) < 0)
			return -1;
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_joint_strength(jv from, struct hb_joint_strength *to)
{
	const char *str;
	switch (jv_get_kind(from)) {
	case JV_KIND_STRING:
		str = jv_string_value(from);
		if (strcmp(str, "rigid")) return -1;
		to->is_rigid = true;
		return 0;
	case JV_KIND_NUMBER:
		if (_hb_jv_parse_number(from, &to->val, NULL) < 0)
			return -1;
		to->is_rigid = false;
		return 0;
	case JV_KIND_INVALID:
		to->is_rigid = true;
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_joint(jv from, struct hb_joint **to, struct hb_disc **discs)
{
	struct hb_joint *joint;
	int disc_count;

	if (jv_get_kind(from) != JV_KIND_OBJECT)
		return -1;

	disc_count = 0;
	while (*discs) {
		++disc_count;
		++discs;
	}

	joint = *to = calloc(1, sizeof(struct hb_joint));

	/////////////d0
	{
		jv d0;
		float f;
		d0 = jv_object_get(jv_copy(from), jv_string("d0"));
		if (_hb_jv_parse_number_and_free(d0, &f, NULL) < 0)
			return -1;
		joint->d0 = f;
		if (joint->d0 < 0 || joint->d0 >= disc_count)
			return -1;
	}

	/////////////d1
	{
		jv d1;
		float f;
		d1 = jv_object_get(jv_copy(from), jv_string("d1"));
		if (_hb_jv_parse_number_and_free(d1, &f, NULL) < 0)
			return -1;
		joint->d1 = f;
		if (joint->d1 < 0 || joint->d1 >= disc_count)
			return -1;
	}

	/////////////length
	{
		jv length;
		length = jv_object_get(jv_copy(from), jv_string("length"));
		if (_hb_jv_parse_joint_length_and_free(length, &joint->length) < 0)
			return -1;
	}

	/////////////strength
	{
		jv strength;
		strength = jv_object_get(jv_copy(from), jv_string("strength"));
		if (_hb_jv_parse_joint_strength_and_free(strength, &joint->strength) < 0)
			return -1;
	}

	/////////////color
	{
		jv color;
		uint32_t fallback_color;
		fallback_color = 0xff000000;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (_hb_jv_parse_color_and_free(color, &joint->color, &fallback_color) < 0)
			return -1;
	}

	return 0;
}

static int
_hb_jv_parse_joint_list(jv from, struct hb_joint ***to, struct hb_disc **discs)
{
	int count;

	switch (jv_get_kind(from)) {
	case JV_KIND_ARRAY:
		count = jv_array_length(jv_copy(from));
		*to = calloc(count + 1, sizeof(struct hb_joint *));
		jv_array_foreach(from, index, joint) {
			if (_hb_jv_parse_joint_and_free(joint, &((*to)[index]), discs) < 0)
				return -1;
		}
		return 0;
	case JV_KIND_INVALID:
		*to = calloc(1, sizeof(struct hb_joint *));
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_point(jv from, struct hb_point **to)
{
	struct hb_point *point;
	float v2[2];
	if (_hb_jv_parse_vec2(from, v2, NULL) < 0)
		return -1;
	point = *to = malloc(sizeof(struct hb_point));
	point->x = v2[0];
	point->y = v2[1];
	return 0;
}

static int
_hb_jv_parse_point_list(jv from, struct hb_point ***to)
{
	int count;

	switch (jv_get_kind(from)) {
	case JV_KIND_ARRAY:
		count = jv_array_length(jv_copy(from));
		*to = calloc(count + 1, sizeof(struct hb_point *));
		jv_array_foreach(from, index, point) {
			if (_hb_jv_parse_point_and_free(point, &((*to)[index])) < 0)
				return -1;
		}
		return 0;
	case JV_KIND_INVALID:
		*to = calloc(1, sizeof(struct hb_point *));
		return 0;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_player_physics(jv from, struct hb_player_physics **to)
{
	struct hb_player_physics *player_physics;
	jv_kind kind;

	kind = jv_get_kind(from);

	if (kind != JV_KIND_OBJECT
			&& kind != JV_KIND_INVALID)
		return -1;

	if (kind == JV_KIND_INVALID) {
		player_physics = *to = calloc(1, sizeof(struct hb_player_physics));
		player_physics->gravity[0] = player_physics->gravity[1] = 0.0f;
		player_physics->radius = 15.0f;
		player_physics->inv_mass = 0.5f;
		player_physics->b_coef = 0.5f;
		player_physics->damping = 0.96f;
		player_physics->c_group = 0;
		player_physics->acceleration = 0.1f;
		player_physics->kicking_acceleration = 0.07f;
		player_physics->kicking_damping = 0.96f;
		player_physics->kick_strength = 5.0f;
		player_physics->kickback = 0.0f;
		return 0;
	}

	player_physics = *to = calloc(1, sizeof(struct hb_player_physics));

	/////////////gravity
	{
		jv gravity;
		gravity = jv_object_get(jv_copy(from), jv_string("gravity"));
		if (_hb_jv_parse_vec2_and_free(gravity, player_physics->gravity, HB_V2_ZERO) < 0)
			return -1;
	}

	/////////////radius
	{
		jv radius;
		float fallback_radius;
		fallback_radius = 15.0f;
		radius = jv_object_get(jv_copy(from), jv_string("radius"));
		if (_hb_jv_parse_number_and_free(radius, &player_physics->radius, &fallback_radius) < 0)
			return -1;
	}

	/////////////invMass
	{
		jv inv_mass;
		float fallback_inv_mass;
		fallback_inv_mass = 0.5f;
		inv_mass = jv_object_get(jv_copy(from), jv_string("invMass"));
		if (_hb_jv_parse_number_and_free(inv_mass, &player_physics->inv_mass, &fallback_inv_mass) < 0)
			return -1;
	}

	/////////////bCoef
	{
		jv b_coef;
		float fallback_b_coef;
		fallback_b_coef = 0.5f;
		b_coef = jv_object_get(jv_copy(from), jv_string("bCoef"));
		if (_hb_jv_parse_number_and_free(b_coef, &player_physics->b_coef, &fallback_b_coef) < 0)
			return -1;
	}

	/////////////damping
	{
		jv damping;
		float fallback_damping;
		fallback_damping = 0.96f;
		damping = jv_object_get(jv_copy(from), jv_string("damping"));
		if (_hb_jv_parse_number_and_free(damping, &player_physics->damping, &fallback_damping) < 0)
			return -1;
	}

	/////////////cGroup
	{
		jv c_group;
		enum hb_collision_flags fallback_c_group;
		fallback_c_group = 0;
		c_group = jv_object_get(jv_copy(from), jv_string("cGroup"));
		if (_hb_jv_parse_collision_flags_and_free(c_group, &player_physics->c_group, &fallback_c_group) < 0)
			return -1;
	}

	/////////////acceleration
	{
		jv acceleration;
		float fallback_acceleration;
		fallback_acceleration = 0.1f;
		acceleration = jv_object_get(jv_copy(from), jv_string("acceleration"));
		if (_hb_jv_parse_number_and_free(acceleration, &player_physics->acceleration, &fallback_acceleration) < 0)
			return -1;
	}

	/////////////kickingAcceleration
	{
		jv kicking_acceleration;
		float fallback_kicking_acceleration;
		fallback_kicking_acceleration = 0.07f;
		kicking_acceleration = jv_object_get(jv_copy(from), jv_string("kickingAcceleration"));
		if (_hb_jv_parse_number_and_free(kicking_acceleration, &player_physics->kicking_acceleration, &fallback_kicking_acceleration) < 0)
			return -1;
	}

	/////////////kickingDamping
	{
		jv kicking_damping;
		float fallback_kicking_damping;
		fallback_kicking_damping = 0.96f;
		kicking_damping = jv_object_get(jv_copy(from), jv_string("kickingDamping"));
		if (_hb_jv_parse_number_and_free(kicking_damping, &player_physics->kicking_damping, &fallback_kicking_damping) < 0)
			return -1;
	}

	/////////////kickStrength
	{
		jv kick_strength;
		float fallback_kick_strength;
		fallback_kick_strength = 5.0f;
		kick_strength = jv_object_get(jv_copy(from), jv_string("kickStrength"));
		if (_hb_jv_parse_number_and_free(kick_strength, &player_physics->kick_strength, &fallback_kick_strength) < 0)
			return -1;
	}

	/////////////kickback
	{
		jv kickback;
		float fallback_kickback;
		fallback_kickback = 0.0f;
		kickback = jv_object_get(jv_copy(from), jv_string("kickback"));
		if (_hb_jv_parse_number_and_free(kickback, &player_physics->kickback, &fallback_kickback) < 0)
			return -1;
	}

	return 0;
}

//////////////////////////////////////////////
//////////////PARSE + JV_FREE/////////////////
//////////////////////////////////////////////

static inline int
_hb_jv_parse_xxx_and_free_wrapper_1(int ret, jv v)
{
	jv_free(v);
	return ret;
}

static inline int
_hb_jv_parse_xxx_and_free_wrapper_2(int ret, jv v, jv w)
{
	jv_free(v); jv_free(w);
	return ret;
}

static int
_hb_jv_parse_string_and_free(jv from, char **to, const char *fallback)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_string(from, to, fallback), from);
}

static int
_hb_jv_parse_number_and_free(jv from, float *to, const float *fallback)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_number(from, to, fallback), from);
}

static int
_hb_jv_parse_boolean_and_free(jv from, bool *to, const bool *fallback)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_boolean(from, to, fallback), from);
}

static int
_hb_jv_parse_camera_follow_and_free(jv from, enum hb_camera_follow *to,
		const enum hb_camera_follow *fallback)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_camera_follow(from, to, fallback), from);
}

static int
_hb_jv_parse_kick_off_reset_and_free(jv from, enum hb_kick_off_reset *to,
		const enum hb_kick_off_reset *fallback)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_kick_off_reset(from, to, fallback), from);
}

static int
_hb_jv_parse_bg_type_and_free(jv from, enum hb_background_type *to,
		const enum hb_background_type *fallback)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_bg_type(from, to, fallback), from);
}

static int
_hb_jv_parse_color_and_free(jv from, uint32_t *to, const uint32_t *fallback)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_color(from, to, fallback), from);
}

static int
_hb_jv_parse_bg_and_free(jv from, struct hb_background **to)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_bg(from, to), from);
}

static int
_hb_jv_parse_collision_flag_and_free(jv from, enum hb_collision_flags *to,
		const enum hb_collision_flags *fallback)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_collision_flag(from, to, fallback), from);
}

static int
_hb_jv_parse_collision_flags_and_free(jv from, enum hb_collision_flags *to,
		const enum hb_collision_flags *fallback)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_collision_flags(from, to, fallback), from);
}

static int
_hb_jv_parse_trait_and_free(jv from, jv name, struct hb_trait **to)
{
	return _hb_jv_parse_xxx_and_free_wrapper_2(
			_hb_jv_parse_trait(from, name, to), from, name);
}

static int
_hb_jv_parse_trait_list_and_free(jv from, struct hb_trait ***to)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_trait_list(from, to), from);
}

static int
_hb_jv_find_trait_by_name_and_free(jv from, struct hb_trait **to, struct hb_trait **traits)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_find_trait_by_name(from, to, traits), from);
}

static int
_hb_jv_parse_vertex_and_free(jv from, struct hb_vertex **to,
		struct hb_trait **traits)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_vertex(from, to, traits), from);
}

static int
_hb_jv_parse_vertex_list_and_free(jv from, struct hb_vertex ***to,
		struct hb_trait **traits)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_vertex_list(from, to, traits), from);
}

static int
_hb_jv_parse_segment_and_free(jv from, struct hb_segment **to,
	   struct hb_vertex **vertexes, struct hb_trait **traits)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_segment(from, to, vertexes, traits), from);
}

static int
_hb_jv_parse_segment_list_and_free(jv from, struct hb_segment ***to,
		struct hb_vertex **vertexes, struct hb_trait **traits)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_segment_list(from, to, vertexes, traits), from);
}

static int
_hb_jv_parse_vec2_and_free(jv from, float to[2], const float fallback[2])
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_vec2(from, to, fallback), from);
}

static int
_hb_jv_parse_team_and_free(jv from, enum hb_team *to, const enum hb_team *fallback)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_team(from, to, fallback), from);
}

static int
_hb_jv_parse_goal_and_free(jv from, struct hb_goal **to)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_goal(from, to), from);
}

static int
_hb_jv_parse_goal_list_and_free(jv from, struct hb_goal ***to)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_goal_list(from, to), from);
}

static int
_hb_jv_parse_ball_physics_and_free(jv from, struct hb_disc **to)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_ball_physics(from, to), from);
}

static int
_hb_jv_parse_disc_and_free(jv from, struct hb_disc **to,
		struct hb_trait **traits)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_disc(from, to, traits), from);
}

static int
_hb_jv_parse_disc_list_and_free(jv from, struct hb_disc ***to,
		struct hb_trait **traits, struct hb_disc **ball_physics)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_disc_list(from, to, traits, ball_physics), from);
}

static int
_hb_jv_parse_plane_and_free(jv from, struct hb_plane **to, struct hb_trait **traits)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_plane(from, to, traits), from);
}

static int
_hb_jv_parse_plane_list_and_free(jv from, struct hb_plane ***to, struct hb_trait **traits)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_plane_list(from, to, traits), from);
}

static int
_hb_jv_parse_joint_length_and_free(jv from, struct hb_joint_length *to)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_joint_length(from, to), from);
}

static int
_hb_jv_parse_joint_strength_and_free(jv from, struct hb_joint_strength *to)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_joint_strength(from, to), from);
}

static int
_hb_jv_parse_joint_and_free(jv from, struct hb_joint **to, struct hb_disc **discs)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_joint(from, to, discs), from);
}

static int
_hb_jv_parse_joint_list_and_free(jv from, struct hb_joint ***to, struct hb_disc **discs)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_joint_list(from, to, discs), from);
}

static int
_hb_jv_parse_point_and_free(jv from, struct hb_point **to)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_point(from, to), from);
}

static int
_hb_jv_parse_point_list_and_free(jv from, struct hb_point ***to)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_point_list(from, to), from);
}

static int
_hb_jv_parse_player_physics_and_free(jv from, struct hb_player_physics **to)
{
	return _hb_jv_parse_xxx_and_free_wrapper_1(
			_hb_jv_parse_player_physics(from, to), from);
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

	/////////////width
	{
		jv width;
		width = jv_object_get(jv_copy(root), jv_string("width"));
		if (_hb_jv_parse_number_and_free(width, &s->width, NULL) < 0)
			goto err;
	}

	/////////////height
	{
		jv height;
		height = jv_object_get(jv_copy(root), jv_string("height"));
		if (_hb_jv_parse_number_and_free(height, &s->height, NULL) < 0)
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
		if (_hb_jv_parse_bg_and_free(bg, &s->bg) < 0)
			goto err;
	}

	/////////////traits
	{
		jv traits;
		traits = jv_object_get(jv_copy(root), jv_string("traits"));
		if (_hb_jv_parse_trait_list_and_free(traits, &s->traits) < 0)
			goto err;
	}

	/////////////vertexes
	{
		jv vertexes;
		vertexes = jv_object_get(jv_copy(root), jv_string("vertexes"));
		if (_hb_jv_parse_vertex_list_and_free(vertexes, &s->vertexes, s->traits) < 0)
			goto err;
	}

	/////////////segments
	{
		jv segments;
		segments = jv_object_get(jv_copy(root), jv_string("segments"));
		if (_hb_jv_parse_segment_list_and_free(segments, &s->segments, s->vertexes, s->traits) < 0)
			goto err;
	}

	/////////////goals
	{
		jv goals;
		goals = jv_object_get(jv_copy(root), jv_string("goals"));
		if (_hb_jv_parse_goal_list_and_free(goals, &s->goals) < 0)
			goto err;
	}

	/////////////ballPhysics
	{
		jv ball_physics;
		ball_physics = jv_object_get(jv_copy(root), jv_string("ballPhysics"));
		if (_hb_jv_parse_ball_physics_and_free(ball_physics, &s->ball_physics) < 0)
			goto err;
	}

	/////////////discs
	{
		jv discs;
		discs = jv_object_get(jv_copy(root), jv_string("discs"));
		if (_hb_jv_parse_disc_list_and_free(discs, &s->discs, s->traits, &s->ball_physics) < 0)
			goto err;
	}

	/////////////planes
	{
		jv planes;
		planes = jv_object_get(jv_copy(root), jv_string("planes"));
		if (_hb_jv_parse_plane_list_and_free(planes, &s->planes, s->traits) < 0)
			goto err;
	}

	/////////////joints
	{
		jv joints;
		joints = jv_object_get(jv_copy(root), jv_string("joints"));
		if (_hb_jv_parse_joint_list_and_free(joints, &s->joints, s->discs) < 0)
			goto err;
	}

	/////////////redSpawnPoints
	{
		jv red_spawn_points;
		red_spawn_points = jv_object_get(jv_copy(root), jv_string("redSpawnPoints"));
		if (_hb_jv_parse_point_list_and_free(red_spawn_points, &s->red_spawn_points) < 0)
			goto err;
	}

	/////////////blueSpawnPoints
	{
		jv blue_spawn_points;
		blue_spawn_points = jv_object_get(jv_copy(root), jv_string("blueSpawnPoints"));
		if (_hb_jv_parse_point_list_and_free(blue_spawn_points, &s->blue_spawn_points) < 0)
			goto err;
	}

	/////////////playerPhysics
	{
		jv player_physics;
		player_physics = jv_object_get(jv_copy(root), jv_string("playerPhysics"));
		if (_hb_jv_parse_player_physics_and_free(player_physics, &s->player_physics) < 0)
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

static jv
_hb_jv_to_json_camera_follow(enum hb_camera_follow from)
{
	switch (from) {
	case HB_CAMERA_FOLLOW_BALL: return jv_string("ball");
	case HB_CAMERA_FOLLOW_PLAYER: return jv_string("player");
	default: return jv_string("unknown");
	}
}

static jv
_hb_jv_to_json_kick_off_reset(enum hb_kick_off_reset from)
{
	switch (from) {
	case HB_KICK_OFF_RESET_FULL: return jv_string("full");
	case HB_KICK_OFF_RESET_PARTIAL: return jv_string("partial");
	default: return jv_string("unknown");
	}
}

static jv
_hb_jv_to_json_bg_type(enum hb_background_type from)
{
	switch (from) {
	case HB_BACKGROUND_TYPE_NONE: return jv_string("none");
	case HB_BACKGROUND_TYPE_GRASS: return jv_string("grass");
	case HB_BACKGROUND_TYPE_HOCKEY: return jv_string("hockey");
	default: return jv_string("unknown");
	}
}

static jv
_hb_jv_to_json_color(uint32_t from)
{
	if (from & (0xff << 24))
		return jv_string_fmt("%06x", from & 0xffffff);
	return jv_string("transparent");
}

static jv
_hb_jv_to_json_bg(struct hb_background *from)
{
	jv out;
	out = jv_object();

	out = jv_object_set(out, jv_string("type"),
			_hb_jv_to_json_bg_type(from->type));

	out = jv_object_set(out, jv_string("width"),
			jv_number(from->width));

	out = jv_object_set(out, jv_string("height"),
			jv_number(from->height));

	out = jv_object_set(out, jv_string("kickOffRadius"),
			jv_number(from->kick_off_radius));

	out = jv_object_set(out, jv_string("cornerRadius"),
			jv_number(from->corner_radius));

	out = jv_object_set(out, jv_string("goalLine"),
			jv_number(from->goal_line));

	out = jv_object_set(out, jv_string("color"),
			_hb_jv_to_json_color(from->color));

	return out;
}

static jv
_hb_jv_to_json_collision_flags(enum hb_collision_flags from)
{
	jv out;
	out = jv_array();

	if (hb_collision_flags_is_set(from, HB_COLLISION_ALL)) {
		out = jv_array_append(out, jv_string("all"));
		from ^= HB_COLLISION_ALL;
	}

	if (hb_collision_flags_is_set(from, HB_COLLISION_BALL))
		out = jv_array_append(out, jv_string("ball"));

	if (hb_collision_flags_is_set(from, HB_COLLISION_RED))
		out = jv_array_append(out, jv_string("red"));

	if (hb_collision_flags_is_set(from, HB_COLLISION_BLUE))
		out = jv_array_append(out, jv_string("blue"));

	if (hb_collision_flags_is_set(from, HB_COLLISION_RED_KO))
		out = jv_array_append(out, jv_string("redKO"));

	if (hb_collision_flags_is_set(from, HB_COLLISION_BLUE_KO))
		out = jv_array_append(out, jv_string("blueKO"));

	if (hb_collision_flags_is_set(from, HB_COLLISION_WALL))
		out = jv_array_append(out, jv_string("wall"));

	if (hb_collision_flags_is_set(from, HB_COLLISION_KICK))
		out = jv_array_append(out, jv_string("kick"));

	if (hb_collision_flags_is_set(from, HB_COLLISION_SCORE))
		out = jv_array_append(out, jv_string("score"));

	if (hb_collision_flags_is_set(from, HB_COLLISION_C0))
		out = jv_array_append(out, jv_string("c0"));

	if (hb_collision_flags_is_set(from, HB_COLLISION_C1))
		out = jv_array_append(out, jv_string("c1"));

	if (hb_collision_flags_is_set(from, HB_COLLISION_C2))
		out = jv_array_append(out, jv_string("c2"));

	if (hb_collision_flags_is_set(from, HB_COLLISION_C3))
		out = jv_array_append(out, jv_string("c3"));

	return out;
}

static jv
_hb_jv_to_json_vertex(const struct hb_vertex *from)
{
	jv out;
	out = jv_object();

	out = jv_object_set(out, jv_string("x"), jv_number(from->x));
	out = jv_object_set(out, jv_string("y"), jv_number(from->y));
	out = jv_object_set(out, jv_string("bCoef"), jv_number(from->b_coef));
	out = jv_object_set(out, jv_string("cGroup"), _hb_jv_to_json_collision_flags(from->c_group));
	out = jv_object_set(out, jv_string("cMask"), _hb_jv_to_json_collision_flags(from->c_mask));

	return out;
}

static jv
_hb_jv_to_json_segment(const struct hb_segment *from)
{
	jv out;
	out = jv_object();

	out = jv_object_set(out, jv_string("v0"), jv_number(from->v0));
	out = jv_object_set(out, jv_string("v1"), jv_number(from->v1));
	out = jv_object_set(out, jv_string("bCoef"), jv_number(from->b_coef));
	out = jv_object_set(out, jv_string("curve"), jv_number(from->curve));
	out = jv_object_set(out, jv_string("bias"), jv_number(from->bias));
	out = jv_object_set(out, jv_string("cGroup"), _hb_jv_to_json_collision_flags(from->c_group));
	out = jv_object_set(out, jv_string("cMask"), _hb_jv_to_json_collision_flags(from->c_mask));
	out = jv_object_set(out, jv_string("vis"), jv_bool(from->vis));
	out = jv_object_set(out, jv_string("color"), _hb_jv_to_json_color(from->color));

	return out;
}

static jv
_hb_jv_to_json_player_physics(const struct hb_player_physics *from)
{
	jv out;
	out = jv_object();

	out = jv_object_set(out, jv_string("gravity"), JV_ARRAY_2(jv_number(from->gravity[0]), jv_number(from->gravity[1])));
	out = jv_object_set(out, jv_string("radius"), jv_number(from->radius));
	out = jv_object_set(out, jv_string("invMass"), jv_number(from->inv_mass));
	out = jv_object_set(out, jv_string("bCoef"), jv_number(from->b_coef));
	out = jv_object_set(out, jv_string("damping"), jv_number(from->damping));
	out = jv_object_set(out, jv_string("cGroup"), _hb_jv_to_json_collision_flags(from->c_group));
	out = jv_object_set(out, jv_string("acceleration"), jv_number(from->acceleration));
	out = jv_object_set(out, jv_string("kickingAcceleration"), jv_number(from->kicking_acceleration));
	out = jv_object_set(out, jv_string("kickingDamping"), jv_number(from->kicking_damping));
	out = jv_object_set(out, jv_string("kickStrength"), jv_number(from->kick_strength));
	out = jv_object_set(out, jv_string("kickback"), jv_number(from->kickback));

	return out;
}

static jv
_hb_jv_to_json_team(enum hb_team from)
{
	switch (from) {
	case HB_TEAM_RED: return jv_string("red");
	case HB_TEAM_BLUE: return jv_string("blue");
	case HB_TEAM_SPECTATOR: return jv_string("spectator");
	default: return jv_string("unknown");
	}
}

static jv
_hb_jv_to_json_goal(const struct hb_goal *from)
{
	jv out;
	out = jv_object();

	out = jv_object_set(out, jv_string("p0"), JV_ARRAY_2(jv_number(from->p0[0]), jv_number(from->p0[1])));
	out = jv_object_set(out, jv_string("p1"), JV_ARRAY_2(jv_number(from->p1[0]), jv_number(from->p1[1])));
	out = jv_object_set(out, jv_string("team"), _hb_jv_to_json_team(from->team));

	return out;
}

static jv
_hb_jv_to_json_disc(const struct hb_disc *from)
{
	jv out;
	out = jv_object();

	out = jv_object_set(out, jv_string("pos"), JV_ARRAY_2(jv_number(from->pos[0]), jv_number(from->pos[1])));
	out = jv_object_set(out, jv_string("speed"), JV_ARRAY_2(jv_number(from->speed[0]), jv_number(from->speed[1])));
	out = jv_object_set(out, jv_string("gravity"), JV_ARRAY_2(jv_number(from->gravity[0]), jv_number(from->gravity[1])));
	out = jv_object_set(out, jv_string("radius"), jv_number(from->radius));
	out = jv_object_set(out, jv_string("invMass"), jv_number(from->inv_mass));
	out = jv_object_set(out, jv_string("damping"), jv_number(from->damping));
	out = jv_object_set(out, jv_string("color"), _hb_jv_to_json_color(from->color));
	out = jv_object_set(out, jv_string("bCoef"), jv_number(from->b_coef));
	out = jv_object_set(out, jv_string("cMask"), _hb_jv_to_json_collision_flags(from->c_mask));
	out = jv_object_set(out, jv_string("cGroup"), _hb_jv_to_json_collision_flags(from->c_group));

	return out;
}

static jv
_hb_jv_to_json_plane(const struct hb_plane *from)
{
	jv out;
	out = jv_object();

	out = jv_object_set(out, jv_string("normal"), JV_ARRAY_2(jv_number(from->normal[0]), jv_number(from->normal[1])));
	out = jv_object_set(out, jv_string("dist"), jv_number(from->dist));
	out = jv_object_set(out, jv_string("bCoef"), jv_number(from->b_coef));
	out = jv_object_set(out, jv_string("cMask"), _hb_jv_to_json_collision_flags(from->c_mask));
	out = jv_object_set(out, jv_string("cGroup"), _hb_jv_to_json_collision_flags(from->c_group));

	return out;
}

static jv
_hb_jv_to_json_joint_length(const struct hb_joint_length *from)
{
	switch (from->kind) {
	case HB_JOINT_LENGTH_FIXED: return jv_number(from->val.f);
	case HB_JOINT_LENGTH_RANGE: return JV_ARRAY_2(jv_number(from->val.range[0]), jv_number(from->val.range[1]));
	case HB_JOINT_LENGTH_AUTO: return jv_null();
	default: return jv_null();
	}
}

static jv
_hb_jv_to_json_joint_strength(const struct hb_joint_strength *from)
{
	if (from->is_rigid) return jv_string("rigid");
	return jv_number(from->val);
}

static jv
_hb_jv_to_json_joint(const struct hb_joint *from)
{
	jv out;
	out = jv_object();

	out = jv_object_set(out, jv_string("d0"), jv_number(from->d0));
	out = jv_object_set(out, jv_string("d1"), jv_number(from->d1));
	out = jv_object_set(out, jv_string("length"), _hb_jv_to_json_joint_length(&from->length));
	out = jv_object_set(out, jv_string("strength"), _hb_jv_to_json_joint_strength(&from->strength));
	out = jv_object_set(out, jv_string("color"), _hb_jv_to_json_color(from->color));

	return out;
}

static jv
_hb_jv_to_json_point(const struct hb_point *from)
{
	jv out;
	out = jv_array();
	out = jv_array_append(out, jv_number(from->x));
	out = jv_array_append(out, jv_number(from->y));
	return out;
}

static jv
_hb_jv_to_json_stadium(const struct hb_stadium *s)
{
	jv root;

	root = jv_object();

	root = jv_object_set(root, jv_string("name"), jv_string(s->name));
	root = jv_object_set(root, jv_string("width"), jv_number(s->width));
	root = jv_object_set(root, jv_string("height"), jv_number(s->height));

	if (s->camera_width > 0 && s->camera_width > 0) {
		root = jv_object_set(root, jv_string("cameraWidth"),
				jv_number(s->camera_width));
		root = jv_object_set(root, jv_string("cameraHeight"),
				jv_number(s->camera_height));
	}

	root = jv_object_set(root, jv_string("maxViewWidth"),
			jv_number(s->max_view_width));

	root = jv_object_set(root, jv_string("cameraFollow"),
			_hb_jv_to_json_camera_follow(s->camera_follow));

	root = jv_object_set(root, jv_string("spawnDistance"),
			jv_number(s->spawn_distance));

	root = jv_object_set(root, jv_string("canBeStored"),
			jv_bool(s->can_be_stored));

	root = jv_object_set(root, jv_string("kickOffReset"),
			_hb_jv_to_json_kick_off_reset(s->kick_off_reset));

	root = jv_object_set(root, jv_string("ballPhysics"),
			jv_string("disc0"));

	root = jv_object_set(root, jv_string("playerPhysics"),
			_hb_jv_to_json_player_physics(s->player_physics));

	root = jv_object_set(root, jv_string("bg"),
			_hb_jv_to_json_bg(s->bg));

	/////////////vertexes
	{
		jv vertexes;
		vertexes = jv_array();

		hb_stadium_vertexes_foreach(s, vertex) {
			vertexes = jv_array_append(vertexes,
					_hb_jv_to_json_vertex(vertex));
		}

		root = jv_object_set(root, jv_string("vertexes"),
			vertexes);
	}

	/////////////segments
	{
		jv segments;
		segments = jv_array();

		hb_stadium_segments_foreach(s, segment) {
			segments = jv_array_append(segments,
					_hb_jv_to_json_segment(segment));
		}

		root = jv_object_set(root, jv_string("segments"),
			segments);
	}

	/////////////goals
	{
		jv goals;
		goals = jv_array();

		hb_stadium_goals_foreach(s, goal) {
			goals = jv_array_append(goals,
					_hb_jv_to_json_goal(goal));
		}

		root = jv_object_set(root, jv_string("goals"),
			goals);
	}

	/////////////discs
	{
		jv discs;
		discs = jv_array();

		hb_stadium_discs_foreach(s, disc) {
			discs = jv_array_append(discs,
					_hb_jv_to_json_disc(disc));
		}

		root = jv_object_set(root, jv_string("discs"),
			discs);
	}

	/////////////planes
	{
		jv planes;
		planes = jv_array();

		hb_stadium_planes_foreach(s, plane) {
			planes = jv_array_append(planes,
					_hb_jv_to_json_plane(plane));
		}

		root = jv_object_set(root, jv_string("planes"),
			planes);
	}

	///////////joints
	{
		jv joints;
		joints = jv_array();

		hb_stadium_joints_foreach(s, joint) {
			joints = jv_array_append(joints,
					_hb_jv_to_json_joint(joint));
		}

		root = jv_object_set(root, jv_string("joints"),
			joints);
	}

	/////////////redSpawnPoints
	{
		jv red_spawn_points;
		red_spawn_points = jv_array();

		hb_stadium_red_spawn_points_foreach(s, point) {
			red_spawn_points = jv_array_append(red_spawn_points,
					_hb_jv_to_json_point(point));
		}

		root = jv_object_set(root, jv_string("redSpawnPoints"),
			red_spawn_points);
	}

	/////////////blueSpawnPoints
	{
		jv blue_spawn_points;
		blue_spawn_points = jv_array();

		hb_stadium_blue_spawn_points_foreach(s, point) {
			blue_spawn_points = jv_array_append(blue_spawn_points,
					_hb_jv_to_json_point(point));
		}

		root = jv_object_set(root, jv_string("blueSpawnPoints"),
			blue_spawn_points);
	}

	return root;
}

extern char *
hb_stadium_to_json(const struct hb_stadium *s)
{
	jv root, out;
	char *str;

	root = _hb_jv_to_json_stadium(s);
	out = jv_dump_string(root, 0);
	str = strdup(jv_string_value(out));
	jv_free(out);

	return str;
}

extern void
hb_stadium_print(const struct hb_stadium *s)
{
	jv root, out;

	root = _hb_jv_to_json_stadium(s);
	out = jv_dump_string(root, 0);
	printf("%s\n", jv_string_value(out));
	jv_free(out);
}

extern void
hb_stadium_free(struct hb_stadium *s)
{
	free(s->name);
	free(s->bg);

	hb_stadium_traits_foreach(s, trait) {
		free(trait->name);
		free(trait);
	}

	hb_stadium_vertexes_foreach(s, vertex) free(vertex);
	hb_stadium_segments_foreach(s, segment) free(segment);
	hb_stadium_goals_foreach(s, goal) free(goal);
	hb_stadium_discs_foreach(s, disc) free(disc);
	hb_stadium_planes_foreach(s, plane) free(plane);
	hb_stadium_joints_foreach(s, joint) free(joint);
	hb_stadium_red_spawn_points_foreach(s, point) free(point);
	hb_stadium_blue_spawn_points_foreach(s, point) free(point);

	free(s->traits);
	free(s->vertexes);
	free(s->segments);
	free(s->goals);
	free(s->discs);
	free(s->planes);
	free(s->joints);
	free(s->red_spawn_points);
	free(s->blue_spawn_points);
	free(s->player_physics);
	free(s);
}

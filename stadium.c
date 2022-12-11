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

static int
_hb_collision_flags_parse(const char *str, enum hb_collision_flags *flags)
{
	if (!strcmp(str, "ball")) *flags = HB_COLLISION_BALL;
	else if (!strcmp(str, "red")) *flags = HB_COLLISION_RED;
	else if (!strcmp(str, "blue")) *flags = HB_COLLISION_BLUE;
	else if (!strcmp(str, "redKO")) *flags = HB_COLLISION_RED_KO;
	else if (!strcmp(str, "blueKO")) *flags = HB_COLLISION_BLUE_KO;
	else if (!strcmp(str, "wall")) *flags = HB_COLLISION_WALL;
	else if (!strcmp(str, "all")) *flags = HB_COLLISION_ALL;
	else if (!strcmp(str, "kick")) *flags = HB_COLLISION_KICK;
	else if (!strcmp(str, "score")) *flags = HB_COLLISION_SCORE;
	else if (!strcmp(str, "c0")) *flags = HB_COLLISION_C0;
	else if (!strcmp(str, "c1")) *flags = HB_COLLISION_C1;
	else if (!strcmp(str, "c2")) *flags = HB_COLLISION_C2;
	else if (!strcmp(str, "c3")) *flags = HB_COLLISION_C3;
	else {
		_err_unmatched_property_value("collision_flags",
				"<collision_flags>", str);
		return -1;
	}

	return 0;
}

static int
_hb_trait_parse(jv root, struct hb_trait *trait)
{
	/////////////curve
	{
		jv             curve;
		jv_kind   curve_kind;

		curve = jv_object_get(jv_copy(root), jv_string("curve"));
		curve_kind = jv_get_kind(curve);

		if (curve_kind == JV_KIND_NUMBER) {
			trait->has_curve = true;
			trait->curve = jv_number_value(curve);
		} else if (curve_kind != JV_KIND_INVALID) {
			_err_unmatched_property_type("trait.curve",
					JV_KIND_NUMBER, curve_kind);
			return -1;
		}
	}

	/////////////damping
	{
		jv             damping;
		jv_kind   damping_kind;

		damping = jv_object_get(jv_copy(root), jv_string("damping"));
		damping_kind = jv_get_kind(damping);

		if (damping_kind == JV_KIND_NUMBER) {
			trait->has_damping = true;
			trait->damping = jv_number_value(damping);
		} else if (damping_kind != JV_KIND_INVALID) {
			_err_unmatched_property_type("trait.damping",
					JV_KIND_NUMBER, damping_kind);
			return -1;
		}
	}

	/////////////invMass
	{
		jv             inv_mass;
		jv_kind   inv_mass_kind;

		inv_mass = jv_object_get(jv_copy(root), jv_string("invMass"));
		inv_mass_kind = jv_get_kind(inv_mass);

		if (inv_mass_kind == JV_KIND_NUMBER) {
			trait->has_inv_mass = true;
			trait->inv_mass = jv_number_value(inv_mass);
		} else if (inv_mass_kind != JV_KIND_INVALID) {
			_err_unmatched_property_type("trait.invMass",
					JV_KIND_NUMBER, inv_mass_kind);
			return -1;
		}
	}

	/////////////radius
	{
		jv             radius;
		jv_kind   radius_kind;

		radius = jv_object_get(jv_copy(root), jv_string("radius"));
		radius_kind = jv_get_kind(radius);

		if (radius_kind == JV_KIND_NUMBER) {
			trait->has_radius = true;
			trait->radius = jv_number_value(radius);
		} else if (radius_kind != JV_KIND_INVALID) {
			_err_unmatched_property_type("trait.radius",
					JV_KIND_NUMBER, radius_kind);
			return -1;
		}
	}

	/////////////bCoef
	{
		jv             b_coef;
		jv_kind   b_coef_kind;

		b_coef = jv_object_get(jv_copy(root), jv_string("bCoef"));
		b_coef_kind = jv_get_kind(b_coef);

		if (b_coef_kind == JV_KIND_NUMBER) {
			trait->has_b_coef = true;
			trait->b_coef = jv_number_value(b_coef);
		} else if (b_coef_kind != JV_KIND_INVALID) {
			_err_unmatched_property_type("trait.bCoef",
					JV_KIND_NUMBER, b_coef_kind);
			return -1;
		}
	}

	/////////////color
	{
		jv                  color;
		jv_kind        color_kind;
		const char     *color_str;
		char           *parse_end;
		jv                r, g, b;
		jv_kind            r_kind;
		jv_kind            g_kind;
		jv_kind            b_kind;
		int               arr_len;

		color = jv_object_get(jv_copy(root), jv_string("color"));
		color_kind = jv_get_kind(color);

		if (color_kind == JV_KIND_STRING) {
			color_str = jv_string_value(color);

			if (!strcmp(color_str, "transparent")) {
				trait->has_color = true;
				trait->color = 0x00000000;
			} else {
				trait->color = strtol(color_str, &parse_end, 16);

				if (parse_end - color_str != 6 ||
						parse_end[1] != '\0') {
					_err_unmatched_property_value("trait.color",
							"RRGGBB", color_str);
					return -1;
				}

				trait->has_color = true;
			}
		} else if (color_kind == JV_KIND_ARRAY) {
			if (jv_array_length(color) == 3) {
				r = jv_array_get(color, 0);
				g = jv_array_get(color, 1);
				b = jv_array_get(color, 2);

				r_kind = jv_get_kind(r);
				g_kind = jv_get_kind(g);
				b_kind = jv_get_kind(b);

				if (r_kind == JV_KIND_NUMBER &&
						g_kind == JV_KIND_NUMBER &&
						b_kind == JV_KIND_NUMBER) {
					trait->has_color = true;
					trait->color =
											 (0xff << 24) |
						((int)(jv_number_value(r)) << 16) |
						((int)(jv_number_value(g)) <<  8) |
						((int)(jv_number_value(b)) <<  0);

				} else {
					// FIXME: convert the array to string in an error func
					_err_unmatched_property_value("trait.color",
							"[R, G, B]", "unknown");
					return -1;
				}
			} else {
				// FIXME: convert the array to string in an error func
				_err_unmatched_property_value("trait.color",
						"[R, G, B]", "unknown");
				return -1;
			}
		} else if (color_kind != JV_KIND_INVALID) {
			_err_unmatched_property_value("trait.color",
					"[R, G, B]", "unknown");
			return -1;
		}
	}

	/////////////vis
	{
		jv             vis;
		jv_kind   vis_kind;

		vis = jv_object_get(jv_copy(root), jv_string("vis"));
		vis_kind = jv_get_kind(vis);

		if (vis_kind == JV_KIND_TRUE ||
				vis_kind == JV_KIND_FALSE) {
			trait->has_vis = true;
			trait->vis = vis_kind == JV_KIND_TRUE;
		} else if (vis_kind != JV_KIND_INVALID) {
			_err_unmatched_property_value("trait.vis",
					"[true, false]", jv_kind_name(vis_kind));
			return -1;
		}
	}

	/////////////cGroup
	{
		jv                               c_group;
		jv_kind                     c_group_kind;
		enum hb_collision_flags      c_group_val;

		c_group = jv_object_get(jv_copy(root), jv_string("cGroup"));
		c_group_kind = jv_get_kind(c_group);

		if (c_group_kind == JV_KIND_ARRAY) {
			jv_array_foreach(c_group, idx, value) {
				if (jv_get_kind(value) == JV_KIND_STRING) {
					if (_hb_collision_flags_parse(jv_string_value(value), &c_group_val) < 0)
						return -1;
					trait->has_c_group = true;
					trait->c_group |= c_group_val;
				} else {
					_err_unmatched_property_value("trait.cGroup",
							"[collision_flags...]", jv_kind_name(c_group_kind));
					return -1;
				}
			}
		} else if (c_group_kind != JV_KIND_INVALID) {
			_err_unmatched_property_value("trait.cGroup",
					"[collision_flags...]", jv_kind_name(c_group_kind));
			return -1;
		}
	}

	/////////////cMask
	{
		jv                               c_mask;
		jv_kind                     c_mask_kind;
		enum hb_collision_flags      c_mask_val;

		c_mask = jv_object_get(jv_copy(root), jv_string("cMask"));
		c_mask_kind = jv_get_kind(c_mask);

		if (c_mask_kind == JV_KIND_ARRAY) {
			jv_array_foreach(c_mask, idx, value) {
				if (jv_get_kind(value) == JV_KIND_STRING) {
					if (_hb_collision_flags_parse(jv_string_value(value), &c_mask_val) < 0)
						return -1;
					trait->has_c_mask = true;
					trait->c_mask |= c_mask_val;
				} else {
					_err_unmatched_property_value("trait.cMask",
							"[collision_flags...]", jv_kind_name(c_mask_kind));
					return -1;
				}
			}
		} else if (c_mask_kind != JV_KIND_INVALID) {
			_err_unmatched_property_value("trait.cMask",
					"[collision_flags...]", jv_kind_name(c_mask_kind));
			return -1;
		}
	}

	return 0;
}

static int
_hb_jv_parse_string(jv from, char **to, const char *fallback)
{
	jv_kind kind;

	kind = jv_get_kind(from);

	switch (kind) {
	case JV_KIND_STRING:
		*to = strdup(jv_string_value(from));
		return 0;
	case JV_KIND_INVALID:
		if (fallback != NULL) {
			*to = strdup(fallback);
			return 0;
		}
		return -1;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_number(jv from, float *to, const float *fallback)
{
	jv_kind kind;

	kind = jv_get_kind(from);

	switch (kind) {
	case JV_KIND_NUMBER:
		*to = jv_number_value(from);
		return 0;
	case JV_KIND_INVALID:
		if (fallback != NULL) {
			*to = *fallback;
			return 0;
		}
		return -1;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_boolean(jv from, bool *to, const bool *fallback)
{
	jv_kind kind;

	kind = jv_get_kind(from);

	switch (kind) {
	case JV_KIND_FALSE:
		*to = false;
		return 0;
	case JV_KIND_TRUE:
		*to = true;
		return 0;
	case JV_KIND_INVALID:
		if (NULL != fallback) {
			*to = *fallback;
			return 0;
		}
		return -1;
	default:
		return -1;
	}
}

static int
_hb_jv_parse_color(jv from, uint32_t *color, const uint32_t *fallback)
{
	jv_kind kind;
	const char *color_str;
	char *color_str_end;

	kind = jv_get_kind(from);

	if (kind == JV_KIND_INVALID) {
		if (NULL != fallback) {
			*color = *fallback;
			return 0;
		}
		return -1;
	}

	if (kind == JV_KIND_STRING) {
		color_str = jv_string_value(from);
		if (!strcmp(color_str, "transparent")) *color = 0x00000000;
		else {
			*color = strtol(color_str, &color_str_end, 16);
			if (color_str_end - color_str != 6 ||
					color_str_end[1] != '\0')
				return -1;
		}
		return 0;
	}

	if (kind == JV_KIND_ARRAY) {
		if (jv_array_length(from) != 3)
			return -1;
		*color = 0xff << 24;
		jv_array_foreach(from, index, value) {
			if (jv_get_kind(value) != JV_KIND_NUMBER)
				return -1;
			*color |= ((int)(jv_number_value(value)) & 0xff) << (8*(2-index));
		}
		return 0;
	}

	return -1;
}

static int
_hb_jv_parse_bg(jv from, struct hb_background **bg_ref)
{
	struct hb_background *bg;
	const float zero = 0.0f;
	const uint32_t def_color = 0xff718c5a;
	jv_kind kind;

	kind = jv_get_kind(from);

	bg = *bg_ref = malloc(sizeof(struct hb_background));

	bg->type = HB_BACKGROUND_TYPE_NONE;
	bg->width = 0.0f;
	bg->height = 0.0f;
	bg->kick_off_radius = 0.0f;
	bg->corner_radius = 0.0f;
	bg->goal_line = 0.0f;
	bg->color = 0xff718c5a;

	if (kind == JV_KIND_INVALID)
		return 0;
	if (kind != JV_KIND_OBJECT)
		return -1;

	/////////////type
	{
		jv type;
		char *type_str;
		type = jv_object_get(jv_copy(from), jv_string("type"));
		if (_hb_jv_parse_string(type, &type_str, "none") < 0)
			return -1;
		if (!strcmp(type_str, "none")) bg->type = HB_BACKGROUND_TYPE_NONE;
		else if (!strcmp(type_str, "grass")) bg->type = HB_BACKGROUND_TYPE_GRASS;
		else if (!strcmp(type_str, "hockey")) bg->type = HB_BACKGROUND_TYPE_HOCKEY;
		else { free(type_str); return -1; }
		free(type_str);
	}

	/////////////width & height
	{
		jv width, height;
		width = jv_object_get(jv_copy(from), jv_string("width"));
		height = jv_object_get(jv_copy(from), jv_string("height"));
		if (_hb_jv_parse_number(width, &bg->width, &zero) < 0 ||
				_hb_jv_parse_number(height, &bg->height, &zero) < 0)
			return -1;
	}

	/////////////kickOffRadius
	{
		jv kick_off_radius;
		kick_off_radius = jv_object_get(jv_copy(from), jv_string("kickOffRadius"));
		if (_hb_jv_parse_number(kick_off_radius, &bg->kick_off_radius, &zero) < 0)
			return -1;
	}

	/////////////cornerRadius
	{
		jv corner_radius;
		corner_radius = jv_object_get(jv_copy(from), jv_string("cornerRadius"));
		if (_hb_jv_parse_number(corner_radius, &bg->corner_radius, &zero) < 0)
			return -1;
	}

	/////////////goalLine
	{
		jv goal_line;
		goal_line = jv_object_get(jv_copy(from), jv_string("goalLine"));
		if (_hb_jv_parse_number(goal_line, &bg->goal_line, &zero) < 0)
			return -1;
	}

	/////////////color
	{
		jv color;
		color = jv_object_get(jv_copy(from), jv_string("color"));
		if (_hb_jv_parse_color(color, &bg->color, &def_color) < 0)
			return -1;
	}

	return 0;
}

extern struct hb_stadium *
hb_stadium_parse(const char *in)
{
	struct hb_stadium *s;
	const float zero = 0.0f;
	const bool _true = true;
	jv root;

	/////////////setup
	s = calloc(1, sizeof(struct hb_stadium));
	root = jv_parse(in);

	/////////////name
	{
		jv name;
		name = jv_object_get(jv_copy(root), jv_string("name"));
		if (_hb_jv_parse_string(name, &s->name, NULL) < 0)
			goto err;
	}

	/////////////cameraWidth & cameraHeight
	{
		jv cam_width, cam_height;
		cam_width = jv_object_get(jv_copy(root), jv_string("cameraWidth"));
		cam_height = jv_object_get(jv_copy(root), jv_string("cameraHeight"));
		if (_hb_jv_parse_number(cam_width, &s->camera_width, &zero) < 0
				&& _hb_jv_parse_number(cam_height, &s->camera_height, &zero) < 0)
			goto err;
	}

	/////////////maxViewWidth
	{
		jv max_view_width;
		max_view_width = jv_object_get(jv_copy(root), jv_string("maxViewWidth"));
		if (_hb_jv_parse_number(max_view_width, &s->max_view_width, &zero) < 0)
			goto err;
	}

	/////////////cameraFollow
	{
		jv camera_follow;
		char *camera_follow_str;
		camera_follow = jv_object_get(jv_copy(root), jv_string("cameraFollow"));
		if (_hb_jv_parse_string(camera_follow, &camera_follow_str, "ball") < 0)
			goto err;
		if (!strcmp(camera_follow_str, "ball")) {
			s->camera_follow = HB_CAMERA_FOLLOW_BALL;
		} else if (!strcmp(camera_follow_str, "player")) {
			s->camera_follow = HB_CAMERA_FOLLOW_PLAYER;
		} else {
			goto err;
		}
		free(camera_follow_str);
	}

	/////////////spawnDistance
	{
		jv spawn_distance;
		spawn_distance = jv_object_get(jv_copy(root), jv_string("spawnDistance"));
		if (_hb_jv_parse_number(spawn_distance, &s->spawn_distance, &zero) < 0)
			goto err;
	}

	/////////////canBeStored
	{
		jv can_be_stored;
		can_be_stored = jv_object_get(jv_copy(root), jv_string("canBeStored"));
		if (_hb_jv_parse_boolean(can_be_stored, &s->can_be_stored, &_true) < 0)
			goto err;
	}

	/////////////kickOffReset
	{
		jv kick_off_reset;
		char *kick_off_reset_str;
		kick_off_reset = jv_object_get(jv_copy(root), jv_string("kickOffReset"));
		if (_hb_jv_parse_string(kick_off_reset, &kick_off_reset_str, "partial") < 0)
			goto err;
		if (!strcmp(kick_off_reset_str, "partial")) {
			s->kick_off_reset = HB_KICK_OFF_RESET_PARTIAL;
		} else if (!strcmp(kick_off_reset_str, "full")) {
			s->kick_off_reset = HB_KICK_OFF_RESET_FULL;
		} else {
			goto err;
		}
		free(kick_off_reset_str);
	}

	/////////////bg
	{
		jv bg;
		bg = jv_object_get(jv_copy(root), jv_string("bg"));
		if (_hb_jv_parse_bg(bg, &s->bg) < 0)
			goto err;
	}

	/////////////traits
	{
		jv                  traits;
		jv_kind        traits_kind;
		int             traits_len;
		int            trait_index;

		traits = jv_object_get(jv_copy(root), jv_string("traits"));
		traits_kind = jv_get_kind(traits);

		if (traits_kind == JV_KIND_OBJECT) {
			trait_index = 0;
			traits_len = jv_object_length(jv_copy(traits));
			s->traits = calloc(traits_len + 1, sizeof(struct hb_trait *));

			jv_object_foreach(traits, key, value) {
				s->traits[trait_index] = calloc(1, sizeof(struct hb_trait));
				s->traits[trait_index]->name = strdup(jv_string_value(key));
				if (_hb_trait_parse(value, s->traits[trait_index++]) < 0) {
					goto err;
				}
			}
		} else if (traits_kind == JV_KIND_INVALID) {
			s->traits = calloc(1, sizeof(struct hb_trait *));
		} else {
			_err_unmatched_property_type("traits",
					JV_KIND_OBJECT, traits_kind);
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

static void
_hb_trait_print(struct hb_trait *t)
{
	if (t->has_curve) printf("Trait[%s].curve: %.2f\n", t->name, t->curve);
	if (t->has_damping) printf("Trait[%s].damping: %.2f\n", t->name, t->damping);
	if (t->has_inv_mass) printf("Trait[%s].invMass: %.2f\n", t->name, t->inv_mass);
	if (t->has_radius) printf("Trait[%s].radius: %.2f\n", t->name, t->radius);
	if (t->has_b_coef) printf("Trait[%s].bCoef: %.2f\n", t->name, t->b_coef);
	if (t->has_color) printf("Trait[%s].color: %08x\n", t->name, t->color);
	if (t->has_vis) printf("Trait[%s].vis: %s\n", t->name, _hb_bool_yes_no_to_string(t->vis));
	if (t->has_c_group) printf("Trait[%s].cGroup: %d\n", t->name, t->c_group);
	if (t->has_c_mask) printf("Trait[%s].cMask: %d\n", t->name, t->c_mask);
}

extern const char *
hb_stadium_print(struct hb_stadium *s)
{
	struct hb_trait **trait;

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
	printf("Background.KickOffRadius: %.2f\n", s->bg->kick_off_radius);
	printf("Background.CornerRadius: %.2f\n", s->bg->corner_radius);
	printf("Background.GoalLine: %.2f\n", s->bg->goal_line);
	printf("Background.Color: %08x\n", s->bg->color);

	for (trait = s->traits; *trait; ++trait)
		_hb_trait_print(*trait);
}

int
main(int argc, char **argv)
{
	struct hb_stadium *big;
	big = hb_stadium_from_file(argc > 1 ? argv[1] : "stadiums/big.hbs");
	if (!big) fprintf(stderr, "Bad stadium file!\n");
	else hb_stadium_print(big);
	return 0;
}

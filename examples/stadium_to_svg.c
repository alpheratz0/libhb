/*

	cc -std=c99 -pedantic -Wall -Wextra -Os -I/usr/include/cairo -s \
			-o stadium_to_svg stadium_to_svg.c -lcairo -ljq -lhb -lm

	TODO:
		stadium:bg:type:grass
		stadium:bg:type:hockey
		stadium:bg:cornerRadius
		stadium:planes
		stadium:joints

*/

#include <cairo/cairo-svg.h>
#include <cairo/cairo.h>
#include <hb/stadium.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define deg2rad(deg) ((deg*M_PI)/180)

static inline double
dist(struct hb_vertex *a, struct hb_vertex *b)
{
	double dx, dy;
	dx = b->x - a->x;
	dy = b->y - a->y;
	return sqrt(dx*dx+dy*dy);
}

static void
cairo_set_source_rgb_uint32(cairo_t *cr, uint32_t color)
{
	double r, g, b;

	r = (double)((color >> 16) & 0xff) / 255;
	g = (double)((color >>  8) & 0xff) / 255;
	b = (double)((color >>  0) & 0xff) / 255;

    cairo_set_source_rgb (cr, r, g, b);
}

static void
cairo_render_stadium_bg(cairo_t *cr, struct hb_stadium *s)
{
	cairo_surface_t *image;
	cairo_pattern_t *pattern;
	switch (s->bg->type) {
	case HB_BACKGROUND_TYPE_HOCKEY:
		image = cairo_image_surface_create_from_png ("assets/concrete2.png");
		pattern = cairo_pattern_create_for_surface (image);
		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
		cairo_set_source (cr, pattern);
		cairo_rectangle (cr, -s->width, -s->height, s->width*2, s->height*2);
		cairo_fill (cr);
		cairo_surface_destroy(image);
		cairo_pattern_destroy(pattern);
		image = cairo_image_surface_create_from_png ("assets/concrete.png");
		pattern = cairo_pattern_create_for_surface (image);
		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
		cairo_set_source (cr, pattern);
		cairo_rectangle (cr, -s->bg->width, -s->bg->height, s->bg->width*2, s->bg->height*2);
		cairo_fill (cr);
		cairo_surface_destroy(image);
		cairo_pattern_destroy(pattern);
		break;
	case HB_BACKGROUND_TYPE_GRASS:
		cairo_set_source_rgb_uint32(cr, s->bg->color);
		cairo_rectangle (cr, -s->width, -s->height, s->width*2, s->height*2);
		cairo_fill(cr);
		image = cairo_image_surface_create_from_png ("assets/grass.png");
		pattern = cairo_pattern_create_for_surface (image);
		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
		cairo_set_source (cr, pattern);
		cairo_rectangle (cr, -s->bg->width, -s->bg->height, s->bg->width*2, s->bg->height*2);
		cairo_fill (cr);
		cairo_surface_destroy(image);
		cairo_pattern_destroy(pattern);
		break;
	case HB_BACKGROUND_TYPE_NONE:
		cairo_set_line_width(cr, 3);
		cairo_set_source_rgb_uint32(cr, s->bg->color);
		cairo_rectangle(cr, -s->width, -s->height, s->width*2, s->height*2);
		cairo_fill(cr);
		break;
	}
}

static void
cairo_render_stadium_segment_line(cairo_t *cr, struct hb_vertex *v0,
		struct hb_vertex *v1, uint32_t color)
{
	cairo_set_line_width(cr, 3);
	cairo_set_source_rgb_uint32(cr, color);
	cairo_move_to(cr, v0->x, v0->y);
	cairo_line_to(cr, v1->x, v1->y);
	cairo_stroke(cr);
}

static void
cairo_render_stadium_segment_curve(cairo_t *cr, struct hb_vertex *v0,
		struct hb_vertex *v1, double curve, uint32_t color)
{
	double a, b;
	double mx, my;
	double cx, cy;
	double radius;
	double angle0, angle1;

	cairo_set_line_width(cr, 3);
	cairo_set_source_rgb_uint32(cr, color);

	if (curve < 0) {
		struct hb_vertex *tmp;
		curve *= -1;
		tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	mx = (v0->x + v1->x) / 2;
	my = (v0->y + v1->y) / 2;

	if (curve == 180) {
		cx = mx; cy = my;
		radius = dist(v0, v1) / 2;
		angle0 = atan2(v0->y - my, v0->x - mx);
		angle1 = atan2(v1->y - my, v1->x - mx);
	} else {
		radius = dist(v0, v1) / (2 * sin(deg2rad(curve)/2));
		a = dist(v0, v1) / 2;
		b = sqrt(radius*radius-a*a);
		cx = mx - (b*((v1->y - v0->y) / 2))/a;
		cy = my + (b*((v1->x - v0->x) / 2))/a;
		angle0 = atan2(v0->y - cy, v0->x - cx);
		angle1 = atan2(v1->y - cy, v1->x - cx);
	}

	cairo_arc(cr, cx, cy, radius, angle0, angle1);
	cairo_stroke(cr);
}

static void
cairo_render_stadium_segments(cairo_t *cr, struct hb_stadium *s)
{
	hb_stadium_segments_foreach(s, segment) {
		if (segment->vis) {
			if (segment->curve == 0) {
				cairo_render_stadium_segment_line(cr, s->vertexes[segment->v0],
						s->vertexes[segment->v1], segment->color);
			} else {
				cairo_render_stadium_segment_curve(cr, s->vertexes[segment->v0],
						s->vertexes[segment->v1], segment->curve, segment->color);
			}
		}
	}
}

static void
cairo_render_stadium_discs(cairo_t *cr, struct hb_stadium *s)
{
	hb_stadium_discs_foreach(s, disc) {
		cairo_set_source_rgb_uint32(cr, disc->color);
		cairo_arc(cr, disc->pos[0], disc->pos[1], disc->radius, 0, M_PI*2);
		cairo_fill(cr);
		cairo_set_source_rgb_uint32(cr, 0);
		cairo_set_line_width(cr, 2);
		cairo_arc(cr, disc->pos[0], disc->pos[1], disc->radius, 0, M_PI*2);
		cairo_stroke(cr);
		cairo_set_line_width(cr, 3);
	}
}

int
main(int argc, char **argv)
{
	struct hb_stadium *stadium;
	const char *stadium_path, *out_svg_path;
	cairo_surface_t *surface;
	cairo_t *cr;

	stadium_path = argc > 1 ? argv[1] : "stadium.hbs";
	out_svg_path = argc > 2 ? argv[2] : "stadium.svg";

	if (NULL == (stadium = hb_stadium_from_file(stadium_path))) {
		fprintf(stderr, "stadium_to_svg: couldn't load %s\n",
				stadium_path);
		exit(1);
	}

	surface = cairo_svg_surface_create(out_svg_path, stadium->width * 2,
			stadium->height * 2);

	cr = cairo_create(surface);

	cairo_translate(cr, stadium->width, stadium->height);
	cairo_render_stadium_bg(cr, stadium);
	cairo_render_stadium_segments(cr, stadium);
	cairo_render_stadium_discs(cr, stadium);
	cairo_translate(cr, -stadium->width, -stadium->height);
	cairo_destroy(cr);
	cairo_surface_destroy(surface);

	return 0;
}

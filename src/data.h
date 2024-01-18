#ifndef SPACEINVADERS_DATA_H
#define SPACEINVADERS_DATA_H

#include <cstdlib>

enum tater_type: uint8_t {
	dead_tater   = 0,
	small_tater = 1,
	medium_tater = 2,
	big_tater = 3
};

struct sprite_buffer {
	size_t width, height;
	uint32_t* buf;
};

struct sprite {
	size_t width, height;
	uint8_t* buf;
};

struct tater {
	size_t x, y;
	uint8_t type;
};

struct bullet {
	size_t x, y;
	int dir;
};

struct game_player {
  size_t x, y;
	size_t life;
};

struct game_data {
	size_t width, height;
	size_t num_aliens;
	size_t num_bullets;
	tater* aliens;
  game_player player;
  bullet bullets[180];
};

struct dynamic_sprite {
	bool loop;
	size_t num_frames;
	size_t frame_duration;
	size_t time;
	sprite** frames;
};

static const char* fragment_shader =
		"\n"
		"#version 330\n"
		"\n"
		"uniform sampler2D buffer;\n"
		"noperspective in vec2 TexCoord;\n"
		"\n"
		"out vec3 outColor;\n"
		"\n"
		"void main(void){\n"
		"    outColor = texture(buffer, TexCoord).rgb;\n"
		"}\n";

static const char* vertex_shader =
		"\n"
		"#version 330\n"
		"\n"
		"noperspective out vec2 TexCoord;\n"
		"\n"
		"void main(void){\n"
		"\n"
		"    TexCoord.x = (gl_VertexID == 2)? 2.0: 0.0;\n"
		"    TexCoord.y = (gl_VertexID == 1)? 2.0: 0.0;\n"
		"    \n"
		"    gl_Position = vec4(2.0 * TexCoord - 1.0, 0.0, 1.0);\n"
		"}\n";

#endif

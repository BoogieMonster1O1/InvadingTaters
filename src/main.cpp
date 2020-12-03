#include <cstdio>
#include <cstdint>
#include <limits>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "data.h"
#include "drawablehelper.h"
#include "glutil.h"
#include "state_manager.h"

using namespace std;

bool running = false;
int move_dir = 0;
bool pressed = false;

void errorCallback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
	switch(key){
		case GLFW_KEY_ESCAPE:
			if(action == GLFW_PRESS) running = false;
			break;
		case GLFW_KEY_RIGHT:
			if(action == GLFW_PRESS) move_dir += 1;
			else if(action == GLFW_RELEASE) move_dir -= 1;
			break;
		case GLFW_KEY_LEFT:
			if(action == GLFW_PRESS) move_dir -= 1;
			else if(action == GLFW_RELEASE) move_dir += 1;
			break;
		case GLFW_KEY_SPACE:
			if(action == GLFW_RELEASE) pressed = true;
			break;
		default:
			break;
	}
}

double random(uint32_t* rng) {
	uint32_t x = *rng;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	*rng = x;
	return (double) x / numeric_limits<uint32_t>::max();
}

void clearBuffer(sprite_buffer *buffer, uint32_t color) {
	for(size_t i = 0; i < buffer->width * buffer->height; ++i) {
		buffer->buf[i] = color;
	}
}

uint32_t pack_rgb(uint8_t r, uint8_t g, uint8_t b) {
	return (r << 24) | (g << 16) | (b << 8) | 255;
}

int main(int argc, char** argv) {
	const size_t buffer_width = 224;
	const size_t buffer_height = 256;

	glfwSetErrorCallback(errorCallback);

	if (!glfwInit()) return -1;

	windowHints();

	GLFWwindow* window = glfwCreateWindow(2 * buffer_width, 2 * buffer_height, "Space Invaders", nullptr, nullptr);
	if(!window) {
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, keyCallback);

	glfwMakeContextCurrent(window);

	GLenum err = glewInit();
	if(err != GLEW_OK) {
		fprintf(stderr, "Error initializing GLEW.\n");
		glfwTerminate();
		return -1;
	}

	int glVersion[2] = {-1, 1};
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);

	gl_debug(__FILE__, __LINE__);

	printf("OpenGL: %d.%d\n", glVersion[0], glVersion[1]);
	printf("Renderer: %s\n", glGetString(GL_RENDERER));

	glfwSwapInterval(1);

	StateManager::clearColor(1.0, 0.0, 0.0, 1.0);

	sprite_buffer buffer{};
	buffer.width  = buffer_width;
	buffer.height = buffer_height;
	buffer.buf   = new uint32_t[buffer.width * buffer.height];

	clearBuffer(&buffer, 0);

	GLuint buffer_texture;
	glGenTextures(1, &buffer_texture);
	glBindTexture(GL_TEXTURE_2D, buffer_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, buffer.width, buffer.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer.buf);
	texParams();

	GLuint fs_triangle_vao;
	glGenVertexArrays(1, &fs_triangle_vao);

	GLuint shader_id = glCreateProgram();

	{
		GLuint shader_vp = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(shader_vp, 1, &vertex_shader, nullptr);
		glCompileShader(shader_vp);
		validateShader(shader_vp, vertex_shader);
		glAttachShader(shader_id, shader_vp);

		glDeleteShader(shader_vp);
	}

	{
		GLuint shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(shader_fp, 1, &fragment_shader, nullptr);
		glCompileShader(shader_fp);
		validateShader(shader_fp, fragment_shader);
		glAttachShader(shader_id, shader_fp);

		glDeleteShader(shader_fp);
	}

	glLinkProgram(shader_id);

	if(!validateProgram(shader_id)){
		fprintf(stderr, "Error while validating shader.\n");
		glfwTerminate();
		glDeleteVertexArrays(1, &fs_triangle_vao);
		delete[] buffer.buf;
		return -1;
	}

	glUseProgram(shader_id);

	GLint location = glGetUniformLocation(shader_id, "buffer");
	glUniform1i(location, 0);

	StateManager::disableDepthTest();
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(fs_triangle_vao);

	sprite alien_sprites[6];

	alien_sprites[0].width = 8;
	alien_sprites[0].height = 8;
	// Lil tater
	alien_sprites[0].buf = new uint8_t[64]
			{
					0,1,1,1,1,1,1,0, // .@@@@@@.
					0,1,1,1,1,1,1,0, // .@@@@@@.
					0,1,0,1,1,0,1,0, // .@.@@.@.
					0,1,1,1,1,1,1,0, // .@@@@@@.
					0,1,1,1,1,1,1,0, // .@.@@.@.
					0,1,0,1,1,0,1,0, // .@@..@@.
					0,1,1,0,0,1,1,0, // .@@@@@@.
					0,1,1,1,1,1,1,0  // ........
			};

	alien_sprites[1].width = 8;
	alien_sprites[1].height = 8;
	// Lil tater
	alien_sprites[1].buf = new uint8_t[64]
			{
					0,1,1,1,1,1,1,0, // .@@@@@@.
					0,1,1,1,1,1,1,0, // .@@@@@@.
					0,1,0,1,1,0,1,0, // .@.@@.@.
					0,1,1,1,1,1,1,0, // .@@@@@@.
					0,1,1,1,1,1,1,0, // .@.@@.@.
					0,1,0,1,1,0,1,0, // .@@..@@.
					0,1,1,0,0,1,1,0, // .@@@@@@.
					0,1,1,1,1,1,1,0  // ........
			};

	alien_sprites[2].width = 11;
	alien_sprites[2].height = 8;
	// Lil tater
	alien_sprites[2].buf = new uint8_t[88]
			{
					0,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@.
					0,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@.
					0,1,1,0,1,1,1,0,1,1,0, // .@@.@@@.@@.
					0,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@.
					0,1,1,0,1,1,1,0,1,1,0, // .@@.@@@.@@.
					0,0,1,1,0,0,0,1,1,0,0, // .@@@...@@@.
					0,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@.
					0,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@.
			};

	alien_sprites[3].width = 11;
	alien_sprites[3].height = 8;
	// Lil tater
	alien_sprites[3].buf = new uint8_t[88]
			{
					0,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@.
					0,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@.
					0,1,1,0,1,1,1,0,1,1,0, // .@@.@@@.@@.
					0,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@.
					0,1,1,0,1,1,1,0,1,1,0, // .@@.@@@.@@.
					0,0,1,1,0,0,0,1,1,0,0, // .@@@...@@@.
					0,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@.
					0,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@.
			};

	alien_sprites[4].width = 12;
	alien_sprites[4].height = 8;
	// Lil tater
	alien_sprites[4].buf = new uint8_t[96]
			{
					0,0,0,0,0,0,0,0,0,0,0,0, // ............
					0,0,1,1,1,1,1,1,1,1,0,0, // ..@@@@@@@@..
					0,0,1,0,1,1,1,1,0,1,0,0, // ..@.@@@@.@..
					0,0,1,1,1,1,1,1,1,1,0,0, // ..@@@@@@@@..
					0,0,1,0,1,1,1,1,0,1,0,0, // ..@.@@@@.@..
					0,0,1,1,0,0,0,0,1,1,0,0, // ..@@....@@..
					0,0,1,1,1,1,1,1,1,1,0,0, // ..@@@@@@@@..
					0,0,0,0,0,0,0,0,0,0,0,0  // ............
			};


	alien_sprites[5].width = 12;
	alien_sprites[5].height = 8;
	// Lil tater
	alien_sprites[5].buf = new uint8_t[96]
			{
					0,0,0,0,0,0,0,0,0,0,0,0, // ............
					0,0,1,1,1,1,1,1,1,1,0,0, // ..@@@@@@@@..
					0,0,1,0,1,1,1,1,0,1,0,0, // ..@.@@@@.@..
					0,0,1,1,1,1,1,1,1,1,0,0, // ..@@@@@@@@..
					0,0,1,0,1,1,1,1,0,1,0,0, // ..@.@@@@.@..
					0,0,1,1,0,0,0,0,1,1,0,0, // ..@@....@@..
					0,0,1,1,1,1,1,1,1,1,0,0, // ..@@@@@@@@..
					0,0,0,0,0,0,0,0,0,0,0,0  // ............
			};

	sprite alien_death_sprite{};
	alien_death_sprite.width = 13;
	alien_death_sprite.height = 7;
	// Alien death
	alien_death_sprite.buf = new uint8_t[91]
			{
					0,1,0,0,1,0,0,0,1,0,0,1,0, // .@..@...@..@.
					0,0,1,0,0,1,0,1,0,0,1,0,0, // ..@..@.@..@..
					0,0,0,1,0,0,0,0,0,1,0,0,0, // ...@.....@...
					1,1,0,0,0,0,0,0,0,0,0,1,1, // @@.........@@
					0,0,0,1,0,0,0,0,0,1,0,0,0, // ...@.....@...
					0,0,1,0,0,1,0,1,0,0,1,0,0, // ..@..@.@..@..
					0,1,0,0,1,0,0,0,1,0,0,1,0  // .@..@...@..@.
			};

	sprite player_sprite{};
	player_sprite.width = 11;
	player_sprite.height = 7;
	// Player
	player_sprite.buf = new uint8_t[77]
			{
					0,0,0,0,0,1,0,0,0,0,0,
					0,0,0,0,1,1,1,0,0,0,0,
					0,0,0,0,1,1,1,0,0,0,0,
					0,1,1,1,1,1,1,1,1,1,0,
					1,1,1,1,1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,1,1,1,1,
			};


	sprite text_spritesheet{};
	text_spritesheet.width = 5;
	text_spritesheet.height = 7;
	text_spritesheet.buf = new uint8_t[65 * 35]
			{
					0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
					0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,
					0,1,0,1,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
					0,1,0,1,0,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,0,1,0,1,0,
					0,0,1,0,0,0,1,1,1,0,1,0,1,0,0,0,1,1,1,0,0,0,1,0,1,0,1,1,1,0,0,0,1,0,0,
					1,1,0,1,0,1,1,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,1,1,0,1,0,1,1,
					0,1,1,0,0,1,0,0,1,0,1,0,0,1,0,0,1,1,0,0,1,0,0,1,0,1,0,0,0,1,0,1,1,1,1,
					0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,
					1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,
					0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,0,1,0,0,0,1,1,1,0,1,0,1,0,1,0,0,1,0,0,
					0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,1,1,1,1,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,
					0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,
					0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,

					0,1,1,1,0,1,0,0,0,1,1,0,0,1,1,1,0,1,0,1,1,1,0,0,1,1,0,0,0,1,0,1,1,1,0,
					0,0,1,0,0,0,1,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,1,1,0,
					0,1,1,1,0,1,0,0,0,1,0,0,0,0,1,0,0,1,1,0,0,1,0,0,0,1,0,0,0,0,1,1,1,1,1,
					1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
					0,0,0,1,0,0,0,1,1,0,0,1,0,1,0,1,0,0,1,0,1,1,1,1,1,0,0,0,1,0,0,0,0,1,0,
					1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
					0,1,1,1,0,1,0,0,0,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
					1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,
					0,1,1,1,0,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
					0,1,1,1,0,1,0,0,0,1,1,0,0,0,1,0,1,1,1,1,0,0,0,0,1,1,0,0,0,1,0,1,1,1,0,

					0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,
					0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,
					0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
					1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,
					0,1,1,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,
					0,1,1,1,0,1,0,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,0,1,0,0,0,1,0,1,1,1,0,

					0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,
					1,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,1,1,1,0,
					0,1,1,1,0,1,0,0,0,1,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,1,1,1,0,
					1,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,0,
					1,1,1,1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,
					1,1,1,1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,
					0,1,1,1,0,1,0,0,0,1,1,0,0,0,0,1,0,1,1,1,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
					1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,
					0,1,1,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,1,1,0,
					0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
					1,0,0,0,1,1,0,0,1,0,1,0,1,0,0,1,1,0,0,0,1,0,1,0,0,1,0,0,1,0,1,0,0,0,1,
					1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,
					1,0,0,0,1,1,1,0,1,1,1,0,1,0,1,1,0,1,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,
					1,0,0,0,1,1,0,0,0,1,1,1,0,0,1,1,0,1,0,1,1,0,0,1,1,1,0,0,0,1,1,0,0,0,1,
					0,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
					1,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,
					0,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,1,0,1,1,0,0,1,1,0,1,1,1,1,
					1,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,1,1,1,0,1,0,1,0,0,1,0,0,1,0,1,0,0,0,1,
					0,1,1,1,0,1,0,0,0,1,1,0,0,0,0,0,1,1,1,0,1,0,0,0,1,0,0,0,0,1,0,1,1,1,0,
					1,1,1,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,
					1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
					1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,
					1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,1,0,1,1,0,1,0,1,1,1,0,1,1,1,0,0,0,1,
					1,0,0,0,1,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,1,0,0,0,1,
					1,0,0,0,1,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,
					1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,1,1,1,1,

					0,0,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,1,
					0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,
					1,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,1,1,0,0,0,
					0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
					0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
			};

	sprite number_spritesheet = text_spritesheet;
	number_spritesheet.buf += 16 * 35;

	sprite player_bullet_sprite{};
	player_bullet_sprite.width = 1;
	player_bullet_sprite.height = 3;
	player_bullet_sprite.buf = new uint8_t[3]
			{
					1, 1, 1
			};

	sprite alien_bullet_sprite[2];
	alien_bullet_sprite[0].width = 3;
	alien_bullet_sprite[0].height = 7;
	alien_bullet_sprite[0].buf = new uint8_t[21]
			{
					0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,
			};

	alien_bullet_sprite[1].width = 3;
	alien_bullet_sprite[1].height = 7;
	alien_bullet_sprite[1].buf = new uint8_t[21]
			{
					0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,
			};


	dynamic_sprite alien_bullet_animation{};
	alien_bullet_animation.loop = true;
	alien_bullet_animation.num_frames = 2;
	alien_bullet_animation.frame_duration = 5;
	alien_bullet_animation.time = 0;

	alien_bullet_animation.frames = new sprite*[2];
	alien_bullet_animation.frames[0] = &alien_bullet_sprite[0];
	alien_bullet_animation.frames[1] = &alien_bullet_sprite[1];

	dynamic_sprite alien_animation[3];

	size_t alien_update_frequency = 120;

	for(size_t i = 0; i < 3; ++i)
	{
		alien_animation[i].loop = true;
		alien_animation[i].num_frames = 2;
		alien_animation[i].frame_duration = alien_update_frequency;
		alien_animation[i].time = 0;

		alien_animation[i].frames = new sprite*[2];
		alien_animation[i].frames[0] = &alien_sprites[2 * i];
		alien_animation[i].frames[1] = &alien_sprites[2 * i + 1];
	}

	game_data game{};
	game.width = buffer_width;
	game.height = buffer_height;
	game.num_bullets = 0;
	game.num_aliens = 55;
	game.aliens = new tater[game.num_aliens];

	game.player.x = 112 - 5;
	game.player.y = 32;

	game.player.life = 3;

	size_t alien_swarm_position = 24;
	size_t alien_swarm_max_position = game.width - 16 * 11 - 3;

	size_t aliens_killed = 0;
	size_t alien_update_timer = 0;
	bool should_change_speed = false;

	for(size_t xi = 0; xi < 11; ++xi) {
		for(size_t yi = 0; yi < 5; ++yi) {
			tater& alien = game.aliens[xi * 5 + yi];
			alien.type = (5 - yi) / 2 + 1;

			const sprite& sprite = alien_sprites[2 * (alien.type - 1)];

			alien.x = 16 * xi + alien_swarm_position + (alien_death_sprite.width - sprite.width)/2;
			alien.y = 17 * yi + 128;
		}
	}

	auto* death_counters = new uint8_t[game.num_aliens];
	for(size_t i = 0; i < game.num_aliens; ++i) {
		death_counters[i] = 10;
	}

	uint32_t clear_color = pack_rgb(0, 0, 0);
	uint32_t rng = 13;

	int alien_move_dir = 4;

	size_t score = 0;
	size_t credits = 0;

	running = true;

	int player_move_dir = 0;
	while (!glfwWindowShouldClose(window) && running) {
		clearBuffer(&buffer, clear_color);

		if(game.player.life == 0) {

			blitText(&buffer, text_spritesheet, "GAME OVER", game . width / 2 - 30, game . height / 2,
					 pack_rgb(255, 255, 255));
			blitText(&buffer, text_spritesheet, "SCORE", 4, game . height - text_spritesheet . height - 7,
					 pack_rgb(255, 255, 255));
			blitNumber(&buffer, number_spritesheet, score, 4 + 2 * number_spritesheet . width,
					   game . height - 2 * number_spritesheet . height - 12, pack_rgb(255, 255, 255));

			glTexSubImage2D(
					GL_TEXTURE_2D, 0, 0, 0,
					buffer.width, buffer.height,
					GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
					buffer.buf
			);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			glfwSwapBuffers(window);
			glfwPollEvents();
			continue;
		}

		blitText(&buffer, text_spritesheet, "SCORE", 4, game . height - text_spritesheet . height - 7,
				 pack_rgb(255, 255, 255));
		blitNumber(&buffer, number_spritesheet, score, 4 + 2 * number_spritesheet . width,
				   game . height - 2 * number_spritesheet . height - 12, pack_rgb(255, 255, 255));

		{
			char credit_text[16];
			sprintf(credit_text, "CREDIT %02lu", credits);
			blitText(&buffer, text_spritesheet, credit_text, 164, 7, pack_rgb(255, 255, 255));
		}

		blitNumber(&buffer, number_spritesheet, game . player . life, 4, 7, pack_rgb(255, 255, 255));
		size_t xp =  11 + number_spritesheet.width;
		for(size_t i = 0; i < game.player.life - 1; ++i) {
			blit(&buffer, player_sprite, xp, 7, pack_rgb(255, 255, 255));
			xp += player_sprite.width + 2;
		}

		for(size_t i = 0; i < game.width; ++i) {
			buffer.buf[game.width * 16 + i] = pack_rgb(255, 255, 255);
		}


		for(size_t ai = 0; ai < game.num_aliens; ++ai) {
			if(death_counters[ai] == 0) {
				continue;
			}

			const tater& alien = game.aliens[ai];
			if(alien.type == dead_tater) {
				blit(&buffer, alien_death_sprite, alien.x, alien.y, pack_rgb(255, 255, 255));
			} else {
				const dynamic_sprite& animation = alien_animation[alien.type - 1];
				size_t current_frame = animation.time / animation.frame_duration;
				const sprite& sprite = *animation.frames[current_frame];
				blit(&buffer, sprite, alien.x, alien.y, pack_rgb(255, 255, 255));
			}
		}

		for(size_t bi = 0; bi < game.num_bullets; ++bi) {
			const bullet& bullet = game.bullets[bi];
			const sprite* sprite;
			if(bullet.dir > 0) {
				sprite = &player_bullet_sprite;
			} else {
				size_t cf = alien_bullet_animation.time / alien_bullet_animation.frame_duration;
				sprite = &alien_bullet_sprite[cf];
			}
			blit(&buffer, *sprite, bullet.x, bullet.y, pack_rgb(255, 255, 255));
		}

		blit(&buffer, player_sprite, game.player.x, game.player.y, pack_rgb(255, 255, 255));

		glTexSubImage2D(
				GL_TEXTURE_2D, 0, 0, 0,
				buffer.width, buffer.height,
				GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
				buffer.buf
		);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glfwSwapBuffers(window);

		for(size_t bi = 0; bi < game.num_bullets; ++bi) {
			game.bullets[bi].y += game.bullets[bi].dir;
			if(game.bullets[bi].y >= game.height || game.bullets[bi].y < player_bullet_sprite.height) {
				game.bullets[bi] = game.bullets[game.num_bullets - 1];
				--game.num_bullets;
				continue;
			}

			if(game.bullets[bi].dir < 0) {
				bool overlap = checkOverlap(
						alien_bullet_sprite[0], game . bullets[bi] . x, game . bullets[bi] . y,
						player_sprite, game . player . x, game . player . y
				);

				if(overlap) {
					--game.player.life;
					game.bullets[bi] = game.bullets[game.num_bullets - 1];
					--game.num_bullets;
					break;
				}
			} else {
				for(size_t bj = 0; bj < game.num_bullets; ++bj) {
					if(bi == bj) {
						continue;
					}

					bool overlap = checkOverlap(
							player_bullet_sprite, game . bullets[bi] . x, game . bullets[bi] . y,
							alien_bullet_sprite[0], game . bullets[bj] . x, game . bullets[bj] . y
					);

					if(overlap) {
						if(bj == game.num_bullets - 1) {
							game.bullets[bi] = game.bullets[game.num_bullets - 2];
						} else if(bi == game.num_bullets - 1) {
							game.bullets[bj] = game.bullets[game.num_bullets - 2];
						} else {
							game.bullets[(bi < bj)? bi: bj] = game.bullets[game.num_bullets - 1];
							game.bullets[(bi < bj)? bj: bi] = game.bullets[game.num_bullets - 2];
						}
						game.num_bullets -= 2;
						break;
					}
				}

				for(size_t ai = 0; ai < game.num_aliens; ++ai) {
					const tater& alien = game.aliens[ai];
					if(alien.type == dead_tater) continue;

					const dynamic_sprite& animation = alien_animation[alien.type - 1];
					size_t current_frame = animation.time / animation.frame_duration;
					const sprite& alien_sprite = *animation.frames[current_frame];
					bool overlap = checkOverlap(
							player_bullet_sprite, game . bullets[bi] . x, game . bullets[bi] . y,
							alien_sprite, alien . x, alien . y
					);

					if(overlap) {
						score += 10 * (4 - game.aliens[ai].type);
						game.aliens[ai].type = dead_tater;
						game.aliens[ai].x -= (alien_death_sprite.width - alien_sprite.width)/2;
						game.bullets[bi] = game.bullets[game.num_bullets - 1];
						--game.num_bullets;
						++aliens_killed;

						if(aliens_killed % 15 == 0) should_change_speed = true;

						break;
					}
				}
			}
		}

		if(should_change_speed) {
			should_change_speed = false;
			alien_update_frequency /= 2;

			for(auto & i : alien_animation) {
				i.frame_duration = alien_update_frequency;
			}
		}

		for(size_t ai = 0; ai < game.num_aliens; ++ai) {
			const tater& alien = game.aliens[ai];

			if(alien.type == dead_tater && death_counters[ai]) {
				--death_counters[ai];
			}
		}

		if(alien_update_timer >= alien_update_frequency) {
			alien_update_timer = 0;

			if((int) alien_swarm_position + alien_move_dir < 0) {
				alien_move_dir *= -1;
				for(size_t ai = 0; ai < game.num_aliens; ++ai) {
					tater& alien = game.aliens[ai];
					alien.y -= 8;
				}
			} else if(alien_swarm_position > alien_swarm_max_position - alien_move_dir) {
				alien_move_dir *= -1;
			}
			alien_swarm_position += alien_move_dir;

			for(size_t ai = 0; ai < game.num_aliens; ++ai) {
				tater& alien = game.aliens[ai];
				alien.x += alien_move_dir;
			}

			if(aliens_killed < game.num_aliens) {
				size_t rai = game.num_aliens * random(&rng);
				while(game.aliens[rai].type == dead_tater) {
					rai = game.num_aliens * random(&rng);
				}
				const sprite& alien_sprite = *alien_animation[game.aliens[rai].type - 1].frames[0];
				game.bullets[game.num_bullets].x = game.aliens[rai].x + alien_sprite.width / 2;
				game.bullets[game.num_bullets].y = game.aliens[rai].y - alien_bullet_sprite[0].height;
				game.bullets[game.num_bullets].dir = -2;
				++game.num_bullets;
			}
		}

		for(auto & i : alien_animation) {
			++i.time;
			if(i.time >= i.num_frames * i.frame_duration) {
				i.time = 0;
			}
		}

		++alien_bullet_animation.time;

		if(alien_bullet_animation.time >= alien_bullet_animation.num_frames * alien_bullet_animation.frame_duration) {
			alien_bullet_animation.time = 0;
		}

		++alien_update_timer;

		// Simulate player
		player_move_dir = 2 * move_dir;

		if(player_move_dir != 0) {
			if(game.player.x + player_sprite.width + player_move_dir >= game.width) {
				game.player.x = game.width - player_sprite.width;
			} else if((int)game.player.x + player_move_dir <= 0) {
				game.player.x = 0;
			}
			else game.player.x += player_move_dir;
		}

		if(aliens_killed < game.num_aliens) {
			size_t ai = 0;
			while(game.aliens[ai].type == dead_tater) ++ai;
			const sprite& sprite = alien_sprites[2 * (game.aliens[ai].type - 1)];
			size_t pos = game.aliens[ai].x - (alien_death_sprite.width - sprite.width)/2;
			if(pos > alien_swarm_position) alien_swarm_position = pos;

			ai = game.num_aliens - 1;
			while(game.aliens[ai].type == dead_tater) --ai;
			pos = game.width - game.aliens[ai].x - 13 + pos;
			if(pos > alien_swarm_max_position) alien_swarm_max_position = pos;
		} else {
			alien_update_frequency = 120;
			alien_swarm_position = 24;

			aliens_killed = 0;
			alien_update_timer = 0;

			alien_move_dir = 4;

			for(size_t xi = 0; xi < 11; ++xi) {
				for(size_t yi = 0; yi < 5; ++yi) {
					size_t ai = xi * 5 + yi;

					death_counters[ai] = 10;

					tater& alien = game.aliens[ai];
					alien.type = (5 - yi) / 2 + 1;

					const sprite& sprite = alien_sprites[2 * (alien.type - 1)];

					alien.x = 16 * xi + alien_swarm_position + (alien_death_sprite.width - sprite.width)/2;
					alien.y = 17 * yi + 128;
				}
			}
		}

		// Process events
		if(pressed && game.num_bullets < 180) {
			game.bullets[game.num_bullets].x = game.player.x + player_sprite.width / 2;
			game.bullets[game.num_bullets].y = game.player.y + player_sprite.height;
			game.bullets[game.num_bullets].dir = 2;
			++game.num_bullets;
		}
		pressed = false;

		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	glDeleteVertexArrays(1, &fs_triangle_vao);

	for(auto & alien_sprite : alien_sprites) {
		delete[] alien_sprite.buf;
	}

	delete[] text_spritesheet.buf;
	delete[] alien_death_sprite.buf;
	delete[] player_bullet_sprite.buf;
	delete[] alien_bullet_sprite[0].buf;
	delete[] alien_bullet_sprite[1].buf;
	delete[] alien_bullet_animation.frames;

	for(auto & i : alien_animation) {
		delete[] i.frames;
	}
	delete[] buffer.buf;
	delete[] game.aliens;
	delete[] death_counters;

	return 0;
}

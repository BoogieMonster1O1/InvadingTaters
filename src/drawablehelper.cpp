#include "drawablehelper.h"

void blit(sprite_buffer* buffer, const sprite& sprite, size_t x, size_t y, uint32_t color) {
	for(size_t xi = 0; xi < sprite.width; ++xi) {
		for(size_t yi = 0; yi < sprite.height; ++yi) {
			if(sprite.buf[yi * sprite.width + xi] && (sprite.height - 1 + y - yi) < buffer->height && (x + xi) < buffer->width) {
				buffer -> buf[(sprite.height - 1 + y - yi) * buffer->width + (x + xi)] = color;
			}
		}
	}
}

void blitNumber(sprite_buffer* buffer, const sprite& number_spritesheet, size_t number, size_t x, size_t y, uint32_t color) {
	uint8_t digits[64];
	size_t num_digits = 0;

	size_t current_number = number;
	do {
		digits[num_digits++] = current_number % 10;
		current_number = current_number / 10;
	} while(current_number > 0);

	size_t xp = x;
	size_t stride = number_spritesheet.width * number_spritesheet.height;
	sprite sprite = number_spritesheet;
	for(size_t i = 0; i < num_digits; ++i) {
		uint8_t digit = digits[num_digits - i - 1];
		sprite.buf = number_spritesheet.buf + digit * stride;
		blit(buffer, sprite, xp, y, color);
		xp += sprite.width + 1;
	}
}

void blitText(sprite_buffer* buffer, const sprite& atlas, const char* text, size_t x, size_t y, uint32_t color) {
	size_t xp = x;
	size_t stride = atlas.width * atlas.height;
	sprite sprite = atlas;
	for(const char* cbuf = text; *cbuf != '\0'; ++cbuf) {
		char character = *cbuf - 32;
		if(character < 0 || character >= 65) continue;

		sprite.buf = atlas.buf + character * stride;
		blit(buffer, sprite, xp, y, color);
		xp += sprite.width + 1;
	}
}

bool checkOverlap(const sprite& sp_a, size_t x_a, size_t y_a, const sprite& sp_b, size_t x_b, size_t y_b) {
	if(x_a < x_b + sp_b.width && x_a + sp_a.width > x_b && y_a < y_b + sp_b.height && y_a + sp_a.height > y_b) {
		return true;
	}

	return false;
}

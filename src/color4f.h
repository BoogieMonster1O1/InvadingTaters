#ifndef INVADINGTATERS_COLOR4F_H
#define INVADINGTATERS_COLOR4F_H

class Color4 {
public:
	float red;
	float green;
	float blue;
	float alpha;

	Color4(float red, float green, float blue, float alpha) {
		this->red = red;
		this->green = green;
		this->blue = blue;
		this->alpha = alpha;
	}
};

#endif

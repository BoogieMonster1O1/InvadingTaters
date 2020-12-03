#ifndef INVADINGTATERS_STATE_MANAGER_H
#define INVADINGTATERS_STATE_MANAGER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "captracker.h"
#include "color4f.h"

class StateManager {
private:
	static CapabilityTracker depthTest;
	static Color4 clearColor4f;

public:
	static void enableDepthTest();

	static void disableDepthTest();

	static void clearColor(float red, float green, float blue, float alpha);
};

#endif

#include "state_manager.h"

CapabilityTracker StateManager::depthTest = CapabilityTracker(GL_DEPTH_TEST, false);
Color4 StateManager::clearColor4f = Color4(0, 0, 0, 0);

void StateManager::enableDepthTest() {
  depthTest.enable();
}

void StateManager::disableDepthTest() {
  depthTest.disable();
}

void StateManager::clearColor(float red, float green, float blue, float alpha) {
  clearColor4f.red = red;
  clearColor4f.green = green;
  clearColor4f.blue = blue;
  clearColor4f.alpha = alpha;
  glClearColor(red, green, blue, alpha);
}

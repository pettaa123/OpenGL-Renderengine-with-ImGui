#include "Monitor.h"

Monitor::Monitor() {
  m = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(m);
  width = mode->width;
  height = mode->height;
}

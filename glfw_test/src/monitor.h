#ifndef MONITOR_H
#define MONITOR_H

#include <GLFW/glfw3.h>

/* Wrapper struct for GLFWmonitor */
struct Monitor {
  Monitor();
  int width;
  int height;
  GLFWmonitor* m;
};

#endif // MONITOR_H

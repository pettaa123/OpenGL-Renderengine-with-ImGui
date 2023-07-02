/*
MIT License

Copyright(c) 2018 ThePhysicsGuys

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#include "keyboard.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <GLFW/glfw3.h>


namespace Engine {

class Event;

class InputHandler {
private:
	void setCallBacks(GLFWwindow* window);

protected:
	GLFWwindow* window;

private:
	bool modifiers[1];
	double timestamp[GLFW_KEY_LAST + 1];
	void keyCallback(int key, int action, int mods);
	void cursorCallback(double x, double y);
	void cursorEnterCallback(int entered);
	void windowDropCallback(const char* path);
	void scrollCallback(double xOffset, double yOffset);
	void mouseButtonCallback(int button, int action, int mods);
	void windowSizeCallback(int width, int height);
	void framebufferSizeCallback(int width, int height);

	double keyInterval = 0.2;
public:
	bool keys[GLFW_KEY_LAST + 1];
	char anyKey = 0;

	bool rightDragging = false;
	bool middleDragging = false;
	bool leftDragging = false;

	glm::vec2 mousePosition;
	glm::vec4 viewport;

	InputHandler(GLFWwindow* window);

	bool getKey(const Key& key) const;
	void setKey(const Key& key, bool pressed);
	glm::vec2 getMousePosition() const;

	virtual void onEvent(Event& event) {};
};

};
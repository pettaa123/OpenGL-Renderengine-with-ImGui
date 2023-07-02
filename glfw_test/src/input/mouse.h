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

#include <string>

namespace Engine {

struct Button {
private:
	std::string name;
	int code;

public:
	inline Button(const std::string& name, int code) : name(name), code(code) {}

	std::string getName() const;
	int getCode() const;

	// Button <-> Button
	bool operator==(const Button& other) const;
	bool operator!=(const Button& other) const;

	// Key <-> int
	bool operator==(int other) const;
	bool operator!=(int other) const;

	// Key <-> string
	bool operator==(const std::string& other) const;
	bool operator!=(const std::string& other) const;
};
							 
namespace Mouse {

	Button getButton(std::string name);
	Button getButton(int code);

	extern const Button LEFT;
	extern const Button RIGHT;
	extern const Button MIDDLE;
	extern const Button MOUSE_4;
	extern const Button MOUSE_5;
	extern const Button MOUSE_6;
	extern const Button MOUSE_7;
	extern const Button MOUSE_8;

	extern const int MOUSE_FIRST;
	extern const int MOUSE_LAST;
	
	extern const int PRESS;
	extern const int RELEASE;
	extern const int REPEAT;

};

};
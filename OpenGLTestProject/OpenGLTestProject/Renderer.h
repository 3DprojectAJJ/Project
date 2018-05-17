#pragma once

#include <GL/glew.h>
#include <iostream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) glClearError();\
	x;\
	ASSERT(glLogCall(#x,__FILE__,__LINE__))

	void glClearError();

	bool glLogCall(const char * function, const char* file, int line);


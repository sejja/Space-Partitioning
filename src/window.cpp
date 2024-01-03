//
//	window.cpp
//	Space Partitioning
//
//	Created by Diego Revilla on 08/09/23
//	Copyright © 2023. All Rights reserved
//

#include "window.hpp"
#include <tlhelp32.h>
#include <WinUser.h>
#include <exception>
#include <stdexcept>
#include <iostream>

// ------------------------------------------------------------------------
/*! Custom Constructor
*
*   Constructs a window with a width, height, name, and if it is visible
*/ // --------------------------------------------------------------------
window::window(int w, int h, const char *window_name, bool visible) {
	int xpos, ypos, height;
	const char* description;

	if (!glfwInit())
	{
		glfwGetError(&description);
		printf("Error: %s\n", description);
		exit(EXIT_FAILURE);
	}

	mWindow = glfwCreateWindow(w, h, window_name, NULL, NULL);
	if (!mWindow)
	{
		glfwGetError(&description);
		printf("Error: %s\n", description);
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(mWindow);
	gladLoadGL();
	glClearColor(0.f, 0.f, 0.f, 1.f);
}

// ------------------------------------------------------------------------
/*! Destructor
*
*   Destroys the window
*/ // --------------------------------------------------------------------
window::~window() {
	destroy();
}

bool window::GetKey(int key) {
	return glfwGetKey(mWindow, key);
}

// ------------------------------------------------------------------------
/*! Update
*
*   Loads a Source for our Shader
*/ // --------------------------------------------------------------------
bool window::update() {
	glfwMakeContextCurrent(mWindow);
	glfwSwapBuffers(mWindow);
	glfwPollEvents();

	if (glfwWindowShouldClose(mWindow) ||
		glfwGetKey(mWindow, GLFW_KEY_ESCAPE))
	{
		glfwTerminate();
		exit(EXIT_SUCCESS);
	}


	glClear(GL_COLOR_BUFFER_BIT);
	return true;
}

void window::destroy() {
	glfwTerminate();
}
/*
 * A C++ framework for OpenGL programming in TNM046 for MT1
 *
 * This is a small, limited framework, designed to be easy to use for students in an introductory
 * computer graphics course in the first year of a M Sc curriculum. It uses custom code for some
 * things that are better solved by external libraries like GLM, but the emphasis is on simplicity
 * andreadability, not generality.
 *
 * For the window management, GLFW 3.x is used for convenience.
 * The framework should work in Windows, MacOS X and Linux.
 * GLEW is used for handling platform specific OpenGL extensions.
 * This code depends only on GLFW, GLEW, and OpenGL libraries.
 * OpenGL 3.3 or higher is required.
 *
 * Authors: Stefan Gustavson (stegu@itn.liu.se) 2013-2015
 *          Martin Falk (martin.falk@liu.se) 2021
 *
 * This code is in the public domain.
 */
#if defined(WIN32) && !defined(_USE_MATH_DEFINES)
#define _USE_MATH_DEFINES
#endif

// File and console I/O for logging and error reporting
#include <iostream>
// Math header for trigonometric functions
#include <cmath>

// glew provides easy access to advanced OpenGL functions and extensions
#include <GL/glew.h>

// GLFW 3.x, to handle the OpenGL window
#include <GLFW/glfw3.h>

#include "Utilities.hpp"

#include <vector>

#include "Shader.hpp"

GLuint createVertexBuffer(int location, int dimensions, const std::vector<float>& vertices) {
    GLuint bufferID;
    glGenBuffers(1, &bufferID);
    // Activate the vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    // Activate the vertex array object
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(location, dimensions, GL_FLOAT, GL_FALSE, 0, nullptr);
    // Enable vertex attribute array 0 to send xyz coordinates to the shader.
    
    glEnableVertexAttribArray(location);

    return bufferID;
}

GLuint createIndexBuffer(const std::vector<unsigned int>& indices) {
    GLuint bufferID;

    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    return bufferID;
}
 
int main(int, char*[]) {
    const std::vector<GLfloat> vertexArrayData = {
        -1.0f, -1.0f, 0.0f, 
        1.0f, -1.0f, 0.0f,  
        0.0f,  1.0f, 0.0f
    };

    const std::vector<GLuint> indexArrayData = {0, 1, 2};

    const std::vector<GLfloat> colorArrayData = {
        1.0f, 0.0f, 0.0f, 
        0.0f, 1.0f, 0.0f, 
        0.0f, 0.0f, 1.0f,
    };

    Shader myShader;

    // Initialise GLFW
    glfwInit();

    const GLFWvidmode* vidmode;  // GLFW struct to hold information about the display
    // Determine the desktop size
    vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    // Make sure we are getting a GL context of at least version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Enable the OpenGL core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Open a square window (aspect 1:1) to fill half the screen height
    GLFWwindow* window =
        glfwCreateWindow(vidmode->height / 2, vidmode->height / 2, "GLprimer", nullptr, nullptr);
    if (!window) {
        std::cout << "Unable to open window. Terminating.\n";
        glfwTerminate();  // No window was opened, so we can't continue in any useful way
        return -1;
    }

    // Make the newly created window the "current context" for OpenGL
    // (This step is strictly required or things will simply not work)
    glfwMakeContextCurrent(window);

    // Initialize glew
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << "\n";
        glfwTerminate();
        return -1;
    }

    GLuint vertexArrayID = 0;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    GLuint vertexBufferID = createVertexBuffer(0, 3, vertexArrayData);
    GLuint colorBufferID = createVertexBuffer(1, 3, colorArrayData);
    GLuint indexBufferID = createIndexBuffer(indexArrayData);

    glBindVertexArray(0);

    myShader.createShader("vertex.glsl", "fragment.glsl");

    // Show some useful information on the GL context
    std::cout << "GL vendor:       " << glGetString(GL_VENDOR)
              << "\nGL renderer:     " << glGetString(GL_RENDERER)
              << "\nGL version:      " << glGetString(GL_VERSION)
              << "\nDesktop size:    " << vidmode->width << " x " << vidmode->height << "\n";

    // Get window size. It may start out different from the requested size and
    // will change if the user resizes the window
    int width, height;

    glfwSwapInterval(0);  // Do not wait for screen refresh between frames

    // Main loop
    while (!glfwWindowShouldClose(window)) {

        util::displayFPS(window);
        glfwGetWindowSize(window, &width, &height);
        // Set viewport. This is the pixel rectangle we want to draw into
        glViewport(0, 0, width, height);  // The entire window
        // Set the clear color to a dark gray (RGBA)
        glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
        // Clear the color and depth buffers for drawing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* ---- Rendering code should go here ---- */
        glUseProgram(myShader.id());
        glBindVertexArray(vertexArrayID);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

        // Swap buffers, display the image and prepare for next frame
        glfwSwapBuffers(window);

        // Poll events (read keyboard and mouse input)
        glfwPollEvents();

        // Exit if the ESC key is pressed (and also if the window is closed)
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteBuffers(1, &indexBufferID);

    // Close the OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}

#include <GLEnv.h>
#include <Mat4.h>
#include <GLAppKeyTranslation.h>

#include <iostream>
#include <sstream>

GLFWwindow* window;
GLuint vbo;
GLuint vao;
GLuint program;

// triangle vertex position data
const float triangle[] = {
	 0.0f,  0.5f, 0.0f, // top
	-0.5f, -0.5f, 0.0f,	// bottom left
	 0.5f, -0.5f, 0.0f  // bottom right
};

// vertex shader code
const char* vertexShaderSource =
"#version 410 core\n"
"layout(location = 0) in vec3 vPos;\n"
"void main()\n"
"{\n"
"  gl_Position = vec4(vPos, 1.0);\n"
"}\0";

// fragment shader code
const char* fragmentShaderSource =
"#version 410 core\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"  fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0);\n"
"}\0";


static void draw() {
  GL( glClearColor(0.0f, 0.0f, 0.0f, 1.0f) );
  GL( glClear(GL_COLOR_BUFFER_BIT) );

  GL( glBindVertexArray(vao) );
  GL( glUseProgram(program) );
  GL( glDrawArrays(GL_TRIANGLES, 0, 3) );
  GL( glBindVertexArray(0) );
}

static void setupShaders() {
  // create the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  GL( glShaderSource(vertexShader, 1, &vertexShaderSource, NULL) );
  GL( glCompileShader(vertexShader) );
  checkAndThrowShader(vertexShader);
  
  // create the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  GL( glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL) );
  GL( glCompileShader(fragmentShader) );
  checkAndThrowShader(fragmentShader);
  
  // link shaders into program
  program = glCreateProgram();
  GL( glAttachShader(program, vertexShader) );
  GL( glAttachShader(program, fragmentShader) );
  GL( glLinkProgram(program) );
  checkAndThrowProgram(program);
  
  GL( glDeleteShader(vertexShader) );
  GL( glDeleteShader(fragmentShader) );
}

static void setupGeometry() {
  // define VAO for triangle
  GL( glGenVertexArrays(1, &vao) );
  GL( glBindVertexArray(vao) );
  
  // upload vertex positions to VBO
  GL( glGenBuffers(1, &vbo) );
  GL( glBindBuffer(GL_ARRAY_BUFFER, vbo) );
  GL( glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW) );
  
  GL( glEnableVertexAttribArray(0) );
  GL( glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0) );
  
  GL( glBindVertexArray(0) );
}

#ifndef __EMSCRIPTEN__
static void keyCallback(GLFWwindow* window, int key, int scancode, int action,
  int mods) {
    if (glfwGetKey(window, GLENV_KEY_ESCAPE) == GLENV_PRESS)
      glfwSetWindowShouldClose(window, true);
  }

static void sizeCallback(GLFWwindow* window, int width, int height) {
  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  GL( glViewport(0, 0, w, h) );
}

#endif

int main(int argc, char** argv) {
#ifdef __EMSCRIPTEN__
  GLEnv glEnv{800,600,1,"My First OpenGL Program",true,false,3,0,true};
#else
  GLEnv glEnv{800,600,1,"My First OpenGL Program",true,false,4,1,true};
  glEnv.setKeyCallback(keyCallback);
  glEnv.setResizeCallback(sizeCallback);
#endif
  
  setupShaders();
  setupGeometry();
  
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(draw, nullptr, 0, 1);
#else
  while (!glEnv.shouldClose()) {
    draw();
    glEnv.endOfFrame();
  }
#endif
  
  return EXIT_SUCCESS;
}

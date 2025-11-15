#include <fstream>

#include <GLApp.h>

class MyGLApp : public GLApp {
private:
  const float degreePreSecond{ 45.0f };
  float angle{0.0f};
  bool playAnimation = true;
  float animTime, animDelta, lastAnimationTime;

  Mat4 projection{};
  Mat4 rotationX{};
  Mat4 rotationY{};
  Mat4 rotationZ{};
  Mat4 toOrigin{};
  Mat4 toTop{};
  Mat4 toLeft{};
  Mat4 toRight{};
  Mat4 scaling{};
  GLuint program{0};
  GLint modelViewMatrixUniform{-1};
  GLint projectionMatrixUniform{-1};
  GLint animationMatrixUniform{-1};
  GLint toOriginMatrixUniform{-1};
  GLint toPosMatrixUniform{-1};
  GLint scalingMatrixUniform{-1};
  GLuint vbos{0};
  GLuint vaos{0};

  constexpr static float sqrt3{ 1.7320508076f };

  constexpr static GLfloat triangles[] = {
     0.5f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f,   // top->top
     0.0f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f,   // top->left
     1.0f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f,   // top->right

     0.5f,  1.0f,  0.0f, 1.0f, 0.0f, 1.0f,  // left->top
     0.0f,  0.0f,  0.0f, 0.0f, 0.0f, 1.0f,  // left->left
     1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f,  // left->right

     0.5f,  1.0f,  0.0f, 1.0f, 1.0f, 0.0f,  // right->top
     0.0f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f,  // right->left
     1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  // right->right
  };

public:
  MyGLApp()
    : GLApp(800,600,4,"Assignment 02 - Triforce")
  {}
  
  virtual void init() override {
    setupShaders();
    setupGeometry();
    GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
  }

  virtual void animate(double animationTime) override {
    animDelta = animationTime - lastAnimationTime;

    if (playAnimation) {
      animTime += animDelta;
    }
    
    toOrigin = Mat4::translation(-0.5f, 0.5f, 0.0f);
    rotationX = Mat4::rotationX(degreePreSecond * animTime);
    rotationY = Mat4::rotationY(degreePreSecond * animTime);
    rotationZ = Mat4::rotationZ(-degreePreSecond * animTime);
    
    GL(glUseProgram(program));
    GL(glUniformMatrix4fv(toOriginMatrixUniform, 1, GL_TRUE, toOrigin));
    GL(glUseProgram(0));  

    lastAnimationTime = animationTime;
  }

  void toggleAnimation() {
    playAnimation = !playAnimation;
  }

  void resetAnimation() {
    animTime = 0.0f;
  }

  virtual void draw() override {
    GL(glClear(GL_COLOR_BUFFER_BIT));

    GL(glUseProgram(program));
    Mat4 modelView = Mat4::translation(0.0f, -1.0f, 0.0f);
    GL(glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_TRUE, modelView));
    
    GL(glBindVertexArray(vaos));
    
    scaling = Mat4::scaling(1.75f);
    // top triangle
    toTop = Mat4::translation(0.0f, 0.5f, 0.0f);
    GL(glUniformMatrix4fv(toPosMatrixUniform, 1, GL_TRUE, toTop));
    GL(glUniformMatrix4fv(animationMatrixUniform, 1, GL_TRUE, rotationX));
    GL(glUniformMatrix4fv(scalingMatrixUniform, 1, GL_TRUE, scaling));
    GL(glDrawArrays(GL_TRIANGLES, 0, 3));
    
    // left triangle
    toLeft = Mat4::translation(-0.375f, -0.5f, 0.0f);
    GL(glUniformMatrix4fv(toPosMatrixUniform, 1, GL_TRUE, toLeft));
    GL(glUniformMatrix4fv(animationMatrixUniform, 1, GL_TRUE, rotationZ));
    GL(glUniformMatrix4fv(scalingMatrixUniform, 1, GL_TRUE, scaling));
    GL(glDrawArrays(GL_TRIANGLES, 3, 3));
    
    // right triangle
    toRight = Mat4::translation(0.375f, -0.5f, 0.0f);
    GL(glUniformMatrix4fv(toPosMatrixUniform, 1, GL_TRUE, toRight));
    GL(glUniformMatrix4fv(animationMatrixUniform, 1, GL_TRUE, rotationY));
    GL(glUniformMatrix4fv(scalingMatrixUniform, 1, GL_TRUE, scaling));
    GL(glDrawArrays(GL_TRIANGLES, 6, 3));
    
    GL(glBindVertexArray(0));
    GL(glUseProgram(0));
  }
  
  virtual void resize(int width, int height) override {
    const float ratio = static_cast<float>(width) / static_cast<float>(height);

    if (ratio * sqrt3 >= 2)
      projection = Mat4::ortho(-ratio * sqrt3, ratio * sqrt3, -sqrt3,
                               sqrt3, -10.0f, 10.0f);
    else
      projection = Mat4::ortho(-2, 2, -2/ratio,
                               2/ratio, -10.0f, 10.0f);
    GL(glUseProgram(program));
    GL(glUniformMatrix4fv(projectionMatrixUniform, 1, GL_TRUE, projection));
    GL(glUseProgram(0));
    GL(glViewport(0, 0, width, height));
  }

  std::string loadFile(const std::string& filename) {
    std::ifstream shaderFile{ filename };
    if (!shaderFile) {
      throw GLException{ std::string("Unable to open file ") + filename };
    }
    std::string str;
    std::string fileContents;
    while (std::getline(shaderFile, str)) {
      fileContents += str + "\n";
    }
    return fileContents;
  }
  
  GLuint createShaderFromFile(GLenum type, const std::string& sourcePath) {
    const std::string shaderCode = loadFile(sourcePath);
    const GLchar* c_shaderCode = shaderCode.c_str();
    const GLuint s = glCreateShader(type);
    GL(glShaderSource(s, 1, &c_shaderCode, NULL));
    glCompileShader(s); checkAndThrowShader(s);
    return s;
  }
  
  void setupShaders() {
    const std::string vertexSrcPath = "res/shaders/vertexShader.vert";
    const std::string fragmentSrcPath = "res/shaders/fragmentShader.frag";
    const GLuint vertexShader = createShaderFromFile(GL_VERTEX_SHADER, vertexSrcPath);
    const GLuint fragmentShader = createShaderFromFile(GL_FRAGMENT_SHADER, fragmentSrcPath);
    
    program = glCreateProgram();
    GL(glAttachShader(program, vertexShader));
    GL(glAttachShader(program, fragmentShader));
    GL(glLinkProgram(program));
    checkAndThrowProgram(program);
    
    GL(glUseProgram(program));
    modelViewMatrixUniform = glGetUniformLocation(program, "modelViewMatrix");
    projectionMatrixUniform = glGetUniformLocation(program, "projectionMatrix");
    animationMatrixUniform = glGetUniformLocation(program, "animationMatrix");
    toOriginMatrixUniform = glGetUniformLocation(program, "toOriginMatrix");
    toPosMatrixUniform = glGetUniformLocation(program, "toPosMatrix");
    scalingMatrixUniform = glGetUniformLocation(program, "scalingMatrix");
    GL(glUseProgram(0));
  }
  
  void setupGeometry() {
    const GLuint vertexPos = GLuint(glGetAttribLocation(program, "vertexPosition"));
    const GLuint colorPos = GLuint(glGetAttribLocation(program, "vertexColor"));

    GL(glGenVertexArrays(1, &vaos));
    GL(glBindVertexArray(vaos));
    
    GL(glGenBuffers(1, &vbos));
    GL(glBindBuffer(GL_ARRAY_BUFFER, vbos));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles,
                    GL_STATIC_DRAW));
    
    GL(glEnableVertexAttribArray(vertexPos));
    GL(glEnableVertexAttribArray(colorPos));
    GL(glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0));
    GL(glVertexAttribPointer(colorPos, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))));
    
    GL(glBindVertexArray(0););
  }
  
  virtual void keyboard(int key, int scancode, int action, int mods) override
  {
    if (key == GLENV_KEY_ESCAPE && action == GLENV_PRESS)
      closeWindow();
    if (key == GLENV_KEY_SPACE && action == GLENV_PRESS)
      toggleAnimation();
    if (key == GLENV_KEY_R && action == GLENV_PRESS)
      resetAnimation();
  }
};

int main(int argc, char** argv) {
  MyGLApp myApp;
  myApp.run();
  return EXIT_SUCCESS;
}


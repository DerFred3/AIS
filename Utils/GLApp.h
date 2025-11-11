#pragma once

#include <string>

#include "GLEnv.h"
#include "GLProgram.h"
#include "GLArray.h"
#include "GLBuffer.h"
#include "GLTexture2D.h"
#include "Image.h"
#include "GLAppKeyTranslation.h"

/**
 * @file GLApp.h
 * @brief Minimal application framework for running OpenGL or WebGL demos.
 *
 * Provides a small, cross‑platform wrapper around @ref GLEnv (GLFW+GLEW on
 * desktop, Emscripten on the web) and a set of convenience methods to draw
 * points, lines and textured quads, as well as to manage a few built‑in
 * shader programs. Subclass @ref GLApp and override @ref init(), @ref draw(),
 * @ref animate(), and input callbacks to build an interactive sketch.
 *
 * The class also exposes helpers to compute pixel‑correct transforms for images
 * and to upload point‑sprite textures used by @ref drawPoints().
 */

/**
 * @brief Line primitive topology for @ref drawLines().
 */
enum class LineDrawType {
  LIST,  ///< Disconnected line segments (GL_LINES)
  STRIP, ///< Connected polyline (GL_LINE_STRIP)
  LOOP   ///< Closed polyline (GL_LINE_LOOP)
};

/**
 * @brief Triangle primitive topology for @ref drawTriangles().
 */
enum class TrisDrawType {
  LIST,  ///< Independent triangles (GL_TRIANGLES)
  STRIP, ///< Triangle strip (GL_TRIANGLE_STRIP)
  FAN    ///< Triangle fan (GL_TRIANGLE_FAN)
};

/**
 * @brief Simple app runner with a GL context, stock shaders, and draw helpers.
 *
 * Typical usage:
 * @code
 * struct MyApp : GLApp {
 *   void init() override {  load data  }
 *   void draw() override {  issue draws  }
 *   void animate(double t) override {  update state  }
 * };
 * int main(){ MyApp().run(); }
 * @endcode
*/
class GLApp {
public:
  /**
   * @brief Construct the environment and boot the stock shaders/buffers.
   * @param w          Window width in pixels.
   * @param h          Window height in pixels.
   * @param s          Multisample count (GLFW window hint).
   * @param title      Window/page title.
   * @param fpsCounter If true, show an FPS counter in the title.
   * @param sync       If true, enable vsync.
   */
  GLApp(uint32_t w=640, uint32_t h=480, uint32_t s=4,
        const std::string& title = "My OpenGL App",
        bool fpsCounter=true, bool sync=true);
  /** @brief Virtual destructor. */
  virtual ~GLApp();

  /**
   * @brief Create the context/window and enter the render loop.
   *
   * Calls @ref init(), then @ref resize() with the framebuffer size, and
   * finally enters the platform‑specific main loop that repeatedly calls
   * @ref animate() and @ref draw() until the window closes.
   */
  void run();

  /**
   * @brief Enable/disable the animation step inside the main loop.
   * @param animationActive If true, @ref animate() is called each frame.
   *
   * Pausing stores a timestamp so resuming keeps animation time continuous.
   */
  void setAnimation(bool animationActive);
  /** @brief Query whether animation is currently active. */
  bool getAnimation() const;
  /** @brief Reset the animation timer and invoke @ref animate(0). */
  void resetAnimation();

  /**
   * @brief Current window aspect ratio (width/height).
   */
  float getAspect() const;

  /**
   * @brief Update filtering for images drawn by @ref drawImage().
   * @param magFilter GL magnification filter.
   * @param minFilter GL minification filter.
   */
  void setImageFilter(GLint magFilter, GLint minFilter);

  /** @name Rect / textured quad */
  ///@{
  /** @brief Draw a solid‑color rectangle given 2D corners. */
  void drawRect(const Vec4& color, const Vec2& bl, const Vec2& tr);
  /** @brief Draw a solid‑color rectangle given 3D corners (defaults to NDC quad). */
  void drawRect(const Vec4& color,
                const Vec3& bl=Vec3{-1.0f,-1.0f,0.0f},
                const Vec3& br=Vec3{1.0f,-1.0f,0.0f},
                const Vec3& tl=Vec3{-1.0f,1.0f,0.0f},
                const Vec3& tr=Vec3{1.0f,1.0f,0.0f});

  /** @brief Draw a textured quad from a texture handle using 2D corners. */
  void drawImage(const GLTexture2D& image, const Vec2& bl, const Vec2& tr);
  /** @brief Draw a textured quad from an @ref Image using 2D corners. */
  void drawImage(const Image& image, const Vec2& bl, const Vec2& tr);
  /** @brief Draw a textured quad from a texture handle using 3D corners. */
  void drawImage(const GLTexture2D& image,
                 const Vec3& bl=Vec3{-1.0f,-1.0f,0.0f},
                 const Vec3& br=Vec3{1.0f,-1.0f,0.0f},
                 const Vec3& tl=Vec3{-1.0f,1.0f,0.0f},
                 const Vec3& tr=Vec3{1.0f,1.0f,0.0f});
  /** @brief Draw a textured quad from an @ref Image using 3D corners. */
  void drawImage(const Image& image,
                 const Vec3& bl=Vec3{-1.0f,-1.0f,0.0f},
                 const Vec3& br=Vec3{1.0f,-1.0f,0.0f},
                 const Vec3& tl=Vec3{-1.0f,1.0f,0.0f},
                 const Vec3& tr=Vec3{1.0f,1.0f,0.0f});
  ///@}

  /** @name Primitives */
  ///@{
  /**
   * @brief Draw an array of colored points.
   * @param data     Interleaved floats per vertex (x,y,z,r,g,b,a) → 7 floats.
   * @param pointSize Pixel size (ignored on desktop when using @ref simplePointProg).
   * @param useTex   If true, draw as point sprites using the configured textures.
   */
  void drawPoints(const std::vector<float>& data, float pointSize=1.0f, bool useTex=false);

  /**
   * @brief Draw colored lines using the given topology.
   * @param data           Interleaved vertices (x,y,z,r,g,b,a) → 7 floats.
   * @param t              Topology: LIST/STRIP/LOOP.
   * @param lineThickness  If >1, thick lines are triangulated in screen space.
   */
  void drawLines(const std::vector<float>& data, LineDrawType t, float lineThickness=1.0f);

  /**
   * @brief Upload triangle data and draw immediately (or as wireframe edges).
   * @param data      Interleaved floats per vertex; 7 floats for color only
   *                  (x,y,z,r,g,b,a) or 10 floats when lighting is enabled
   *                  (x,y,z,r,g,b,a,nx,ny,nz).
   * @param t         Topology: LIST/STRIP/FAN.
   * @param wireframe If true, edges are derived and drawn as lines.
   * @param lighting  If true, expect normals and use the lighting shader.
   */
  void drawTriangles(const std::vector<float>& data, TrisDrawType t, bool wireframe, bool lighting);
  /** @brief Re‑issue the last triangle draw using cached settings. */
  void redrawTriangles(bool wireframe);
  ///@}

  /** @name Transforms */
  ///@{
  /** @brief Compute a uniform image scale so the longer side fits the window. */
  Mat4 computeImageTransform(const Vec2ui& imageSize) const;
  /** @brief Compute a transform that fits an image to a given height. */
  Mat4 computeImageTransformFixedHeight(const Vec2ui& imageSize,
                                        float height=1.0f,
                                        const Vec3& center=Vec3{0.0f,0.0f,0.0f}) const;
  /** @brief Compute a transform that fits an image to a given width. */
  Mat4 computeImageTransformFixedWidth(const Vec2ui& imageSize,
                                       float width=1.0f,
                                       const Vec3& center=Vec3{0.0f,0.0f,0.0f}) const;
  ///@}

  /** @name View/projection */
  ///@{
  /** @brief Set the projection matrix used by the stock shaders. */
  void setDrawProjection(const Mat4& mat);
  /** @brief Set the model‑view matrix used by the stock shaders. */
  void setDrawTransform(const Mat4& mat);
  /** @brief Current projection matrix. */
  Mat4 getDrawProjection() const;
  /** @brief Current model‑view matrix. */
  Mat4 getDrawTransform() const;
  ///@}

  /** @name Point sprite textures */
  ///@{
  /** @brief Create a default circular mask texture of given resolution. */
  void resetPointTexture(uint32_t resolution=64);
  /** @brief Set the point sprite from raw bytes. */
  void setPointTexture(const std::vector<uint8_t>& shape, uint32_t x,
                       uint32_t y, uint8_t components);
  /** @brief Set the point sprite from an @ref Image. */
  void setPointTexture(const Image& shape);
  /** @brief Set an optional highlight overlay sprite from an @ref Image. */
  void setPointHighlightTexture(const Image& shape);
  /** @brief Clear the highlight overlay sprite. */
  void resetPointHighlightTexture();
  ///@}

  // ===== Lifecycle hooks (override in subclasses) =====
  /** @brief One‑time initialization hook (resources, buffers, etc.). */
  virtual void init() {}
  /** @brief Per‑frame draw hook. */
  virtual void draw() {}
  /** @brief Per‑frame animation/update hook; parameter is seconds since start. */
  virtual void animate(double animationTime) {}
  /** @brief Resize notification; override to update projection, etc. */
  virtual void resize(int width, int height);

  // ===== Input hooks (override in subclasses) =====
  /** @brief Keyboard event; parameters mirror backend callbacks. */
  virtual void keyboard(int key, int scancode, int action, int mods) {}
  /** @brief Unicode character input. */
  virtual void keyboardChar(unsigned int key) {}
  /** @brief Mouse move event (window coordinates). */
  virtual void mouseMove(double xPosition, double yPosition) {}
  /** @brief Mouse button event with cursor position. */
  virtual void mouseButton(int button, int state, int mods, double xPosition, double yPosition) {}
  /** @brief Mouse wheel/trackpad scroll event with cursor position. */
  virtual void mouseWheel(double x_offset, double y_offset, double xPosition, double yPosition) {}

protected:
  GLEnv glEnv;                 ///< Window/context + platform utilities.
  Mat4 p;                      ///< Projection matrix used by stock shaders.
  Mat4 mv;                     ///< Model‑view matrix used by stock shaders.
  Mat4 mvi;                    ///< Inverse of @ref mv (for lighting helpers).
  GLProgram simpleProg;        ///< Solid color shader (pos+color).
  GLProgram simplePointProg;   ///< Point shader (pos+color), desktop uses glPointSize.
  GLProgram simpleSpriteProg;  ///< Point sprite shader (pos+color+texture).
  GLProgram simpleHLSpriteProg;///< Point sprite + highlight overlay.
  GLProgram simpleTexProg;     ///< Textured quad shader (pos+uv).
  GLProgram simpleLightProg;   ///< Lit shader (pos+color+normal).
  GLArray simpleArray;         ///< VAO used by helpers.
  GLBuffer simpleVb;           ///< VBO used by helpers.
  GLTexture2D raster;          ///< Temporary texture for @ref drawImage(Image).
  GLTexture2D pointSprite;     ///< Point‑sprite base texture.
  GLTexture2D pointSpriteHighlight; ///< Optional highlight overlay sprite.
  double resumeTime;           ///< Timestamp used to keep animation time continuous on resume.
#ifdef __EMSCRIPTEN__
  float xMousePos;             ///< Last mouse X (Emscripten).
  float yMousePos;             ///< Last mouse Y (Emscripten).
#endif

  /** @brief Update stock shader uniforms (MVP/MV/MVit) to current transforms. */
  void shaderUpdate();

  /** @brief Request closing the window. */
  void closeWindow() { glEnv.setClose(); }

private:
  bool animationActive;   ///< Whether @ref animate() runs each frame.
  TrisDrawType lastTrisType; ///< Cached last triangle topology.
  GLsizei lastTrisCount;  ///< Cached last vertex count for triangles.
  bool lastLighting;      ///< Cached last lighting flag.
  double startTime;       ///< Start timestamp for animation.

  /** @brief Platform‑specific main loop implementation. */
  void mainLoop();

#ifdef __EMSCRIPTEN__
  /** @brief Wrapper for Emscripten's C‑style main loop callback. */
  static void mainLoopWrapper(void* arg) {
    GLApp* app = static_cast<GLApp*>(arg);
    app->mainLoop();
  }

  /** @brief Resize callback (Emscripten); forwards to @ref resize(). */
  static bool sizeCallback(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) {
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;

    //TODO
    return EM_TRUE;
  }

  static int map_modifiers_to_bitfield(const EmscriptenKeyboardEvent* e) {
    int mods = 0;
    if (e->shiftKey) mods |= (1 << 0);   // Shift → Bit 0
    if (e->ctrlKey)  mods |= (1 << 1);   // Ctrl → Bit 1
    if (e->altKey)   mods |= (1 << 2);   // Alt  → Bit 2
    return mods;
  }

  static int map_key_string_to_code(const char* code) {
    // Alphanumeric (A-Z)
    if (strncmp(code, "Key", 3) == 0 && strlen(code) == 4) {
      char c = code[3];
      if (c >= 'A' && c <= 'Z') {
        return GLENV_KEY_A + (c - 'A');
      }
    }

    // Number row (0–9)
    if (strncmp(code, "Digit", 5) == 0 && strlen(code) == 6) {
      char d = code[5];
      if (d >= '0' && d <= '9') {
        return GLENV_KEY_0 + (d - '0');
      }
    }

    // Numpad
    if (strncmp(code, "Numpad", 6) == 0) {
      const char* sub = code + 6;
      if (strcmp(sub, "0") == 0) return GLENV_KEY_KP_0;
      if (strcmp(sub, "1") == 0) return GLENV_KEY_KP_1;
      if (strcmp(sub, "2") == 0) return GLENV_KEY_KP_2;
      if (strcmp(sub, "3") == 0) return GLENV_KEY_KP_3;
      if (strcmp(sub, "4") == 0) return GLENV_KEY_KP_4;
      if (strcmp(sub, "5") == 0) return GLENV_KEY_KP_5;
      if (strcmp(sub, "6") == 0) return GLENV_KEY_KP_6;
      if (strcmp(sub, "7") == 0) return GLENV_KEY_KP_7;
      if (strcmp(sub, "8") == 0) return GLENV_KEY_KP_8;
      if (strcmp(sub, "9") == 0) return GLENV_KEY_KP_9;
      if (strcmp(sub, "Decimal") == 0) return GLENV_KEY_KP_DECIMAL;
      if (strcmp(sub, "Divide") == 0) return GLENV_KEY_KP_DIVIDE;
      if (strcmp(sub, "Multiply") == 0) return GLENV_KEY_KP_MULTIPLY;
      if (strcmp(sub, "Subtract") == 0) return GLENV_KEY_KP_SUBTRACT;
      if (strcmp(sub, "Add") == 0) return GLENV_KEY_KP_ADD;
      if (strcmp(sub, "Enter") == 0) return GLENV_KEY_KP_ENTER;
    }

    // Function keys
    if (strncmp(code, "F", 1) == 0 && strlen(code) <= 3) {
      int fn = atoi(code + 1);
      if (fn >= 1 && fn <= 12) return GLENV_KEY_F1 + (fn - 1);
    }

    // Other named keys
    if (strcmp(code, "Enter") == 0) return GLENV_KEY_ENTER;
    if (strcmp(code, "Space") == 0) return GLENV_KEY_SPACE;
    if (strcmp(code, "Tab") == 0) return GLENV_KEY_TAB;
    if (strcmp(code, "Backspace") == 0) return GLENV_KEY_BACKSPACE;
    if (strcmp(code, "Escape") == 0) return GLENV_KEY_ESCAPE;

    if (strcmp(code, "ArrowUp") == 0) return GLENV_KEY_UP;
    if (strcmp(code, "ArrowDown") == 0) return GLENV_KEY_DOWN;
    if (strcmp(code, "ArrowLeft") == 0) return GLENV_KEY_LEFT;
    if (strcmp(code, "ArrowRight") == 0) return GLENV_KEY_RIGHT;


    // Default: unknown or unsupported
    return 0;
  } EmscriptenUiEvent *uiEvent, void *userData);
  /** @brief Keyboard callback (Emscripten); forwards to hooks. */
  static bool keyCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData) {

    // TODO: handle modifiers properly
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;
    const int keyCode = map_key_string_to_code(keyEvent->code);
    if (eventType == EMSCRIPTEN_EVENT_KEYDOWN)
      glApp->keyboardChar(keyCode);
    glApp->keyboard(keyCode, keyCode, eventType, map_modifiers_to_bitfield(keyEvent));

    return EM_TRUE;
  }
  /** @brief Mouse move callback (Emscripten). */
  static bool cursorPositionCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;

    glApp->xMousePos = mouseEvent->targetX;
    glApp->yMousePos = mouseEvent->targetY;

    glApp->mouseMove(glApp->xMousePos, glApp->yMousePos);
    return EM_TRUE;
  }
  /** @brief Mouse button up callback (Emscripten). */
  static bool mouseButtonUpCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;

    glApp->mouseButton(mouseEvent->button, GLFW_RELEASE, 0, glApp->xMousePos, glApp->yMousePos);
    return EM_TRUE;
  }
  /** @brief Mouse button down callback (Emscripten). */
  static bool mouseButtonDownCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;

    glApp->mouseButton(mouseEvent->button, GLFW_PRESS, 0, glApp->xMousePos, glApp->yMousePos);
    return EM_TRUE;
  }
  /** @brief Mouse button callback (Emscripten). */
  static bool mouseButtonCallback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;

    glApp->mouseButton(mouseEvent->button, 0, 0, glApp->xMousePos, glApp->yMousePos);
    return EM_TRUE;
  }
  /** @brief Scroll callback (Emscripten). */
  static bool scrollCallback(int eventType, const EmscriptenWheelEvent *wheelEvent, void *userData) {
    GLApp* glApp = static_cast<GLApp*>(userData);
    if (!glApp) return EM_FALSE;

    // TODO

    return EM_TRUE;
  }
#else
  /** @brief Static storage of the current app instance for GLFW callbacks. */
  static GLApp* staticAppPtr;
  /** @brief Framebuffer resize callback (GLFW). */
  static void sizeCallback(GLFWwindow* window, int width, int height) {
    if (staticAppPtr) staticAppPtr->resize(width, height);
  }
  /** @brief Key action callback (GLFW). */
  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (staticAppPtr) staticAppPtr->keyboard(key, scancode, action, mods);
  }
  /** @brief Unicode character callback (GLFW). */
  static void keyCharCallback(GLFWwindow* window, unsigned int codepoint) {
    if (staticAppPtr) staticAppPtr->keyboardChar(codepoint);
  }
  /** @brief Mouse move callback (GLFW). */
  static void cursorPositionCallback(GLFWwindow* window, double xPosition, double yPosition) {
    if (staticAppPtr) staticAppPtr->mouseMove(xPosition, yPosition);
  }
  /** @brief Mouse button callback (GLFW). */
  static void mouseButtonCallback(GLFWwindow* window, int button, int state, int mods) {
    if (staticAppPtr) {
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);
      staticAppPtr->mouseButton(button, state, mods, xpos, ypos);
    }
  }
  /** @brief Scroll callback (GLFW). */
  static void scrollCallback(GLFWwindow* window, double x_offset, double y_offset) {
    if (staticAppPtr) {
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);
      staticAppPtr->mouseWheel(x_offset, y_offset, xpos, ypos);
    }
  }
#endif

  /**
   * @brief Build a thick line segment as a triangle strip around a polyline.
   * @param p0 Previous point (for correct joins).
   * @param p1 Current start point + its color.
   * @param c1 Current color.
   * @param p2 Current end point + its color.
   * @param c2 Next color.
   * @param p3 Next point (for correct joins).
   * @param lineThickness Thickness in pixels.
   * @param trisData Output buffer appended with interleaved vertices (pos+color).
   */
  void triangulate(const Vec3& p0,
                   const Vec3& p1, const Vec4& c1,
                   const Vec3& p2, const Vec4& c2,
                   const Vec3& p3,
                   float lineThickness,
                   std::vector<float>& trisData);
};

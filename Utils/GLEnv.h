#pragma once

#include <memory>
#include <string>
#include <chrono>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

#include "GLDebug.h"

/**
 * @file GLEnv.h
 * @brief Cross‑platform OpenGL window/context helper for desktop and Emscripten.
 *
 * Provides a minimal environment wrapper that creates and manages an OpenGL
 * context and window/canvas, exposes dimensions, vsync, FPS counter, and basic
 * input callback wiring. On desktop it uses **GLFW** (+**GLEW** initialization);
 * on the web it uses **Emscripten** WebGL APIs.
 *
 * @details Key features:
 *  - Creation of an OpenGL (or WebGL) context with configurable version and
 *    profile (@ref GLEnv::GLEnv).
 *  - Event callback registration for keyboard, mouse, and resize.
 *  - Frame lifecycle utilities: @ref endOfFrame() swaps buffers/polls events and
 *    updates an optional FPS counter in the window title.
 *  - Dimension queries (@ref getFramebufferSize(), @ref getWindowSize()).
 *  - VSync control (@ref setSync(), @ref getSync()).
 *  - Simple cursor mode handling via @ref setCursorMode().
 *
 * All GL error handling uses the utilities declared in @ref GLDebug.h.
 */

/**
 * @brief Element type selector for texture uploads/allocations.
 */
enum class GLDataType {BYTE, HALF, FLOAT};

/**
 * @brief Depth storage selection for depth textures.
 */
enum class GLDepthDataType {DEPTH16, DEPTH24, DEPTH32};

/**
 * @brief Mouse cursor behavior modes.
 */
enum class CursorMode {NORMAL, HIDDEN, FIXED};

/**
 * @brief Window/context manager for OpenGL (GLFW+GLEW) or WebGL (Emscripten).
 *
 * Creates a context and provides utilities around frame management, input,
 * vsync, and dimension queries. On errors, the desktop path throws
 * @ref GLException using GLFW's error callback; Web builds avoid tear‑down
 * exceptions for \c shouldClose().
 */
class GLEnv {
public:
  /**
   * @brief Construct a GL environment and create a window/canvas + context.
   * @param w          Initial width in pixels.
   * @param h          Initial height in pixels.
   * @param s          Multisample count (GLFW window hint).
   * @param title      Window title (desktop) or page title (web via FPS counter).
   * @param fpsCounter If true, compute FPS and append to the title each second.
   * @param sync       If true, enable vsync (glfwSwapInterval(1) / RAF timing).
   * @param major      Requested GL major version (desktop) / WebGL major.
   * @param minor      Requested GL minor version (desktop) / WebGL minor.
   * @param core       If true (desktop), request a core profile context.
   * @throw GLException On GLFW/GLEW initialization or window creation failures.
   */
  GLEnv(uint32_t w, uint32_t h, uint32_t s, const std::string& title, bool fpsCounter=false, bool sync=true, int major=2, int minor=1, bool core=false);
  /** @brief Destroy window/context (desktop) or release resources (web). */
  ~GLEnv();

#ifdef __EMSCRIPTEN__
  /** @brief Set key callback for Emscripten builds. */
  void setKeyCallback(em_key_callback_func f, void *userData);
  /** @brief Set mouse callbacks for Emscripten builds (move/press/release/wheel). */
  void setMouseCallbacks(em_mouse_callback_func p,
                         em_mouse_callback_func b,
                         em_mouse_callback_func bu,
                         em_mouse_callback_func bd,
                         em_wheel_callback_func s,
                         void *userData);
  /** @brief Set resize callback for Emscripten builds. */
  void setResizeCallback(em_ui_callback_func f, void *userData);
#else
  /** @brief Set key callback for GLFW builds. */
  void setKeyCallback(GLFWkeyfun f);
  /** @brief Set key + char callbacks for GLFW builds. */
  void setKeyCallbacks(GLFWkeyfun f, GLFWcharfun c);
  /** @brief Set mouse position/button/scroll callbacks for GLFW builds. */
  void setMouseCallbacks(GLFWcursorposfun p, GLFWmousebuttonfun b, GLFWscrollfun s);
  /** @brief Set framebuffer resize callback for GLFW builds. */
  void setResizeCallback(GLFWframebuffersizefun f);
#endif

  /**
   * @brief Get framebuffer size in texels/pixels.
   * @return @ref Dimensions with width/height (framebuffer).
   */
  Dimensions getFramebufferSize() const;
  /**
   * @brief Get logical window size.
   * @return @ref Dimensions with width/height (window/canvas).
   */
  Dimensions getWindowSize() const;
  /**
   * @brief Query whether the window should close (desktop always; web always false).
   */
  bool shouldClose() const;
  /** @brief Request closing the window (desktop only; no‑op on web). */
  void setClose();
  /**
   * @brief End‑of‑frame housekeeping: swap/poll + optional FPS title update.
   */
  void endOfFrame();

  /**
   * @brief Set mouse cursor behavior.
   * @param mode NORMAL (visible), HIDDEN, or FIXED (pointer‑lock/web or disabled cursor/desktop).
   */
  void setCursorMode(CursorMode mode);

  /** @brief Enable/disable FPS title updates. */
  void setFPSCounter(bool fpsCounter);
  /** @brief Enable/disable vsync. */
  void setSync(bool sync);
  /** @brief Current vsync state. */
  bool getSync() const {return sync;}

  /**
   * @brief Convenience GL error check, logs to stderr with an identifier.
   * @param id Human‑readable label for the check site.
   */
  static void checkGLError(const std::string& id);

  /** @brief Set the window/page title (stored; displayed on next FPS refresh). */
  void setTitle(const std::string& title);

private:
#ifndef __EMSCRIPTEN__
  GLFWwindow* window; ///< GLFW window handle (desktop only).
#endif
  bool sync;                              ///< VSync flag.
  std::string title;                      ///< Base window/page title.
  bool fpsCounter;                        ///< Enable FPS computation.
  std::chrono::high_resolution_clock::time_point last; ///< Time of last FPS update.
  uint64_t frameCount;                    ///< Frames accumulated since last title refresh.

  /** @brief GLFW error callback that throws a GLException (desktop only). */
  static void errorCallback(int error, const char* description);
};

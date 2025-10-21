#pragma once

#include <iostream>
#include <exception>

/**
 * @file GLDebug.h
 * @brief Minimal OpenGL error utilities, exception type, and a debug GL() wrapper macro.
 *
 * Declares a lightweight OpenGL error reporting facility:
 *  - A \c GLException used to surface GL errors and shader/program log messages.
 *  - A small \c Dimensions struct with a convenience aspect() getter.
 *  - String conversion for common GL error codes via @ref errorString().
 *  - Debug helpers @ref checkAndThrow(), @ref checkAndThrowShader(), and
 *    @ref checkAndThrowProgram().
 *  - A debug-only \c GL(stmt) macro that wraps GL calls to flush and report
 *    error codes before and after executing \p stmt.
 *
 * @note When \c NDEBUG is defined, \c GL(stmt) compiles down to a plain
 *       \c stmt without extra error checks.
 */

/**
 * @brief Exception used for OpenGL-related failures.
 *
 * Thrown by helpers in this header/its implementation to indicate a GL error
 * state or to forward shader/program info logs. The message can be retrieved
 * using what().
 */
class GLException : public std::exception {
public:
  /** @brief Construct with an explanatory message. */
  GLException(const std::string& whatStr) : whatStr(whatStr) {}
  /** @brief Retrieve the explanatory C-string. */
  virtual const char* what() const throw() {
    return whatStr.c_str();
  }
private:
  std::string whatStr; ///< Stored message.
};

/**
 * @brief Simple pixel/viewport dimensions with an aspect ratio helper.
 */
struct Dimensions {
  uint32_t width;  ///< Width in pixels/texels.
  uint32_t height; ///< Height in pixels/texels.

  /**
   * @brief Aspect ratio as width/height.
   * @return (float)width / (float)height.
   */
  float aspect() const {return float(width)/float(height);}
};

/**
 * @brief Convert a GL error enum to a short string (e.g., "GL_INVALID_VALUE").
 * @param glerr Error code returned by glGetError().
 * @return Human-readable token for common error codes or "Unknown Error".
 */
std::string errorString(GLenum glerr);

#ifndef NDEBUG

/**
 * @name Debug-only error reporting support
 * @{
 */

/**
 * @brief Maximum number of consecutive glGetError() iterations before bailing.
 *
 * Protects against pathological drivers/contexts where querying the error state
 * itself may produce errors or never clears.
 */
#define MAX_GL_ERROR_COUNT 10

/**
 * @brief Internal printer used by the GL() macro to emit formatted errors.
 * @param statement The stringified GL statement being executed.
 * @param location  One of "before" or "in" indicating when the error occurred.
 * @param line      Source line number.
 * @param file      Source file name.
 * @param errnum    Numeric GL error code.
 */
void errorOut(const std::string& statement, const std::string& location,
              uint32_t line, const std::string& file, uint32_t errnum);

/**
 * @brief Debug wrapper that flushes and reports GL errors around a statement.
 *
 * Usage:
 * @code
 * GL(glBindTexture(GL_TEXTURE_2D, id));
 * @endcode
 *
 * In debug builds, existing GL errors are drained and reported ("before"), the
 * statement is executed, then new errors are drained and reported ("in"). In
 * release builds (\c NDEBUG defined), this expands to the bare statement.
 */
# define GL(stmt)                                                      \
do {                                                                 \
GLenum glerr;                                                      \
uint32_t counter = 0;                                              \
while((glerr = glGetError()) != GL_NO_ERROR) {                     \
errorOut(#stmt, "before", __LINE__, __FILE__,                    \
static_cast<uint32_t>(glerr));                           \
counter++;                                                       \
if (counter > MAX_GL_ERROR_COUNT) break;                         \
}                                                                  \
stmt;                                                              \
counter = 0;                                                       \
while((glerr = glGetError()) != GL_NO_ERROR) {                     \
errorOut(#stmt, "in", __LINE__, __FILE__,                        \
static_cast<uint32_t>(glerr));                           \
counter++;                                                       \
if (counter > MAX_GL_ERROR_COUNT) break;                         \
}                                                                  \
} while(0)
/** @} */
#else
/** @brief Release build: GL(stmt) becomes a no-op wrapper around stmt. */
# define GL(stmt) do { stmt; } while(0)
#endif

/**
 * @brief Throw a GLException if glGetError() reports an error.
 */
void checkAndThrow();

/**
 * @brief Check a shader object's compile status and throw with its log on error.
 * @param shader Shader object name returned by glCreateShader().
 */
void checkAndThrowShader(GLuint shader);

/**
 * @brief Check a program object's link status and throw with its log on error.
 * @param program Program object name returned by glCreateProgram().
 */
void checkAndThrowProgram(GLuint program);

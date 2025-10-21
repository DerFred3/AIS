#pragma once

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLBuffer.h"
#include "GLProgram.h"

/**
 * @file GLArray.h
 * @brief RAII wrapper for an OpenGL Vertex Array Object (VAO).
 *
 * Encapsulates creation/binding/destruction of a VAO and provides helpers to
 * associate buffer contents with shader attributes as well as to bind an index
 * (element) buffer. Attribute specification uses the stride/format previously
 * established on the @ref GLBuffer via its setData() calls.
 *
 * @note All GL calls in the implementation are wrapped via a GL() macro
 *       (see GLDebug/GLEnv). The class does not assume ownership of the
 *       buffers or program it interacts with.
 */
class GLArray {
public:
  /** @brief Generate a VAO and bind it. */
  GLArray();
  /** @brief Destroy and delete the VAO name. */
  ~GLArray();

  /**
   * @brief Bind this VAO for subsequent vertex specification/draws.
   */
  void bind() const;

  /**
   * @brief Connect a vertex attribute from a buffer to a program input.
   * @param buffer    Source buffer holding interleaved attribute data.
   * @param program   Shader program from which to query the attribute location.
   * @param variable  Attribute identifier in the shader (e.g., "in_position").
   * @param elemCount Components per vertex attribute (e.g., 2, 3, or 4).
   * @param offset    Offset (in elements, not bytes) from the start of each record.
   * @param divisor   Instance divisor (0 = per-vertex; >0 = per-instance).
   *
   * Queries the attribute location from @p program and forwards the binding to
   * @ref GLBuffer::connectVertexAttrib() using the stride/element size derived
   * from the most recent setData() on @p buffer.
   */
  void connectVertexAttrib(const GLBuffer& buffer, const GLProgram& program,
                           const std::string& variable, size_t elemCount,
                           size_t offset=0, GLuint divisor = 0) const;

  /**
   * @brief Bind @p buffer as the index (element) buffer for this VAO.
   * @param buffer Buffer previously filled with indices (GLuint).
   */
  void connectIndexBuffer(const GLBuffer& buffer) const;

private:
  GLuint glId; ///< OpenGL VAO object name.

};

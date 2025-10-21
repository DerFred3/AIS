#pragma once

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/**
 * @file GLBuffer.h
 * @brief Thin RAII wrapper for an OpenGL buffer object with upload/binding helpers.
 *
 * Encapsulates creation/destruction of a generic GL buffer (e.g., VBO/EBO/SSBO)
 * and provides utilities to upload data (floats or unsigned ints), bind it,
 * and describe vertex attributes. The buffer target is chosen at construction
 * (e.g., `GL_ARRAY_BUFFER`, `GL_ELEMENT_ARRAY_BUFFER`).
 */
class GLBuffer {
public:
  /**
   * @brief Create a buffer object for the given OpenGL target.
   * @param target Buffer binding target (e.g., `GL_ARRAY_BUFFER`).
   * @post A GL name is generated via `glGenBuffers`.
   */
  GLBuffer(GLenum target);

  /** @brief Destroy and delete the GL buffer name. */
  ~GLBuffer();

  /**
   * @brief Upload interleaved float data and define element stride.
   * @param data              Flat vector of floats.
   * @param valuesPerElement  Number of float values that make up one logical element (stride in values).
   * @param usage             GL usage hint (e.g., `GL_STATIC_DRAW`).
   * @post Internal bookkeeping (elemSize/stride/type) is updated for later attribute binding.
   */
  void setData(const std::vector<GLfloat>& data, size_t valuesPerElement,GLenum usage=GL_STATIC_DRAW);

  /**
   * @brief Upload unsigned-int data (e.g., element indices).
   * @param data Flat vector of `GLuint` values.
   * @post Element size/stride/type are set for index-style data (stride = 1, type = `GL_UNSIGNED_INT`).
   */
  void setData(const std::vector<GLuint>& data);

  /**
   * @brief Upload interleaved float data from a raw pointer.
   * @param data              Pointer to `elemCount*valuesPerElement` floats.
   * @param elemCount         Number of logical elements.
   * @param valuesPerElement  Number of floats per element (stride in values).
   * @param usage             GL usage hint.
   */
  void setData(const float data[], size_t elemCount,
               size_t valuesPerElement,GLenum usage=GL_STATIC_DRAW);

  /**
   * @brief Upload unsigned-int data from a raw pointer.
   * @param data      Pointer to `elemCount` indices.
   * @param elemCount Number of indices.
   */
  void setData(const GLuint data[], size_t elemCount);

  /**
   * @brief Describe a vertex attribute array for the current buffer contents.
   * @param location  Attribute location as retrieved from the shader.
   * @param elemCount Number of components per attribute (e.g., 2,3,4).
   * @param offset    Offset (in elements, not bytes) from the beginning of each record.
   * @param divisor   Advanced: instance divisor for instanced rendering (0 = per-vertex).
   * @throw GLException If no prior `setData` call has established an element type.
   *
   * Binds the buffer to its target, enables the attribute array, and calls
   * `glVertexAttribPointer` with the stride and byte offset computed from the
   * previously uploaded data.
   */
  void connectVertexAttrib(GLuint location, size_t elemCount,
                           size_t offset=0, GLuint divisor = 0) const;

  /** @brief Bind the buffer to its target with `glBindBuffer(target, id)`. */
  void bind() const;

private:
  GLenum target;   ///< Buffer binding target passed at construction.
  GLuint bufferID; ///< GL name of the buffer object.
  size_t elemSize; ///< Size in bytes of one component (e.g., sizeof(float)).
  size_t stride;   ///< Stride in bytes between consecutive logical elements.
  GLenum type;     ///< Component type (`GL_FLOAT` or `GL_UNSIGNED_INT`).
};

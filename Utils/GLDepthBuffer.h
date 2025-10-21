#pragma once

#include <vector>

#include "GLEnv.h"

/**
 * @file GLDepthBuffer.h
 * @brief RAII wrapper for an OpenGL depth renderbuffer (GL_RENDERBUFFER).
 *
 * Manages the lifetime of a depth-only renderbuffer object and provides simple
 * helpers to query its size and (re)allocate storage. Intended for use as a
 * depth attachment to framebuffer objects (FBOs) when a texture is not needed.
 *
 * @note All GL calls are wrapped with the `GL()` macro defined in `GLEnv.h`.
 */
class GLDepthBuffer {
public:
  /**
   * @brief Construct a depth renderbuffer and allocate storage.
   * @param width  Initial width in pixels.
   * @param height Initial height in pixels.
   *
   * Creates a GL renderbuffer name and allocates a depth component storage for
   * the specified dimensions.
   */
  GLDepthBuffer(uint32_t width=0, uint32_t height=0);

  /** @brief Destroy and delete the GL renderbuffer name. */
  ~GLDepthBuffer();

  /**
   * @brief Retrieve the OpenGL renderbuffer object name.
   * @return GLuint of the managed depth renderbuffer.
   */
  const GLuint getId() const;

  /** @name Introspection */
  ///@{
  /** @brief Stored width in pixels. */
  uint32_t getWidth() const {return width;}
  /** @brief Stored height in pixels. */
  uint32_t getHeight() const {return height;}
  ///@}

  /**
   * @brief Reallocate storage to a new size.
   * @param width  New width in pixels.
   * @param height New height in pixels.
   *
   * Updates internal bookkeeping and calls glRenderbufferStorage with a depth
   * format for the new dimensions.
   */
  void setSize(uint32_t width, uint32_t height);
private:
  GLuint id;        ///< GL name of the renderbuffer object.
  uint32_t width;   ///< Width in pixels.
  uint32_t height;  ///< Height in pixels.
};

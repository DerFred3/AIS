#pragma once

#include <vector>

#include "GLEnv.h"

#include "GLTexture2D.h"
#include "GLTexture3D.h"
#include "GLDepthBuffer.h"
#include "GLDepthTexture.h"

/**
 * @file GLFramebuffer.h
 * @brief Convenience RAII wrapper for an OpenGL framebuffer object (FBO).
 *
 * Provides helpers to bind combinations of 2D textures, 3D texture slices, and
 * either a depth **texture** or a depth **renderbuffer** as attachments. The
 * correct draw buffers are set automatically based on the number of color
 * attachments, and the viewport is updated to match the attached color target's
 * dimensions.
 *
 * @details All GL calls in the implementation are wrapped by the \c GL() macro
 * from @c GLEnv.h. Overloads cover 0–4 color attachments for both 2D textures
 * and 3D texture slices. Use @ref checkBinding() to verify completeness after
 * binding, and @ref unbind2D()/@ref unbind3D() to detach attachments.
 */
class GLFramebuffer {
public:
  /** @brief Create a new framebuffer object (glGenFramebuffers). */
  GLFramebuffer();
  /** @brief Destroy the framebuffer (glDeleteFramebuffers). */
  ~GLFramebuffer();

  /**
   * @brief Retrieve the OpenGL framebuffer object name.
   * @return GLuint name of the managed FBO.
   */
  const GLuint getId() const;

  // ===== Bind helpers: depth texture + color 2D textures =====
  /** @brief Bind only a depth texture (no color attachments; draw buffer = NONE). */
  void bind(const GLDepthTexture& d);
  /** @brief Bind one 2D color texture plus a depth texture. */
  void bind(const GLTexture2D& t, const GLDepthTexture& d);
  /** @brief Bind two 2D color textures plus a depth texture. */
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLDepthTexture& d);
  /** @brief Bind three 2D color textures plus a depth texture. */
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2, const GLDepthTexture& d);
  /** @brief Bind four 2D color textures plus a depth texture. */
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2, const GLTexture2D& t3, const GLDepthTexture& d);

  // ===== Bind helpers: depth renderbuffer + color 2D textures =====
  /** @brief Bind one 2D color texture plus a depth renderbuffer. */
  void bind(const GLTexture2D& t, const GLDepthBuffer& d);
  /** @brief Bind two 2D color textures plus a depth renderbuffer. */
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLDepthBuffer& d);
  /** @brief Bind three 2D color textures plus a depth renderbuffer. */
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2, const GLDepthBuffer& d);
  /** @brief Bind four 2D color textures plus a depth renderbuffer. */
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2, const GLTexture2D& t3, const GLDepthBuffer& d);

  // ===== Bind helpers: color-only 2D textures (no depth) =====
  /** @brief Bind one 2D color texture (no depth). */
  void bind(const GLTexture2D& t);
  /** @brief Bind two 2D color textures (no depth). */
  void bind(const GLTexture2D& t0, const GLTexture2D& t1);
  /** @brief Bind three 2D color textures (no depth). */
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2);
  /** @brief Bind four 2D color textures (no depth). */
  void bind(const GLTexture2D& t0, const GLTexture2D& t1, const GLTexture2D& t2, const GLTexture2D& t3);

  // ===== Bind helpers: depth renderbuffer + 3D texture slices =====
  /** @brief Bind one slice from a 3D texture plus a depth renderbuffer. */
  void bind(const GLTexture3D& t, size_t slice, const GLDepthBuffer& d);
  /** @brief Bind two slices from 3D textures plus a depth renderbuffer. */
  void bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1, const GLDepthBuffer& d);
  /** @brief Bind three slices from 3D textures plus a depth renderbuffer. */
  void bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1, const GLTexture3D& t2, size_t slice2,
            const GLDepthBuffer& d);
  /** @brief Bind four slices from 3D textures plus a depth renderbuffer. */
  void bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1, const GLTexture3D& t2, size_t slice2,
            const GLTexture3D& t3, size_t slice3, const GLDepthBuffer& d);

  // ===== Bind helpers: color-only 3D texture slices (no depth) =====
  /** @brief Bind one slice from a 3D texture (no depth). */
  void bind(const GLTexture3D& t, size_t slice);
  /** @brief Bind two slices from 3D textures (no depth). */
  void bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1);
  /** @brief Bind three slices from 3D textures (no depth). */
  void bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1, const GLTexture3D& t2, size_t slice2);
  /** @brief Bind four slices from 3D textures (no depth). */
  void bind(const GLTexture3D& t0, size_t slice0, const GLTexture3D& t1, size_t slice1, const GLTexture3D& t2, size_t slice2,
            const GLTexture3D& t3, size_t slice3);

  /** @brief Detach 2D color attachments and unbind the FBO. */
  void unbind2D();
  /** @brief Detach 3D color/depth attachments and unbind the FBO. */
  void unbind3D();

  /**
   * @brief Check whether the currently bound FBO is complete.
   * @return true if glCheckFramebufferStatus(...) == GL_FRAMEBUFFER_COMPLETE.
   */
  bool checkBinding() const;

private:
  GLuint id; ///< OpenGL framebuffer object name.

  /**
   * @brief Set glDrawBuffers according to @p count and update viewport.
   * @param count  Number of color attachments (0..4).
   * @param width  Viewport width to set.
   * @param height Viewport height to set.
   */
  void setBuffers(size_t count, size_t width, size_t height);
};

// glFramebufferTexture3D

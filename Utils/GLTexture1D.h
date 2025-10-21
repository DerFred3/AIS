#ifndef __EMSCRIPTEN__
#pragma once

#include <vector>

#include "GLEnv.h"

/**
 * @file GLTexture1D.h
 * @brief Thin RAII wrapper for an OpenGL 1D texture.
 *
 * Manages the lifetime of a GL_TEXTURE_1D object, exposes its name via
 * getId(), and uploads byte data with configurable filtering and wrap mode.
 * The implementation uses the GL() error‑checking macro from @c GLEnv.h.
 */
class GLTexture1D {
public:
  /**
   * @brief Create an empty GL_TEXTURE_1D with sampler parameters.
   * @param magFilter GL magnification filter (e.g., GL_NEAREST or GL_LINEAR).
   * @param minFilter GL minification filter (e.g., GL_NEAREST or GL_LINEAR).
   * @param wrapX     GL wrap mode for S (x) coordinate (e.g., GL_REPEAT).
   * @post A texture object is generated and the given parameters are applied.
   */
  GLTexture1D(GLint magFilter=GL_NEAREST, GLint minFilter=GL_NEAREST,
              GLint wrapX=GL_REPEAT);

  /** @brief Copy‑construct and duplicate texture contents and parameters. */
  GLTexture1D(const GLTexture1D& other);
  /** @brief Copy‑assign and duplicate texture contents and parameters. */
  GLTexture1D& operator=(GLTexture1D other);

  /** @brief Destroy and delete the GL texture name. */
  ~GLTexture1D();

  /**
   * @brief Retrieve the OpenGL texture object name.
   * @return GLuint of the managed GL_TEXTURE_1D.
   */
  const GLuint getId() const;

  /**
   * @brief Upload interleaved unsigned‑byte data to the texture.
   * @param data           Pixel bytes, size must be componentCount * size.
   * @param size           Number of texels in x.
   * @param componentCount Components per texel (1..4 → R/RG/RGB/RGBA).
   * @throw GLException if @p data.size() != componentCount*size.
   * @post Internal format/format/type are set according to componentCount and
   *       the texture storage is defined at level 0.
   */
  void setData(const std::vector<GLubyte>& data, uint32_t size,
               uint8_t componentCount=4);

private:
  GLuint id;                 ///< GL name of the texture object.
  GLint internalformat;      ///< Internal storage format (e.g., GL_RGBA8).
  GLenum format;              ///< External pixel format (e.g., GL_RGBA).
  GLenum type;                ///< External pixel type (GL_UNSIGNED_BYTE).

  GLint magFilter;           ///< Magnification filter used at creation.
  GLint minFilter;           ///< Minification filter used at creation.
  GLint wrapX;               ///< Wrap mode for S coordinate.
  std::vector<GLubyte> data; ///< Last uploaded pixel bytes (kept for copies).
  uint32_t size;             ///< Texture width in texels.
  uint8_t componentCount;    ///< Components per texel (1..4).
};
#endif

#pragma once

#include <vector>

#include "GLEnv.h"

/**
 * @file GLTexture3D.h
 * @brief RAII wrapper for an OpenGL 3D texture with byte/float upload helpers.
 *
 * Encapsulates creation/destruction of a GL_TEXTURE_3D object, provides data
 * upload from interleaved vectors of unsigned bytes or 32-bit floats, basic
 * parameter control (filters, wrap per axis), and optional read-back helpers on
 * non-Emscripten builds.
 *
 * @note All GL calls are wrapped with the @c GL() macro provided by @c GLEnv.h.
 */
class GLTexture3D {
public:
  /**
   * @brief Create an empty GL_TEXTURE_3D and set sampler parameters.
   * @param magFilter GL magnification filter (e.g., GL_NEAREST/GL_LINEAR).
   * @param minFilter GL minification filter (e.g., GL_NEAREST/GL_LINEAR).
   * @param wrapX     GL wrap mode for S (x) coordinate (e.g., GL_REPEAT).
   * @param wrapY     GL wrap mode for T (y) coordinate.
   * @param wrapZ     GL wrap mode for R (z) coordinate.
   * @post A texture object is generated and the given parameters are applied.
   */
  GLTexture3D(GLint magFilter=GL_NEAREST, GLint minFilter=GL_NEAREST,
              GLint wrapX=GL_REPEAT, GLint wrapY=GL_REPEAT, GLint wrapZ=GL_REPEAT);

  /** @brief Destroy and delete the GL texture name. */
  ~GLTexture3D();

  /** @brief Copy-construct and duplicate texture contents and parameters. */
  GLTexture3D(const GLTexture3D& other);
  /** @brief Copy-assign and duplicate texture contents and parameters. */
  GLTexture3D& operator=(GLTexture3D other);

  /**
   * @brief Retrieve the OpenGL texture object name.
   * @return GLuint of the managed GL_TEXTURE_3D.
   */
  const GLuint getId() const;

  /** @brief Clear the texture to an empty image preserving dimensions/type. */
  void clear();

  /**
   * @brief Allocate an empty texture of given size and format.
   * @param width          Width in texels.
   * @param height         Height in texels.
   * @param depth          Depth in texels.
   * @param componentCount Components per texel (1..4).
   * @param isFloat        If true, allocate 32-bit float storage; otherwise unsigned byte.
   * @post Internal storage is allocated and initialized to zeros.
   */
  void setEmpty(uint32_t width, uint32_t height, uint32_t depth, uint8_t componentCount, bool isFloat=false);

  /**
   * @brief Upload unsigned-byte data with explicit dimensions.
   * @param data           Interleaved pixel bytes of size componentCount*width*height*depth.
   * @param width          Width in texels.
   * @param height         Height in texels.
   * @param depth          Depth in texels.
   * @param componentCount Components per texel (default 4 = RGBA).
   * @throw GLException if sizes do not match.
   */
  void setData(const std::vector<GLubyte>& data, uint32_t width, uint32_t height, uint32_t depth, uint8_t componentCount=4);

  /**
   * @brief Upload unsigned-byte data using previously set dimensions.
   * @param data Interleaved pixel bytes of size getSize().
   */
  void setData(const std::vector<GLubyte>& data);

  /**
   * @brief Upload 32-bit float data with explicit dimensions.
   * @param data           Interleaved float pixels of size componentCount*width*height*depth*4 bytes.
   * @param width          Width in texels.
   * @param height         Height in texels.
   * @param depth          Depth in texels.
   * @param componentCount Components per texel (default 4 = RGBA).
   * @throw GLException if sizes do not match.
   */
  void setData(const std::vector<GLfloat>& data, uint32_t width, uint32_t height, uint32_t depth, uint8_t componentCount=4);

  /**
   * @brief Upload 32-bit float data using previously set dimensions.
   * @param data Interleaved float pixels of size getSize()*sizeof(GLfloat).
   */
  void setData(const std::vector<GLfloat>& data);

  /** @name Introspection */
  ///@{
  /** @brief Texture height (texels). */
  uint32_t getHeight() const {return height;}
  /** @brief Texture width (texels). */
  uint32_t getWidth() const {return width;}
  /** @brief Texture depth (texels). */
  uint32_t getDepth() const {return depth;}
  /** @brief Components per texel (1..4). */
  uint32_t getComponentCount() const {return componentCount;}
  /** @brief Total number of elements (width*height*depth*components). */
  uint32_t getSize() const {return height*width*depth*componentCount;}
  /** @brief True if the storage/type is floating point. */
  bool getIsFloat() const {return isFloat;}
  ///@}

#ifndef __EMSCRIPTEN__
  /** @brief Read back raw unsigned bytes via glGetTexImage. */
  const std::vector<GLubyte>& getDataByte();
  /** @brief Read back raw floats via glGetTexImage. */
  const std::vector<GLfloat>& getDataFloat();
#endif

private:
  GLuint id;               ///< GL name of the texture object.
  GLint internalformat;    ///< Internal storage format (e.g., GL_RGBA8/GL_RGBA32F/...).
  GLenum format;           ///< External pixel format (GL_RED/RG/RGB/RGBA).
  GLenum type;             ///< External pixel type (GL_UNSIGNED_BYTE/GL_FLOAT).

  GLint magFilter;         ///< Magnification filter used at creation/last set.
  GLint minFilter;         ///< Minification filter used at creation/last set.
  GLint wrapX;             ///< Wrap mode for S.
  GLint wrapY;             ///< Wrap mode for T.
  GLint wrapZ;             ///< Wrap mode for R.
  std::vector<GLubyte> data; ///< Last uploaded BYTE pixels (kept for copies/readback).
  std::vector<GLfloat> fdata; ///< Last uploaded FLOAT pixels.
  uint32_t width;          ///< Texture width in texels.
  uint32_t height;         ///< Texture height in texels.
  uint32_t depth;          ///< Texture depth in texels.
  uint8_t componentCount;  ///< Components per texel (1..4).
  bool isFloat;            ///< Storage/type flag: true for FLOAT, false for BYTE.

  /**
   * @brief Core upload routine used by all typed setData() overloads.
   * @param data           Pointer to interleaved pixel data.
   * @param width          Width in texels.
   * @param height         Height in texels.
   * @param depth          Depth in texels.
   * @param componentCount Components per texel (1..4).
   * @param isFloat        If true, interpret data/type as GL_FLOAT; otherwise GL_UNSIGNED_BYTE.
   */
  void setData(GLvoid* data, uint32_t width, uint32_t height, uint32_t depth,
               uint8_t componentCount, bool isFloat);
};

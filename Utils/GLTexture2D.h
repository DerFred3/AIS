#pragma once

#include <vector>

#include "GLEnv.h"
#include "Image.h"

/**
 * @file GLTexture2D.h
 * @brief RAII wrapper for an OpenGL 2D texture with byte/half/float upload helpers.
 *
 * Encapsulates creation/destruction of a GL_TEXTURE_2D object, provides data
 * upload from interleaved vectors or an @ref Image, supports multiple data
 * types (see ::GLDataType), basic parameter control (filters, wrap), and
 * mipmap generation. Read-back helpers are available on non-Emscripten builds.
 *
 * @note All GL calls are wrapped with the @c GL() macro provided by @c GLEnv.h.
 */
class GLTexture2D {
public:
  /**
   * @brief Create an empty GL_TEXTURE_2D and set sampler parameters.
   * @param magFilter GL magnification filter (e.g., GL_NEAREST/GL_LINEAR).
   * @param minFilter GL minification filter (e.g., GL_NEAREST/GL_LINEAR).
   * @param wrapX     GL wrap mode for S (x) coordinate (e.g., GL_REPEAT).
   * @param wrapY     GL wrap mode for T (y) coordinate.
   * @post A texture object is generated and the given parameters are applied.
   */
  GLTexture2D(GLint magFilter=GL_NEAREST, GLint minFilter=GL_NEAREST,
              GLint wrapX=GL_REPEAT, GLint wrapY=GL_REPEAT);

  /** @brief Destroy and delete the GL texture name. */
  ~GLTexture2D();

  /**
   * @brief Construct a texture and immediately upload from an @ref Image.
   * @param image     Source image (bytes, interleaved).
   * @param magFilter GL magnification filter.
   * @param minFilter GL minification filter.
   * @param wrapX     Wrap mode for S.
   * @param wrapY     Wrap mode for T.
   */
  GLTexture2D(const Image& image,
              GLint magFilter=GL_NEAREST, GLint minFilter=GL_NEAREST,
              GLint wrapX=GL_REPEAT, GLint wrapY=GL_REPEAT);

  /** @brief Copy-construct and duplicate texture contents and parameters. */
  GLTexture2D(const GLTexture2D& other);

  /** @brief Copy-assign and duplicate texture contents and parameters. */
  GLTexture2D& operator=(const GLTexture2D& other);

  /**
   * @brief Retrieve the OpenGL texture object name.
   * @return GLuint of the managed GL_TEXTURE_2D.
   */
  const GLuint getId() const;

  /** @brief Clear the texture to an empty image preserving dimensions/type. */
  void clear();

  /**
   * @brief Allocate an empty texture of given size and format.
   * @param width          Width in texels.
   * @param height         Height in texels.
   * @param componentCount Components per texel (1..4).
   * @param dataType       Element type (BYTE/HALF/FLOAT).
   * @post Internal storage is allocated and initialized to zeros.
   */
  void setEmpty(uint32_t width, uint32_t height, uint8_t componentCount, GLDataType dataType=GLDataType::BYTE);

  /**
   * @brief Upload from an @ref Image (bytes).
   * @param image Source image; width/height/components taken from it.
   */
  void setData(const Image& image);

  /**
   * @brief Upload unsigned-byte data with explicit dimensions.
   * @param data           Interleaved pixel bytes of size componentCount*width*height.
   * @param width          Width in texels.
   * @param height         Height in texels.
   * @param componentCount Components per texel (default 4 = RGBA).
   * @throw GLException if sizes do not match.
   */
  void setData(const std::vector<GLubyte>& data, uint32_t width, uint32_t height, uint8_t componentCount=4);

  /**
   * @brief Upload unsigned-byte data using previously set dimensions.
   * @param data Interleaved pixel bytes of size getSize().
   */
  void setData(const std::vector<GLubyte>& data);

  /**
   * @brief Upload float data with explicit dimensions.
   * @param data           Interleaved float pixels of size componentCount*width*height.
   * @param width          Width in texels.
   * @param height         Height in texels.
   * @param componentCount Components per texel (default 4 = RGBA).
   * @throw GLException if sizes do not match.
   */
  void setData(const std::vector<GLfloat>& data, uint32_t width, uint32_t height, uint8_t componentCount=4);

  /**
   * @brief Upload float data using previously set dimensions.
   * @param data Interleaved float pixels of size getSize().
   */
  void setData(const std::vector<GLfloat>& data);

  /**
   * @brief Upload 16-bit half-float data with explicit dimensions.
   * @param data           Interleaved half-float pixels (GLhalf) of size componentCount*width*height.
   * @param width          Width in texels.
   * @param height         Height in texels.
   * @param componentCount Components per texel (default 4 = RGBA).
   * @throw GLException if sizes do not match.
   */
  void setData(const std::vector<GLhalf>& data, uint32_t width, uint32_t height, uint8_t componentCount=4);

  /**
   * @brief Upload 16-bit half-float data using previously set dimensions.
   * @param data Interleaved half-float pixels of size getSize().
   */
  void setData(const std::vector<GLhalf>& data);

  /**
   * @brief Update filtering parameters on the bound texture.
   * @param magFilter GL magnification filter.
   * @param minFilter GL minification filter.
   */
  void setFilter(GLint magFilter, GLint minFilter);

  /**
   * @brief Replace a single pixel at (x,y) with unsigned-byte RGBA/RGB/etc.
   * @param data Bytes for one texel (size = componentCount).
   * @param x    X coordinate (0..width-1).
   * @param y    Y coordinate (0..height-1).
   */
  void setPixel(const std::vector<GLubyte>& data, uint32_t x, uint32_t y);

  /** @brief Generate mipmaps for the texture. */
  void generateMipmap();

  /** @name Introspection */
  ///@{
  uint32_t getHeight() const {return height;}
  uint32_t getWidth() const {return width;}
  uint32_t getComponentCount() const {return componentCount;}
  uint32_t getSize() const {return height*width*componentCount;}
  GLDataType getType() const {return dataType;}
  ///@}

#ifndef __EMSCRIPTEN__
  /** @brief Read back texture as an @ref Image (BYTE). */
  Image getImage();
  /** @brief Read back raw unsigned bytes via glGetTexImage. */
  const std::vector<GLubyte>& getDataByte();
  /** @brief Read back raw half floats via glGetTexImage. */
  const std::vector<GLhalf>& getDataHalf();
  /** @brief Read back raw floats via glGetTexImage. */
  const std::vector<GLfloat>& getDataFloat();
#endif

private:
  GLuint id;              ///< GL name of the texture object.
  GLint internalformat;   ///< Internal storage format (e.g., GL_RGBA8/GL_RGBA16F/...).
  GLenum format;          ///< External pixel format (GL_RED/RG/RGB/RGBA).
  GLenum type;            ///< External pixel type (GL_UNSIGNED_BYTE/GL_HALF_FLOAT/GL_FLOAT).

  GLint magFilter;        ///< Magnification filter used at creation/last set.
  GLint minFilter;        ///< Minification filter used at creation/last set.
  GLint wrapX;            ///< Wrap mode for S.
  GLint wrapY;            ///< Wrap mode for T.

  std::vector<GLubyte> data; ///< Last uploaded BYTE pixels (kept for copies/readback).
  std::vector<GLhalf>  hdata; ///< Last uploaded HALF pixels.
  std::vector<GLfloat> fdata; ///< Last uploaded FLOAT pixels.
  uint32_t width{0};     ///< Texture width in texels.
  uint32_t height{0};    ///< Texture height in texels.
  uint8_t componentCount{0}; ///< Components per texel (1..4).
  GLDataType dataType{GLDataType::BYTE}; ///< Element type of the texture storage.

  /**
   * @brief Core upload routine used by all typed setData() overloads.
   * @param data           Pointer to interleaved pixel data.
   * @param width          Width in texels.
   * @param height         Height in texels.
   * @param componentCount Components per texel (1..4).
   * @param dataType       Element type.
   */
  void setData(GLvoid* data, uint32_t width, uint32_t height,
               uint8_t componentCount, GLDataType dataType);
};

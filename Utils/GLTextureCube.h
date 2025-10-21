#pragma once

#include <vector>

#include "GLEnv.h"
#include "Image.h"

/**
 * @file GLTextureCube.h
 * @brief RAII wrapper for an OpenGL cube map texture with byte/half/float uploads.
 *
 * Encapsulates creation/destruction of a GL_TEXTURE_CUBE_MAP object, data
 * uploads for each face from @ref Image or raw interleaved vectors, basic
 * sampler parameter control (filters, wrap S/T/R), and mipmap generation.
 *
 * @note All GL calls are wrapped with the @c GL() macro provided by @c GLEnv.h.
 */

enum class Face {
  POSX=0, ///< +X face (GL_TEXTURE_CUBE_MAP_POSITIVE_X).
  NEGX,   ///< −X face (GL_TEXTURE_CUBE_MAP_NEGATIVE_X).
  POSY,   ///< +Y face (GL_TEXTURE_CUBE_MAP_POSITIVE_Y).
  NEGY,   ///< −Y face (GL_TEXTURE_CUBE_MAP_NEGATIVE_Y).
  POSZ,   ///< +Z face (GL_TEXTURE_CUBE_MAP_POSITIVE_Z).
  NEGZ    ///< −Z face (GL_TEXTURE_CUBE_MAP_NEGATIVE_Z).
};

/**
 * @brief OpenGL cube map texture (6 faces) with convenience upload helpers.
 */
class GLTextureCube {
public:
  /**
   * @brief Create an empty cube map and apply sampler parameters.
   * @param magFilter GL magnification filter (e.g., GL_NEAREST/GL_LINEAR).
   * @param minFilter GL minification filter (e.g., GL_NEAREST/GL_LINEAR).
   * @param wrapX     GL wrap mode for S (x) coordinate (e.g., GL_CLAMP_TO_EDGE).
   * @param wrapY     GL wrap mode for T (y) coordinate.
   * @param wrapZ     GL wrap mode for R (z) coordinate.
   * @post A GL texture object is generated and the given parameters are applied.
   */
  GLTextureCube(GLint magFilter=GL_NEAREST, GLint minFilter=GL_NEAREST,
                GLint wrapX=GL_CLAMP_TO_EDGE, GLint wrapY=GL_CLAMP_TO_EDGE,
                GLint wrapZ=GL_CLAMP_TO_EDGE);
  /** @brief Destroy and delete the GL texture name. */
  ~GLTextureCube();

  /**
   * @brief Construct and upload six faces from images.
   * @param image0..image5 Source images corresponding to faces; see @ref setData(Image,Face).
   * @param magFilter GL magnification filter.
   * @param minFilter GL minification filter.
   * @param wrapX     GL wrap mode for S.
   * @param wrapY     GL wrap mode for T.
   * @note Z wrap is left at the constructor default (R coordinate).
   */
  GLTextureCube(const Image& image0, const Image& image1, const Image& image2,
                const Image& image3, const Image& image4, const Image& image5,
                GLint magFilter=GL_NEAREST, GLint minFilter=GL_NEAREST,
                GLint wrapX=GL_REPEAT, GLint wrapY=GL_REPEAT);

  /** @brief Copy‑construct and duplicate texture contents and parameters. */
  GLTextureCube(const GLTextureCube& other);
  /** @brief Copy‑assign and duplicate texture contents and parameters. */
  GLTextureCube& operator=(const GLTextureCube& other);

  /**
   * @brief Retrieve the OpenGL texture object name.
   * @return GLuint of the managed GL_TEXTURE_CUBE_MAP.
   */
  const GLuint getId() const;

  /** @brief Clear the texture by reinitializing the current storage. */
  void clear();

  /**
   * @brief Allocate empty storage for all six faces.
   * @param width          Width in texels for each face.
   * @param height         Height in texels for each face.
   * @param componentCount Components per texel (1..4).
   * @param dataType       Element type (BYTE/HALF/FLOAT).
   * @post Internal storage is allocated and initialized to zeros for all faces.
   */
  void setEmpty(uint32_t width, uint32_t height, uint8_t componentCount,
                GLDataType dataType=GLDataType::BYTE);

  /**
   * @brief Upload one face from an @ref Image (unsigned bytes).
   * @param image Source image (bytes, interleaved).
   * @param face  Destination face.
   */
  void setData(const Image& image, Face face);

  /**
   * @brief Upload one face from unsigned‑byte data with explicit dimensions.
   * @param data           Interleaved pixel bytes of size componentCount*width*height.
   * @param width          Width in texels.
   * @param height         Height in texels.
   * @param face           Destination face.
   * @param componentCount Components per texel (default 4 = RGBA).
   * @throw GLException if sizes do not match.
   */
  void setData(const std::vector<GLubyte>& data, uint32_t width,
               uint32_t height, Face face, uint8_t componentCount=4);

  /**
   * @brief Upload one face from unsigned‑byte data using existing dimensions.
   * @param data Interleaved pixel bytes of size getSize().
   * @param face Destination face.
   */
  void setData(const std::vector<GLubyte>& data, Face face);

  /**
   * @brief Upload one face from 32‑bit float data with explicit dimensions.
   * @param data           Interleaved floats of size componentCount*width*height.
   * @param width          Width in texels.
   * @param height         Height in texels.
   * @param face           Destination face.
   * @param componentCount Components per texel (default 4 = RGBA).
   * @throw GLException if sizes do not match.
   */
  void setData(const std::vector<GLfloat>& data, uint32_t width,
               uint32_t height, Face face, uint8_t componentCount=4);

  /**
   * @brief Upload one face from 32‑bit float data using existing dimensions.
   * @param data Interleaved floats of size getSize().
   * @param face Destination face.
   */
  void setData(const std::vector<GLfloat>& data, Face face);

  /**
   * @brief Upload one face from 16‑bit half‑float data with explicit dimensions.
   * @param data           Interleaved half‑floats of size componentCount*width*height.
   * @param width          Width in texels.
   * @param height         Height in texels.
   * @param face           Destination face.
   * @param componentCount Components per texel (default 4 = RGBA).
   * @throw GLException if sizes do not match.
   */
  void setData(const std::vector<GLhalf>& data, uint32_t width,
               uint32_t height, Face face, uint8_t componentCount=4);

  /**
   * @brief Upload one face from 16‑bit half‑float data using existing dimensions.
   * @param data Interleaved half‑floats of size getSize().
   * @param face Destination face.
   */
  void setData(const std::vector<GLhalf>& data, Face face);

  /**
   * @brief Update filtering parameters on the bound cube map.
   * @param magFilter GL magnification filter.
   * @param minFilter GL minification filter.
   */
  void setFilter(GLint magFilter, GLint minFilter);

  /** @name Introspection */
  ///@{
  uint32_t getHeight() const {return height;}       ///< Face height in texels.
  uint32_t getWidth()  const {return width;}        ///< Face width in texels.
  uint32_t getComponentCount() const {return componentCount;} ///< Components per texel (1..4).
  uint32_t getSize() const {return height*width*componentCount;} ///< Elements per face.
  GLDataType getType() const {return dataType;}     ///< Element type (BYTE/HALF/FLOAT).
  ///@}

  /** @brief Generate mipmaps for the cube map. */
  void generateMipmap();

private:
  GLuint id;               ///< GL name of the texture object.
  GLint internalformat;    ///< Internal storage format (e.g., GL_RGBA8/GL_RGBA16F/GL_RGBA32F).
  GLenum format;           ///< External pixel format (GL_RED/RG/RGB/RGBA).
  GLenum type;             ///< External pixel type (GL_UNSIGNED_BYTE/GL_HALF_FLOAT/GL_FLOAT).

  GLint magFilter;         ///< Magnification filter used at creation/last set.
  GLint minFilter;         ///< Minification filter used at creation/last set.
  GLint wrapX;             ///< Wrap mode for S.
  GLint wrapY;             ///< Wrap mode for T.
  GLint wrapZ;             ///< Wrap mode for R.
  std::vector<GLubyte> data; ///< Last uploaded BYTE pixels (kept for copies/readback).
  std::vector<GLhalf> hdata; ///< Last uploaded HALF pixels.
  std::vector<GLfloat> fdata; ///< Last uploaded FLOAT pixels.
  uint32_t width{0};       ///< Face width in texels.
  uint32_t height{0};      ///< Face height in texels.
  uint8_t componentCount{0}; ///< Components per texel (1..4).
  GLDataType dataType;     ///< Element type of the texture storage.

  /**
   * @brief Core upload routine used by all typed setData() overloads.
   * @param data           Pointer to interleaved pixel data.
   * @param width          Width in texels.
   * @param height         Height in texels.
   * @param face           Destination face.
   * @param componentCount Components per texel (1..4).
   * @param dataType       Element type (BYTE/HALF/FLOAT).
   */
  void setData(GLvoid* data, uint32_t width, uint32_t height, Face face,
               uint8_t componentCount, GLDataType dataType);
};

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Vec2.h"
#include "Vec3.h"
#include "Image.h"
#include "GLTexture2D.h"

/**
 * @file Grid2D.h
 * @brief 2D scalar grid (float) with sampling, arithmetic, SDF, and I/O helpers.
 *
 * Represents a width×height grid of single‑precision values stored in row‑major
 * order. Provides bilinear sampling in normalized coordinates, gradient‑based
 * normal estimation, per‑element arithmetic (with scalars and other grids, with
 * automatic size reconciliation), conversion to images / textures, and simple
 * binary streaming.
 *
 * @details Unless stated otherwise, functions interpret normalized positions
 * (x,y) with 0 at the left/bottom and 1 at the right/top, and clamp to
 * [0,1]. When combining two grids of different sizes, operations resample the
 * smaller grid to the larger grid's resolution using bilinear sampling.
 */
class Grid2D {
public:
  /**
   * @brief Construct an empty grid initialized to zeros.
   * @param width  Number of columns.
   * @param height Number of rows.
   */
  Grid2D(size_t width, size_t height);

  /**
   * @brief Construct from raw data (size must equal width*height).
   * @param width  Number of columns.
   * @param height Number of rows.
   * @param data   Row‑major values copied into the grid.
   * @throw std::runtime_error If @p data.size() != width*height.
   */
  Grid2D(size_t width, size_t height, const std::vector<float> data);

  /**
   * @brief Copy‑construct from another grid.
   * @param other Source grid.
   */
  Grid2D(const Grid2D& other);

  /**
   * @brief Construct from the luminance of an @ref Image.
   * @param image Source image; the first channel is sampled and normalized to
   *              [0,1]. Width/height match the image.
   */
  Grid2D(const Image& image);

  /**
   * @brief Read grid from a binary stream written by @ref save().
   * @param is Input stream; reads width, height, then width*height floats.
   */
  Grid2D(std::istream &is);

  /**
   * @brief Write grid to a binary stream.
   * @param os Output stream; writes width, height, then width*height floats.
   */
  void save(std::ostream &os) const;

  /** @name Geometry */
  ///@{
  /** @brief Grid width in elements. */
  size_t getWidth() const;
  /** @brief Grid height in elements. */
  size_t getHeight() const;
  ///@}

  /**
   * @brief String dump of values in row‑major order.
   * @return CSV‑like multiline representation.
   */
  std::string toString() const;

  /**
   * @brief Convert to an 8‑bit RGB byte array (grayscale replicated to RGB).
   * @return Vector of size width*height*3 with bytes in 0..255.
   */
  std::vector<uint8_t> toByteArray() const;

  /**
   * @brief Compute a signed distance field from a binary threshold.
   * @param threshold Values ≥ threshold are considered inside.
   * @return Grid whose values are distances to the nearest boundary, positive
   *         inside and negative outside.
   */
  Grid2D toSignedDistance(float threshold) const;

  /**
   * @brief Upload to an OpenGL texture (1 channel).
   * @return GLTexture2D with dimensions width×height.
   */
  GLTexture2D toTexture() const;

  /**
   * @brief Set one value.
   * @param x Column index in [0,width).
   * @param y Row index in [0,height).
   * @param value New value.
   */
  void setValue(size_t x, size_t y, float value);

  /**
   * @brief Fetch value using normalized [0,1] coordinates (nearest‑neighbor via index mapping).
   * @param x Normalized x.
   * @param y Normalized y.
   * @return Value at floor(x*(width−1)), floor(y*(height−1)).
   */
  float getValueNormalized(float x, float y) const;

  /**
   * @brief Fetch value by integer coordinates.
   * @param x Column index.
   * @param y Row index.
   * @return Stored value.
   */
  float getValue(size_t x, size_t y) const;

  /**
   * @brief Bilinear sample at normalized coordinates in [0,1].
   * @param x Normalized x.
   * @param y Normalized y.
   * @return Interpolated value.
   */
  float sample(float x, float y) const ;

  /**
   * @brief Bilinear sample at normalized coordinates in [0,1].
   * @param pos (x,y) normalized position.
   */
  float sample(const Vec2& pos) const ;

  /**
   * @brief Estimate a normal from local height differences at normalized coords.
   * @param x,y Normalized coordinates.
   * @return Unit normal approximating ∇height.
   */
  Vec3 normal(float x, float y) const;

  /**
   * @brief Estimate a normal from local height differences at normalized coords.
   * @param pos (x,y) normalized position.
   */
  Vec3 normal(const Vec2& pos) const;

  /**
   * @brief Generate a grid of random values in [0,1) using a fixed seed.
   * @param x Width.
   * @param y Height.
   * @param seed Seed for the RNG.
   */
  static Grid2D genRandom(size_t x, size_t y, uint32_t seed);

  /**
   * @brief Generate a grid of random values in [0,1) using a global RNG.
   * @param x Width.
   * @param y Height.
   */
  static Grid2D genRandom(size_t x, size_t y);

  // ===== Scalar arithmetic =====
  /** @brief Per‑element multiply by scalar. */
  Grid2D operator*(const float& value) const;
  /** @brief Per‑element divide by scalar. */
  Grid2D operator/(const float& value) const;
  /** @brief Per‑element add scalar. */
  Grid2D operator+(const float& value) const;
  /** @brief Per‑element subtract scalar. */
  Grid2D operator-(const float& value) const;

  // ===== Grid‑wise arithmetic (size reconciliation by resampling) =====
  /** @brief Per‑element addition with resampling if needed. */
  Grid2D operator+(const Grid2D& other) const;
  /** @brief Per‑element division with resampling if needed. */
  Grid2D operator/(const Grid2D& other) const;
  /** @brief Per‑element multiplication with resampling if needed. */
  Grid2D operator*(const Grid2D& other) const;
  /** @brief Per‑element subtraction with resampling if needed. */
  Grid2D operator-(const Grid2D& other) const;

  /**
   * @brief Normalize values to [0,maxVal] by affine remapping.
   * @param maxVal Upper bound after normalization (default 1).
   */
  void normalize(const float maxVal = 1);

  /**
   * @brief Location of the maximum value.
   * @return Integer coordinates (x,y) of the first maximum encountered.
   */
  Vec2t<size_t> maxValue() const;

  /**
   * @brief Location of the minimum value.
   * @return Integer coordinates (x,y) of the first minimum encountered.
   */
  Vec2t<size_t> minValue() const;

  /**
   * @brief Fill all elements with a constant.
   * @param value Value to assign.
   */
  void fill(float value);

  /**
   * @brief Stream insertion (delegates to @ref toString()).
   */
  friend std::ostream& operator<<(std::ostream &os, const Grid2D& v);

  /**
   * @brief Create a grid from the luminance of a BMP file.
   * @param filename Path to the BMP image.
   * @return Grid with width/height of the image; values in [0,1].
   */
  static Grid2D fromBMP(const std::string& filename);

private:
  size_t width;               ///< Number of columns.
  size_t height;              ///< Number of rows.
  std::vector<float> data{};  ///< Row‑major storage of size width*height.

  /**
   * @brief Convert (x,y) to a flat index (row‑major).
   * @param x Column index.
   * @param y Row index.
   * @return x + y*width.
   */
  size_t index(size_t x, size_t y) const;

  /**
   * @brief Determine the per‑dimension max(width,height) between two grids.
   * @param other Grid to compare against.
   * @return Pair (maxWidth,maxHeight).
   */
  std::pair<size_t,size_t> findMaxSize(const Grid2D& other) const;
};

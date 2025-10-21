#pragma once

#include <fstream>
#include <iostream>
#include <exception>
#include <string>
#include <sstream>

#include "Vec2.h"
#include "Image.h"

/**
 * @file bmp.h
 * @brief Minimal BMP I/O utilities and a region blit helper.
 *
 * Provides functions to save images to 24/32-bit BMP, load BMP files into an
 * \c Image, and copy a rectangular region between images. The save routines
 * accept either an \c Image or raw component data (\c uint8_t or \c float).
 *
 * @details
 *  - Saving writes uncompressed Windows BMP (BITMAPINFOHEADER, 40-byte info
 *    header) and converts component order from RGB(A) to BMP's BGR(A).
 *  - Rows are padded to 4-byte boundaries as required by the BMP format.
 *  - The \c save overload taking floats maps values in [0,1) to bytes via
 *    \f$byte = \lfloor 255\,x \rfloor\f$.
 *  - Loading supports 8/16/24/32 bpp BMP. Component order is converted from
 *    BGR(A) to RGB(A). Top-down BMPs (negative height) are returned flipped to
 *    match the conventional top-left origin used by \c Image.
 */
namespace BMP {
  /**
   * @brief Exception type thrown by BMP I/O helpers.
   */
  class BMPException : public std::exception {
  public:
    /**
     * @brief Construct with an explanatory message.
     * @param whatStr Human-readable description.
     */
    BMPException(const std::string& whatStr) : whatStr(whatStr) {}
    /**
     * @brief Retrieve the explanatory string.
     * @return NUL-terminated message string.
     */
    virtual const char* what() const throw() {
      return whatStr.c_str();
    }
  private:
    std::string whatStr; ///< Stored message.
  };

  /**
   * @brief Save an \c Image to a BMP file.
   * @param filename Destination file path.
   * @param source   Source image (RGB or RGBA expected).
   * @param ignoreSize If false, validate that total size fits into 32-bit BMP
   *        header fields; if too large, throw \c BMPException. If true, skip
   *        the size check (use with care).
   * @return \c true on success; \c false if the file could not be opened.
   */
  bool save(const std::string& filename, const Image& source, bool ignoreSize=false);

  /**
   * @brief Save raw 8-bit component data to a BMP file.
   * @param filename Destination file path.
   * @param w        Image width in pixels.
   * @param h        Image height in pixels.
   * @param data     Interleaved pixel data of size \c w*h*iComponentCount in
   *                 RGB(A) order per pixel.
   * @param iComponentCount Number of components per pixel (3 for RGB, 4 for RGBA).
   * @param ignoreSize If false, validate that total size fits 32-bit BMP fields
   *                   and throw on overflow; if true, skip this validation.
   * @return \c true on success; \c false if the file could not be opened.
   * @note Rows are padded to 4-byte boundaries; components are written BGR(A)
   *       as required by BMP.
   */
  bool save(const std::string& filename, uint32_t w, uint32_t h,
            const std::vector<uint8_t>& data, uint8_t iComponentCount = 3,
            bool ignoreSize=false);

  /**
   * @brief Save raw floating-point component data to a BMP file.
   * @param filename Destination file path.
   * @param w        Image width in pixels.
   * @param h        Image height in pixels.
   * @param data     Interleaved pixel data of size \c w*h*iComponentCount in
   *                 RGB(A) order per pixel. Each channel should be in [0,1].
   * @param iComponentCount Number of components per pixel (3 for RGB, 4 for RGBA).
   * @param ignoreSize If false, validate that total size fits 32-bit BMP fields
   *                   and throw on overflow; if true, skip this validation.
   * @return \c true on success; \c false if the file could not be opened.
   * @note Values are converted to bytes by multiplying by 255 and casting.
   */
  bool save(const std::string& filename, uint32_t w, uint32_t h,
            const std::vector<float>& data, uint8_t iComponentCount = 3,
            bool ignoreSize=false);

  /**
   * @brief Load a BMP file into an \c Image.
   * @param filename Source file path.
   * @return Loaded image (RGB/RGBA). For 8/16/24/32 bpp BMPs, the data is
   *         converted from BGR(A) to RGB(A). If the BMP is stored top-down
   *         (negative height), the image is flipped to a top-left origin.
   * @throw BMPException On I/O errors, unsupported bit depth, or invalid file.
   */
  Image load(const std::string& filename);

  /**
   * @brief Copy a rectangular region from \p source to \p target (blit).
   *
   * Copies pixels in the half-open region [\p sourceStart, \p sourceEnd) from
   * \p source into \p target with its top-left corner placed at \p targetStart.
   * When \p skipChecks is false, component count compatibility and bounds are
   * validated. If the destination would overflow, \p target is grown to fit and
   * the original contents are preserved.
   *
   * @param source       Source image.
   * @param sourceStart  Min corner (inclusive) of the source region.
   * @param sourceEnd    Max corner (exclusive) of the source region.
   * @param target       Destination image to write into (may be resized).
   * @param targetStart  Destination top-left position where the region is placed.
   * @param skipChecks   If true, skip validation and resizing logic (caller is
   *                     responsible for safety).
   */
  void blit(const Image& source, const Vec2ui& sourceStart, const Vec2ui& sourceEnd,
            Image& target, const Vec2ui& targetStart, bool skipChecks=false);
}

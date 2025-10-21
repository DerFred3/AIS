#pragma once

#include "Vec2.h"
#include "Image.h"

/**
 * @file ImageLoader.h
 * @brief Thin wrapper around stb_image to load common raster formats into @ref Image.
 *
 * Declares a convenience API to read image files using **stb_image**. The
 * loader fills an @ref Image with interleaved 8-bit components and supports an
 * optional vertical flip to match a top-left origin.
 *
 * @note Implementation is provided in the corresponding translation unit and
 *       uses `stbi_load`/`stbi_image_free`. See the `.cpp` for details.
 */
namespace ImageLoader {
  /**
   * @brief Exception type thrown by ImageLoader on failures.
   *
   * Used to signal I/O and decoding errors originating from the underlying
   * `stb_image` calls or from invalid inputs.
   */
  class Exception : public std::exception {
  public:
    /**
     * @brief Construct with an explanatory message.
     * @param whatStr Human-readable description.
     */
    Exception(const std::string& whatStr) : whatStr(whatStr) {}

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
   * @brief Load an image file using stb_image.
   * @param filename Path to the image file (formats supported by stb_image).
   * @param flipY If true, flip rows vertically on load (default: true).
   * @return @ref Image with width/height/componentCount set and data copied.
   * @throw Exception If the file cannot be read/decoded.
   *
   * @details The loader preserves the component count reported by stb_image
   *          (e.g., 1/2/3/4). On success, pixel bytes are copied into the
   *          returned @ref Image and the temporary buffer is freed.
   */
  Image load(const std::string& filename, bool flipY=true);
}

#pragma once

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>

#include "bmp.h"
#include "Vec2.h"
#include "GLTexture2D.h"
#include "GLArray.h"
#include "GLBuffer.h"
#include "Grid2D.h"

/**
 * @file FontRenderer.h
 * @brief Bitmap font utilities: CPU-side text rasterization and an OpenGL font engine.
 *
 * Declares two related components:
 *  - @ref FontRenderer: CPU-only blitter that composes strings into an @ref Image
 *    using a bitmap atlas and character box positions read from a text file.
 *  - @ref FontEngine: runtime OpenGL renderer that draws strings as quads using
 *    per-character textures; optionally renders from a signed-distance field.
 */

/**
 * @brief Character box in a bitmap atlas.
 *
 * Holds the character code and the inclusive-exclusive rectangle defining its
 * region in the atlas image, expressed in pixel coordinates with origin at the
 * top-left of the atlas.
 */
struct CharPosition {
  char c;         ///< Character this rectangle corresponds to.
  Vec2ui topLeft; ///< Top-left corner (inclusive) of the glyph box in the atlas.
  Vec2ui bottomRight; ///< Bottom-right corner (exclusive) of the glyph box in the atlas.
};

/**
 * @brief Texture + transform bundle for a single glyph used by @ref FontEngine.
 */
struct CharTex {
  GLTexture2D tex; ///< Glyph texture (RGBA or single-channel).
  Mat4 scale;      ///< Per-glyph scale to normalize to [0,1] quad space.
  Mat4 trans;      ///< Per-glyph translation used by the renderer.
  float width;     ///< Glyph width normalized by the maximum glyph width in the set.
  float height;    ///< Glyph height normalized by the maximum glyph height in the set.
};

/**
 * @brief Text alignment modes used by the OpenGL renderer.
 */
enum class Alignment {
  Left,   ///< Anchor on the left side.
  Right,  ///< Anchor on the right side.
  Center  ///< Anchor centered around the position.
};

/**
 * @brief OpenGL-based text drawer using per-character textures (or SDFs).
 *
 * Builds a simple pipeline to render strings as textured quads. The map
 * @ref chars holds per-glyph textures; @ref sdChars stores signed-distance
 * variants for crisp scaling when @ref renderAsSignedDistanceField is enabled.
 */
class FontEngine {
public:
  /** @brief Create an empty engine and initialize shaders, quad geometry, and VBOs. */
  FontEngine();
  virtual ~FontEngine() {}

  /**
   * @name Draw text
   */
  ///@{
  /**
   * @brief Render a UTF-8 string.
   * @param text      String to draw (unknown chars are rendered as '_').
   * @param winAspect Window aspect ratio (width/height) used for pixel-correct scaling.
   * @param height    Normalized line height in NDC (Y span of one line).
   * @param pos       Anchor position in NDC ([-1,1]²).
   * @param a         Alignment relative to @p pos.
   * @param color     RGBA color multiplier.
   */
  void render(const std::string& text, float winAspect, float height,
              const Vec2& pos, Alignment a = Alignment::Center, const Vec4& color=Vec4{1.0f,1.0f,1.0f,1.0f});
  /** @brief Render with fixed total width (height adapts to keep aspect). */
  void renderFixedWidth(const std::string& text, float winAspect, float width,
                        const Vec2& pos, Alignment a = Alignment::Center, const Vec4& color=Vec4{1.0f,1.0f,1.0f,1.0f});

  /** @brief Convenience overloads rendering an integer number. */
  void render(uint32_t number, float winAspect, float height,
              const Vec2& pos, Alignment a = Alignment::Center, const Vec4& color=Vec4{1.0f,1.0f,1.0f,1.0f});
  void renderFixedWidth(uint32_t number, float winAspect, float width,
                        const Vec2& pos, Alignment a = Alignment::Center, const Vec4& color=Vec4{1.0f,1.0f,1.0f,1.0f});
  ///@}

  /** @name Layout helpers */
  ///@{
  /** @brief Expected on-screen size of @p text for a given @p height. */
  Vec2 getSize(const std::string& text, float winAspect, float height) const;
  /** @brief Expected on-screen size of @p text for a fixed @p width. */
  Vec2 getSizeFixedWidth(const std::string& text, float winAspect, float width) const;
  /** @brief Size helpers for integer numbers. */
  Vec2 getSize(uint32_t number, float winAspect, float height) const;
  Vec2 getSizeFixedWidth(uint32_t number, float winAspect, float width) const;
  ///@}

  /** @brief Return the available characters as a single concatenated string. */
  std::string getAllCharsString() const;

  std::map<char,CharTex> chars;  ///< Per-character textures + transforms.
  std::map<char,CharTex> sdChars;///< Per-character SDF textures + transforms.

  /** @brief Enable/disable signed distance field rendering. */
  void setRenderAsSignedDistanceField(bool renderAsSignedDistanceField) {
    this->renderAsSignedDistanceField = renderAsSignedDistanceField;
  }

  /** @brief Current SDF rendering flag. */
  bool getRenderAsSignedDistanceField() const {
    return renderAsSignedDistanceField;
  }

private:
  GLProgram simpleProg;   ///< Shader for alpha-blended bitmap glyphs.
  GLProgram simpleDistProg; ///< Shader for signed-distance glyph rendering.
  GLArray   simpleArray;  ///< VAO for a unit quad.
  GLBuffer  simpleVb;     ///< VBO with quad positions/UVs.
  bool renderAsSignedDistanceField; ///< If true, use @ref sdChars and distance shader.
};

/**
 * @brief CPU-side text compositor using a bitmap atlas and character boxes.
 *
 * Loads an atlas image and a set of @ref CharPosition entries (from file or
 * provided vector). The @ref render() functions blit glyph rectangles into a
 * new @ref Image placed tightly around the requested string or number.
 */
class FontRenderer {
public:
  /**
   * @brief Construct from filenames.
   * @param imageFilename     Path to the bitmap font image (see @ref BMP::load()).
   * @param positionFilename  Path to a whitespace-separated list of character boxes.
   *
   * The positions file expects lines of the form
   * "<char> <x0> <y0> <x1> <y1>" (or a variant with an explicit placeholder).
   */
  FontRenderer(const std::string& imageFilename,
               const std::string& positionFilename);

  /** @brief Construct from an in-memory font image and a positions file. */
  FontRenderer(const Image& fontImage,
               const std::string& positionFilename);

  /** @brief Construct directly from an image and a vector of positions. */
  FontRenderer(const Image& fontImage,
               const std::vector<CharPosition>& positions);

  /**
   * @brief Compose a new image containing @p text using the atlas glyphs.
   * @return An @ref Image with width = sum of glyph widths and height = max glyph height.
   */
  Image render(const std::string& text) const;
  /** @brief Compose a new image containing the decimal representation of @p number. */
  Image render(uint32_t number) const;

  /**
   * @brief Load character rectangles from a text file.
   * @param positionFilename Path to the positions file.
   * @return Parsed vector of @ref CharPosition entries.
   */
  static const std::vector<CharPosition> loadPositions(const std::string& positionFilename);

  /**
   * @brief Emit C++ code to recreate the atlas image and positions vector.
   * @param varName Prefix used for generated symbols (e.g., "myFont").
   */
  std::string toCode(const std::string& varName) const;

  /**
   * @brief Create a GPU font engine initialized from this bitmap font.
   * @return Shared pointer to a @ref FontEngine with per-character textures.
   */
  std::shared_ptr<FontEngine> generateFontEngine() const;

private:
  Image fontImage;                         ///< Atlas image containing all glyphs.
  std::vector<CharPosition> positions;     ///< Character rectangles in @ref fontImage.

  /**
   * @brief Find the atlas rectangle for character @p c.
   * @return Reference to the matching @ref CharPosition; defaults to the first entry if absent.
   */
  const CharPosition& findElement(char c) const;
};

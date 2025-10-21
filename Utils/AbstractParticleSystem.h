#pragma once

#include "Vec3.h"
#include "Mat4.h"

#include "GLProgram.h"
#include "GLBuffer.h"
#include "GLArray.h"
#include "GLTexture2D.h"

/**
 * @file AbstractParticleSystem.h
 * @brief Base interface and minimal GL pipeline for point‑sprite particle systems.
 *
 * Declares @ref AbstractParticleSystem, an abstract class that provides a
 * small OpenGL rendering path for particles drawn as additive point sprites.
 * Subclasses implement @ref update(), @ref setColor(), @ref getData(), and
 * @ref getParticleCount() to define particle behavior and supply per‑vertex
 * attributes. The renderer expects interleaved per‑particle data in the form
 * \c [x,y,z,r,g,b,a] (7 floats).
 *
 * The implementation creates a tiny circular sprite texture and a shader that
 * computes perspective‑correct point sizes from a reference depth, so the same
 * visual size can be preserved across varying distances.
 */

/**
 * @name Special color sentinels
 * Constants that tell the system to compute colors procedurally.
 */
///@{
/** @brief Sentinel that requests a random RGB color per particle. */
const Vec3 RANDOM_COLOR{-1.0f,-1.0f,-1.0f};
/** @brief Sentinel that can be used by subclasses to generate a rainbow map. */
const Vec3 RAINBOW_COLOR{-2.0f,-2.0f,-2.0f};
///@}

/**
 * @brief Abstract additive point‑sprite particle system with a ready‑made GL path.
 *
 * The class owns a small shader program, a point‑sprite texture, and a dynamic
 * VBO/VAO pair. Derived classes only need to supply the particle data packed as
 * (x,y,z,r,g,b,a) and report the particle count. Colors are multiplied in the
 * fragment stage and blended additively (ONE, ONE).
 */
class AbstractParticleSystem {
public:
  /**
   * @brief Construct the renderer state.
   * @param pointSize Desired point size at @p refDepth (in pixels).
   * @param refDepth  Reference eye‑space depth at which @p pointSize applies.
   */
  AbstractParticleSystem(float pointSize, float refDepth=1.0f);
  /** @brief Virtual destructor. */
  virtual ~AbstractParticleSystem() {}

  /**
   * @brief Advance simulation by @p t seconds (override in derived classes).
   */
  virtual void update(float t) = 0;

  /**
   * @brief Set the base point size and reference depth used by the shader.
   * @param pointSize Desired apparent size in pixels at @p refDepth.
   * @param refDepth  Eye‑space depth at which @p pointSize holds (z of clip‑space before division).
   */
  void setPointSize(float pointSize, float refDepth=1.0f) {
    this->pointSize = pointSize;
    this->refDepth = refDepth;
  }
  /** @brief Current point size at the configured reference depth. */
  float getPointSize() const {return pointSize;}
  /** @brief Current reference depth for size computation. */
  float getRefDepth() const {return refDepth;}

  /**
   * @brief Set a global color used by the emitter (override in derived classes).
   *
   * Implementations may treat @ref RANDOM_COLOR and @ref RAINBOW_COLOR as
   * special cases. See also @ref computeColor().
   */
  virtual void setColor(const Vec3& color) = 0;

  /**
   * @brief Render all particles using the internal program and point sprite.
   * @param v View matrix.
   * @param p Projection matrix.
   *
   * Uploads current particle data (via @ref getData()) to a dynamic VBO, sets
   * uniforms, and draws a GL_POINTS array with additive blending. Returns
   * immediately if @ref getParticleCount() is zero.
   */
  void render(const Mat4& v, const Mat4& p);

  /**
   * @name Data interface (to implement in subclasses)
   */
  ///@{
  /**
   * @brief Return interleaved particle attributes.
   *
   * The vector must contain 7 floats per particle in the order
   * (x,y,z,r,g,b,a).
   */
  virtual std::vector<float> getData() const = 0;
  /** @brief Number of particles to draw. */
  virtual size_t getParticleCount() const = 0;
  ///@}

  /**
   * @brief Interpret special color sentinel values.
   * @param c Input color; if equal to @ref RANDOM_COLOR a random color is
   *          returned; otherwise the input is forwarded unchanged.
   */
  static Vec3 computeColor(const Vec3& c);

private:
  float pointSize;   ///< Point size at @ref refDepth.
  float refDepth;    ///< Reference eye‑space depth used for size scaling.

  // --- GL resources (created/owned by the renderer) ---
  GLProgram  prog;        ///< Point‑sprite shader program (pos+color).
  GLint      mvpLocation; ///< Uniform location for MVP matrix.
  GLint      ppLocation;  ///< Uniform location for (pointSize, refDepth).
  GLint      texLocation; ///< Uniform location for the sprite sampler.
  GLTexture2D sprite;     ///< Small circular mask used for the sprite.

  GLArray particleArray;  ///< VAO describing the interleaved layout.
  GLBuffer vbPosColor;    ///< Dynamic VBO holding (x,y,z,r,g,b,a) per particle.
};

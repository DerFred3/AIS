#pragma once

#define LNC_PI 3.14159265358979323846
#define LNC_RADIANS(deg) (deg * LNC_PI / 180.0)

#include "Vec3.h"
#include "Mat4.h"

/**
 * @file Camera.h
 * @brief Minimal FPS-style camera with yaw/pitch look and WASD-style motion.
 *
 * Maintains position and an orthonormal basis (direction/right/up) derived from
 * Euler yaw/pitch angles. The camera supports continuous motion toggled via
 * boolean flags (front/back/left/right) and optional mouse-look. The view
 * matrix is produced with a standard look-at helper.
 *
 * Usage:
 * @code
 * Camera cam({0,0,3});
 * cam.enableMouse();
 * // Per-event:
 * cam.mouseMove(x, y);     // when mouse moves
 * cam.moveFront(true);     // on key press; set to false on release
 * cam.updatePosition();    // once per frame
 * Mat4 V = cam.viewMatrix();
 * @endcode
 */
class Camera {
  Vec3 position;   ///< Camera world position.
  Vec3 direction;  ///< Forward (view) direction (unit length).
  Vec3 right, up;  ///< Orthonormal right and up vectors.
  Vec3 worldUp;    ///< World up used to derive the basis.

  float yaw{ -90.0f };  ///< Yaw angle in degrees (0 = +X, -90 = -Z by convention).
  float pitch{ 0.0f };  ///< Pitch angle in degrees (clamped to [-89,+89]).

  float moveSpeed;      ///< Linear speed per update (world units/frame).
  bool movingFront{ false }; ///< If true, advance along @ref direction on update.
  bool movingBack{ false };  ///< If true, retreat along @ref direction on update.
  bool movingLeft{ false };  ///< If true, move along -@ref right on update.
  bool movingRight{ false }; ///< If true, move along +@ref right on update.

  float mouseSens;      ///< Mouse sensitivity scale (deg/pixel).
  float mouseX{ -1.0f }, mouseY{ -1.0f }; ///< Last mouse position (init → invalid).
  bool mouseEnabled{ false };              ///< Enable/disable processing in @ref mouseMove().

  /**
   * @brief Recompute direction/right/up from current yaw/pitch and @ref worldUp.
   *
   * Uses standard spherical mapping with yaw around world up and pitch around
   * the camera right. All vectors are normalized.
   */
  void updateDirection() {
    float dirX = cos(LNC_RADIANS(yaw)) * cos(LNC_RADIANS(pitch));
    float dirY = sin(LNC_RADIANS(pitch));
    float dirZ = sin(LNC_RADIANS(yaw)) * cos(LNC_RADIANS(pitch));
    direction = Vec3::normalize(Vec3(dirX, dirY, dirZ));
    right     = Vec3::normalize(Vec3::cross(direction, worldUp));
    up        = Vec3::normalize(Vec3::cross(right, direction));
  }

public:

  /**
   * @brief Construct a camera at @p position with speeds/sensitivity and an up axis.
   * @param position  Initial world position.
   * @param moveSpeed Linear speed per update (world units per frame).
   * @param mouseSens Mouse sensitivity in degrees per pixel.
   * @param worldUp   World up vector used to build the orthonormal basis.
   */
  Camera(Vec3 position, float moveSpeed = 0.015f, float mouseSens = 0.15f, Vec3 worldUp = Vec3(0.0f, 1.0f, 0.0f)) {
    this->position = position;
    this->moveSpeed = moveSpeed;
    this->mouseSens = mouseSens;
    this->worldUp = worldUp;
    updateDirection();
  }

  /** @name Movement toggles (set on key press/release) */
  ///@{
  /** @brief Toggle forward motion along the view direction. */
  void moveFront(bool b) { movingFront = b; }

  /** @brief Toggle backward motion along the view direction. */
  void moveBack(bool b) { movingBack = b; }

  /** @brief Toggle motion to the right (strafe). */
  void moveRight(bool b) { movingRight = b; }

  /** @brief Toggle motion to the left (strafe). */
  void moveLeft(bool b) { movingLeft = b; }
  ///@}

  /** @name Mouse look */
  ///@{
  /** @brief Enable mouse-look processing in @ref mouseMove(). */
  void enableMouse() { mouseEnabled = true; }

  /**
   * @brief Disable mouse-look and reset the internal cursor cache.
   *
   * Resets the cached mouse position so the next @ref mouseMove() call
   * establishes a new reference without producing a jump.
   */
  void disableMouse() {
    mouseEnabled = false;
    mouseX = mouseY = -1.0f;
  }

  /**
   * @brief Update yaw/pitch from a new cursor position.
   * @param xPosition New cursor X in window coordinates (pixels).
   * @param yPosition New cursor Y in window coordinates (pixels).
   *
   * When enabled, computes pixel deltas since the previous call, scales them
   * by @ref mouseSens, applies them to yaw/pitch, clamps pitch to [-89,89],
   * and rebuilds the basis. The first call after enabling only seeds the cache.
   */
  void mouseMove(float xPosition, float yPosition) {
    if (!mouseEnabled) {
      return;
    }
    else if (mouseX == -1.0f && mouseY == -1.0f) {
      mouseX = xPosition;
      mouseY = yPosition;
      return;
    }

    float xOffset = (xPosition - mouseX) * mouseSens;
    float yOffset = (mouseY - yPosition) * mouseSens;

    mouseX = xPosition;
    mouseY = yPosition;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f) pitch = 89.0f;
    else if (pitch < -89.0f) pitch = -89.0f;

    updateDirection();
  }
  ///@}

  /**
   * @brief Integrate position one step based on active movement flags.
   *
   * Applies @ref moveSpeed along @ref direction and @ref right depending on
   * which toggles are set. Call this once per frame.
   */
  void updatePosition() {
    if (movingFront) position = position + (direction * moveSpeed);
    if (movingBack)  position = position - (direction * moveSpeed);
    if (movingRight) position = position + (right * moveSpeed);
    if (movingLeft)  position = position - (right * moveSpeed);
  }

  /**
   * @brief Compute the view (camera) matrix.
   * @return Right-handed look-at matrix from @ref position toward forward @ref direction.
   */
  Mat4 viewMatrix() {
    return Mat4::lookAt(position, position + direction, up);
  }
};

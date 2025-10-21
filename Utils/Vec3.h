#pragma once

#include <ostream>
#include <sstream>
#include <string>
#include <array>
#include <cmath>
#include <optional>

#include "Rand.h"
#include "Vec2.h"

/**
 * @file Vec3.h
 * @brief 3D vector template with arithmetic, geometry helpers, and RNG utilities.
 *
 * This header defines a compact 3D vector type template, \c Vec3t<T>, that stores
 * three components and exposes multiple alias views (x/y/z and r/g/b as well as
 * an \c xy sub-vector). It provides arithmetic operators, length helpers, and
 * common geometric operations (dot, cross, reflect, refract) useful in graphics
 * and simulation code.
 *
 * @note The implementation uses a \c union with multiple member layouts (an
 *       std::array, two structs for aliases, and a 2D sub-vector). This is
 *       intentional for ergonomic access. Access only one active view at a time.
 */

template <typename T>
union Vec3t {
public:
  /** @brief Component storage and named aliases. */
  std::array<T, 3> e;   ///< Indexed access: e[0], e[1], e[2].
  struct { T x; T y; T z; }; ///< Cartesian aliases for components.
  struct { T r; T g; T b; }; ///< Color aliases for components.
  Vec2t<T> xy;                ///< 2D view of the first two components.

  /** @brief Default-construct to (0,0,0). */
  Vec3t() :
  e{0,0,0}
  {}

  /**
   * @brief Construct from three component values.
   * @param x First component.
   * @param y Second component.
   * @param z Third component.
   */
  Vec3t(T x, T y, T z) :
  e{x,y,z}
  {}

  /**
   * @brief Copy-construct from the same type.
   * @param other Source vector to copy.
   */
  Vec3t(const Vec3t& other) :
  e{other.x, other.y, other.z}
  {}

  /**
   * @brief Construct from a 2D vector and a z value.
   * @param other Source 2D vector for x and y.
   * @param z     Third component.
   */
  Vec3t(const Vec2t<T>& other, T z) :
  e{other.x, other.y, z}
  {}

  /**
   * @brief Explicitly convert from another \c Vec3t with different component type.
   * @tparam U Source component type.
   * @param other Source vector to convert from.
   */
  template <typename U>
  explicit Vec3t(const Vec3t<U>& other) :
  e{T(other.x), T(other.y), T(other.z)}
  {}

  /**
   * @brief Explicitly convert from a 2D vector of a different type and a z value.
   * @tparam U Source component type.
   * @param other Source 2D vector for x and y.
   * @param z     Third component.
   */
  template <typename U>
  explicit Vec3t(const Vec2t<U>& other, T z) :
  e{T(other.x), T(other.y), z}
  {}

  /**
   * @brief String representation "[x, y, z]".
   * @return A new std::string describing the vector.
   */
  const std::string toString() const {
    std::stringstream s;
    s << "[" << e[0] << ", " << e[1] << ", " << e[2] << "]";
    return s.str();
  }

  /**
   * @brief Component-wise addition.
   * @param val Vector to add.
   * @return Result of this + val.
   */
  Vec3t operator+(const Vec3t& val) const {
    return Vec3t{e[0]+val.e[0],
      e[1]+val.e[1],
      e[2]+val.e[2]};
  }

  /**
   * @brief Component-wise subtraction.
   * @param val Vector to subtract.
   * @return Result of this - val.
   */
  Vec3t operator-(const Vec3t& val) const {
    return Vec3t{e[0]-val.e[0],
      e[1]-val.e[1],
      e[2]-val.e[2]};
  }

  /**
   * @brief Component-wise multiplication.
   * @param val Vector to multiply component-wise.
   * @return Result of this * val.
   */
  Vec3t operator*(const Vec3t& val) const{
    return Vec3t{e[0]*val.e[0],
      e[1]*val.e[1],
      e[2]*val.e[2]};
  }

  /**
   * @brief Component-wise division.
   * @param val Vector to divide by component-wise.
   * @return Result of this / val.
   */
  Vec3t operator/(const Vec3t& val) const{
    return Vec3t{e[0]/val.e[0],
      e[1]/val.e[1],
      e[2]/val.e[2]};
  }

  /**
   * @brief Scalar multiplication.
   * @param val Scalar factor.
   * @return Result of this * val.
   */
  Vec3t operator*(const T& val) const {
    return {e[0]*val,e[1]*val,e[2]*val};
  }

  /**
   * @brief Scalar division.
   * @param val Scalar divisor.
   * @return Result of this / val.
   */
  Vec3t operator/(const T& val) const{
    return {e[0]/val,e[1]/val,e[2]/val};
  }

  /**
   * @brief Add scalar to all components.
   * @param val Scalar to add.
   * @return Result of this + val.
   */
  Vec3t operator+(const T& val) const {
    return {e[0]+val,e[1]+val,e[2]+val};
  }

  /**
   * @brief Subtract scalar from all components.
   * @param val Scalar to subtract.
   * @return Result of this - val.
   */
  Vec3t operator-(const T& val) const{
    return {e[0]-val,e[1]-val,e[2]-val};
  }

  /**
   * @brief Equality comparison (component-wise equality).
   * @param other Vector to compare against.
   * @return True iff all components are equal.
   */
  bool operator == (const Vec3t& other) const {
    return e[0] == other.e[0] &&
    e[1] == other.e[1] &&
    e[2] == other.e[2];
  }

  /**
   * @brief Inequality comparison (component-wise inequality).
   * @param other Vector to compare against.
   * @return True if any component differs.
   */
  bool operator != (const Vec3t& other) const {
    return e[0] != other.e[0] ||
    e[1] != other.e[1] ||
    e[2] != other.e[2];
  }

  /**
   * @brief Euclidean length (magnitude).
   * @return sqrt(x*x + y*y + z*z).
   */
  T length() const {
    return sqrt(sqlength());
  }

  /**
   * @brief Squared length (avoids sqrt).
   * @return x*x + y*y + z*z.
   */
  T sqlength() const {
    return e[0]*e[0]+e[1]*e[1]+e[2]*e[2];
  }

  /**
   * @brief Implicit pointer conversion to raw component storage.
   * @return Pointer to first component (non-const overload).
   * @warning Raw pointer exposes internal layout; use with care.
   */
  operator T*(void) {return e.data();}

  /**
   * @brief Implicit pointer conversion to raw component storage (const).
   * @return Pointer to first component (const overload).
   */
  operator const T*(void) const  {return e.data();}

  /**
   * @brief Dot product of two vectors.
   * @param a First vector.
   * @param b Second vector.
   * @return a·b.
   */
  static float dot(const Vec3t& a, const Vec3t& b) {
    return a.e[0]*b.e[0]+a.e[1]*b.e[1]+a.e[2]*b.e[2];
  }

  /**
   * @brief Cross product of two vectors.
   * @param a First vector.
   * @param b Second vector.
   * @return a × b.
   */
  static Vec3t cross(const Vec3t& a, const Vec3t& b) {
    return Vec3t{a.e[1] * b.e[2] - a.e[2] * b.e[1],
      a.e[2] * b.e[0] - a.e[0] * b.e[2],
      a.e[0] * b.e[1] - a.e[1] * b.e[0]};
  }

  /**
   * @brief Return the normalized (unit-length) version of a vector.
   * @param a Input vector.
   * @return a / |a| if |a| != 0, otherwise (0,0,0).
   */
  static Vec3t normalize(const Vec3t& a) {
    const float l = a.length();
    return (l != 0.0f) ? a/T(l) : Vec3t{0,0,0};
  }

  /**
   * @brief Reflect a vector about a unit normal.
   * @param a Incident vector.
   * @param n Unit surface normal (assumed normalized).
   * @return Reflected vector r = a - 2(n·a)n.
   */
  static Vec3t reflect(const Vec3t& a, const Vec3t& n) {
    return a-n*dot(a,n)*T(2);
  }

  /**
   * @brief Refract a vector through a surface with index-of-refraction ratio.
   *
   * Computes the transmitted direction using Snell's law. If total internal
   * reflection occurs, returns an empty optional.
   *
   * @param a     Incident vector (assumed pointing towards the surface).
   * @param normal Surface normal (assumed normalized, pointing away from surface).
   * @param IOR   Index-of-refraction of the material (n\_material / n\_outside).
   * @return Transmitted direction if refraction is possible; otherwise empty.
   */
  static std::optional<Vec3t> refract(const Vec3t& a, const Vec3t& normal, const T IOR) {
    const T cosI = Vec3t::dot(a, normal);
    int sign = (cosI < 0) ? -1 : 1;

    // we assume that if we look from the back side we are exiting the material
    // back side means that the sign/cosI is positive because the incoming ray
    // is assumed to point
    // towards the surface and the normal away
    const T n = (sign == 1) ? IOR : 1.0f / IOR;
    const T sinThetaSq = n * n * (1.0f - cosI * cosI);

    if (sinThetaSq > 1.0f) {
      // Total internal reflection
      return {};
    } else {
      const Vec3t d = a * n;
      const T c = n * cosI -sign * sqrt(1.0f - sinThetaSq);
      const Vec3t b = normal * c;
      return d - b;
    }
  }

  /**
   * @brief Component-wise minimum of two vectors.
   * @param a First vector.
   * @param b Second vector.
   * @return {min(a.x,b.x), min(a.y,b.y), min(a.z,b.z)}.
   */
  static Vec3t minV(const Vec3t& a, const Vec3t& b) {
    return {std::min(a.x,b.x), std::min(a.y,b.y), std::min(a.z,b.z)};
  }

  /**
   * @brief Component-wise maximum of two vectors.
   * @param a First vector.
   * @param b Second vector.
   * @return {max(a.x,b.x), max(a.y,b.y), max(a.z,b.z)}.
   */
  static Vec3t maxV(const Vec3t& a, const Vec3t& b) {
    return {std::max(a.x,b.x), std::max(a.y,b.y), std::max(a.z,b.z)};
  }

  /**
   * @brief Create a vector with components sampled from [0,1).
   * @return Random vector (x,y,z) in [0,1) each.
   * @note Depends on a global/namespace-scope RNG named \c staticRand
   *       provided by @c Rand.h .
   */
  static Vec3t<float> random() {
    return {staticRand.rand01(),staticRand.rand01(),staticRand.rand01()};
  }

  /**
   * @brief Uniform random point inside the unit sphere.
   * @return Random point with |p| <= 1.
   */
  static Vec3t<float> randomPointInSphere() {
    while (true) {
      Vec3t<float> p{staticRand.rand11(),staticRand.rand11(),staticRand.rand11()};
      if (p.sqlength() > 1) continue;
      return p;
    }
  }

  /**
   * @brief Uniform random point inside the upper unit hemisphere (z>=0).
   * @return Random point with |p| <= 1 and z >= 0.
   */
  static Vec3t<float> randomPointInHemisphere() {
    while (true) {
      Vec3t<float> p{staticRand.rand01(),staticRand.rand01(),staticRand.rand01()};
      if (p.sqlength() > 1) continue;
      return p;
    }
  }

  /**
   * @brief Uniform random point inside the unit disc on the xy-plane.
   * @return Random point with x^2 + y^2 <= 1 and z = 0.
   */
  static Vec3t<float> randomPointInDisc() {
    while (true) {
      Vec3t<float> p{staticRand.rand11(),staticRand.rand11(),0};
      if (p.sqlength() > 1) continue;
      return p;
    }
  }

  /**
   * @brief Uniform random unit vector on the sphere.
   * @return Unit-length random direction.
   */
  static Vec3t<float> randomUnitVector() {
    const float a = staticRand.rand0Pi();
    const float z = staticRand.rand11();
    const float r = sqrt(1.0f - z*z);
    return {r*cosf(a), r*sinf(a), z};
  }

  /**
   * @brief Clamp each component to [minVal, maxVal].
   * @param val    Input vector.
   * @param minVal Lower bound.
   * @param maxVal Upper bound.
   * @return New vector with components clamped to the given range.
   */
  static Vec3t<float> clamp(const Vec3t& val, float minVal, float maxVal) {
    return { std::clamp(val.x, minVal, maxVal),
      std::clamp(val.y, minVal, maxVal),
      std::clamp(val.z, minVal, maxVal)
    };
  }
};

/**
 * @brief Stream insertion for string representation of a vector.
 * @tparam T Component type.
 * @param os Output stream.
 * @param v  Vector to print.
 * @return Reference to the output stream.
 */
template <typename T>
std::ostream & operator<<(std::ostream & os, const Vec3t<T> & v) {
  os << v.toString();
  return os;
}

/**
 * @brief Scalar multiplication with scalar on the left.
 * @tparam T Component type of vector.
 * @param scalar Scalar factor.
 * @param vec    Vector to multiply.
 * @return Result of vec * scalar.
 */
template <typename T>
Vec3t<T> operator*(float scalar, const Vec3t<T>& vec) {
  return vec * scalar;
}

/** @brief 3D float vector (x,y,z). */
typedef Vec3t<float> Vec3;
/** @brief 3D 32-bit signed integer vector (x,y,z). */
typedef Vec3t<int32_t> Vec3i;
/** @brief 3D 32-bit unsigned integer vector (x,y,z). */
typedef Vec3t<uint32_t> Vec3ui;

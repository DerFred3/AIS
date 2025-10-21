#pragma once

#include <ostream>
#include <string>
#include <array>
#include <sstream>
#include <cmath>

#include "Rand.h"

/**
 * @file Vec2.h
 * @brief 2D vector template with arithmetic, length utilities, and helpers.
 *
 * This header defines a simple 2D vector type template, \c Vec2t<T>, that stores
 * two components and exposes multiple alias views (x/y and r/g). It provides
 * basic arithmetic operators, scalar operations, length helpers, and a few
 * convenience utilities (random generation, normalization, clamping) suitable
 * for lightweight math and graphics code.
 *
 * @note The implementation uses a \c union with multiple member layouts
 *       (an std::array and two structs for aliases). This is intentionally done
 *       for ergonomic access (e.g., \c v.x vs. \c v.e[0]).
 */

template <typename T>
union Vec2t {
public:
  /** @brief Component storage and named aliases. */
  std::array<T, 2> e;   ///< Indexed access: e[0] and e[1].
  struct { T x; T y; }; ///< Cartesian alias for components.
  struct { T r; T g; }; ///< Color alias for components.

  /**
   * @brief Default-construct to (0, 0).
   */
  Vec2t() :
  e{0,0}
  {}

  /**
   * @brief Construct from two component values.
   * @param x First component.
   * @param y Second component.
   */
  Vec2t(T x, T y) :
  e{x,y}
  {}

  /**
   * @brief Copy-construct from another vector of the same type.
   * @param other Source vector to copy from.
   */
  Vec2t(const Vec2t& other) :
  e{other.x, other.y}
  {}

  /**
   * @brief Explicitly convert from another \c Vec2t with different component type.
   * @tparam U Source component type.
   * @param other Source vector to convert from.
   */
  template <typename U>
  explicit Vec2t(const Vec2t<U>& other) :
  e{T(other.x), T(other.y)}
  {}

  /**
   * @brief String representation "[x, y]".
   * @return A new std::string describing the vector.
   */
  const std::string toString() const {
    std::stringstream s;
    s << "[" << e[0] << ", " << e[1] << "]";
    return s.str();
  }

  /**
   * @brief Component-wise addition.
   * @param val Vector to add.
   * @return Result of this + val.
   */
  Vec2t operator+(const Vec2t& val) const{
    return {e[0]+val.e[0],e[1]+val.e[1]};
  }

  /**
   * @brief Component-wise subtraction.
   * @param val Vector to subtract.
   * @return Result of this - val.
   */
  Vec2t operator-(const Vec2t& val) const {
    return {e[0]-val.e[0],e[1]-val.e[1]};
  }

  /**
   * @brief Component-wise multiplication.
   * @param val Vector to multiply component-wise.
   * @return Result of this * val.
   */
  Vec2t operator*(const Vec2t& val) const {
    return {e[0]*val.e[0],e[1]*val.e[1]};
  }

  /**
   * @brief Component-wise division.
   * @param val Vector to divide by component-wise.
   * @return Result of this / val.
   */
  Vec2t operator/(const Vec2t& val) const {
    return {e[0]/val.e[0],e[1]/val.e[1]};
  }

  /**
   * @brief Scalar multiplication.
   * @param val Scalar factor.
   * @return Result of this * val.
   */
  Vec2t operator*(const T& val) const {
    return {e[0]*val,e[1]*val};
  }

  /**
   * @brief Scalar division.
   * @param val Scalar divisor.
   * @return Result of this / val.
   */
  Vec2t operator/(const T& val) const {
    return {e[0]/val, e[1]/val};
  }

  /**
   * @brief Add scalar to both components.
   * @param val Scalar to add.
   * @return Result of this + val.
   */
  Vec2t operator+(const T& val) const {
    return {e[0]+val,e[1]+val};
  }

  /**
   * @brief Subtract scalar from both components.
   * @param val Scalar to subtract.
   * @return Result of this - val.
   */
  Vec2t operator-(const T& val) const{
    return {e[0]-val,e[1]-val};
  }

  /**
   * @brief Equality comparison (component-wise equality).
   * @param other Vector to compare against.
   * @return True if components are equal.
   */
  bool operator == ( const Vec2t& other ) const {
    return e == other.e;
  }

  /**
   * @brief Inequality comparison (component-wise inequality).
   * @param other Vector to compare against.
   * @return True if any component differs.
   */
  bool operator != ( const Vec2t& other ) const {
    return e != other.e;
  }

  /**
   * @brief Euclidean length (magnitude).
   * @return sqrt(x*x + y*y).
   */
  T length() const {
    return sqrt(sqlength());
  }

  /**
   * @brief Squared length (avoids sqrt).
   * @return x*x + y*y.
   */
  T sqlength() const {
    return e[0]*e[0]+e[1]*e[1];
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
   * @brief Create a vector with components sampled from [0,1).
   * @return Random vector (x,y) in [0,1) each.
   * @note Depends on a global/namespace-scope RNG named \c staticRand
   *       provided by @c Rand.h .
   */
  static Vec2t random() {
    return Vec2t{T{staticRand.rand01()},T{staticRand.rand01()}};
  }

  /**
   * @brief Return the normalized (unit-length) version of a vector.
   * @param a Input vector.
   * @return a / |a| if |a| != 0, otherwise (0,0).
   */
  static Vec2t normalize(const Vec2t& a) {
    const float l = a.length();
    return (l != T(0)) ? a/l : Vec2t{T(0),T(0)};
  }

  /**
   * @brief Clamp both components to [minVal, maxVal].
   * @param val    Input vector.
   * @param minVal Lower bound.
   * @param maxVal Upper bound.
   * @return New vector with each component clamped to the given range.
   * @note The return type is specialized as \c Vec2t<float>.
   */
  static Vec2t<float> clamp(const Vec2t& val, float minVal, float maxVal) {
    return { std::clamp(val.x, minVal, maxVal),
      std::clamp(val.y, minVal, maxVal)
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
std::ostream & operator<<(std::ostream & os, const Vec2t<T> & v) {
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
Vec2t<T> operator*(float scalar, const Vec2t<T>& vec) {
  return vec * scalar;
}

/** @brief Float vector (x,y). */
typedef Vec2t<float> Vec2;
/** @brief 32-bit signed integer vector (x,y). */
typedef Vec2t<int32_t> Vec2i;
/** @brief 32-bit unsigned integer vector (x,y). */
typedef Vec2t<uint32_t> Vec2ui;

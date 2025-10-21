#pragma once

#include <ostream>
#include <string>
#include <array>
#include <sstream>
#include <cmath>

#include "Vec3.h"

/**
 * @file Mat3.h
 * @brief 3×3 matrix template for rotations, scaling, and basic linear algebra.
 *
 * Provides a compact column-major 3×3 matrix type, \c Mat3t<T>, with scalar
 * arithmetic, matrix–matrix and matrix–vector multiplication, constructors for
 * axis-aligned scaling and axis rotations (X/Y/Z), as well as transpose,
 * determinant, and inverse helpers.
 *
 * @note Layout: elements are stored in a flat \c std::array<T,9> named \c e in
 *       column-major order (OpenGL-style). Operators and factory methods assume
 *       this layout.
 */

template <typename T>
class Mat3t {
public:
  /** @brief Construct the identity matrix. */
  Mat3t():
  Mat3t{1,0,0,
    0,1,0,
    0,0,1}
  {}

  /**
   * @brief Construct from 9 elements (column-major order).
   * @param m11..m33 Elements of the matrix in column-major order.
   */
  Mat3t(T m11, T m12, T m13,
        T m21, T m22, T m23,
        T m31, T m32, T m33):
  e{m11,m12,m13,
    m21,m22,m23,
    m31,m32,m33}
  {
  }

  /**
   * @brief Construct from a raw array of 9 elements.
   * @param e Array in column-major order.
   */
  Mat3t(const std::array<T, 9>& e):
  e{e}
  {
  }

  /**
   * @brief Construct from three column vectors.
   * @param e1 First column (x,y,z).
   * @param e2 Second column (x,y,z).
   * @param e3 Third column (x,y,z).
   */
  Mat3t(const Vec3t<T>& e1,
        const Vec3t<T>& e2,
        const Vec3t<T>& e3):
  Mat3t(e1.x,e1.y,e1.z,
        e2.x,e2.y,e2.z,
        e3.x,e3.y,e3.z)
  {
  }

  /** @brief Copy-construct from another matrix. */
  Mat3t(const Mat3t& other):
  e{other.e}
  {
  }

  /**
   * @brief String representation (pretty-printed rows).
   * @return New string describing the matrix.
   */
  const std::string toString() const{
    std::stringstream s;
    s << "[" << e[0] << ", " << e[1] << ", " << e[2] << std::endl <<
    " " << e[3] << ", " << e[4] << ", " << e[5] << std::endl <<
    " " << e[6] << ", " << e[7] << ", " << e[8] <<  "]";
    return s.str();
  }

  /**
   * @brief Stream insertion using @ref toString().
   */
  friend std::ostream& operator<<(std::ostream &os, const Mat3t& m)  {
    os << m.toString() ; return os;
  }

  /**
   * @brief Implicit conversion to raw pointer (non-const).
   * @return Pointer to the underlying \c e[0].
   * @warning Exposes internal layout; use with care.
   */
  operator T*(void){
    return e.data();
  }

  /**
   * @brief Implicit conversion to raw pointer (const).
   * @return Const pointer to the underlying \c e[0].
   */
  operator const T*(void) const{
    return e.data();
  }

  // ===== Scalar arithmetic =====
  /** @brief Component-wise multiplication by scalar. */
  Mat3t operator * ( T scalar ) const{
    return Mat3t{e[0]*scalar,e[1]*scalar,e[2]*scalar,
      e[3]*scalar,e[4]*scalar,e[5]*scalar,
      e[6]*scalar,e[7]*scalar,e[8]*scalar};
  }
  /** @brief Component-wise addition of scalar. */
  Mat3t operator + ( T scalar ) const{
    return Mat3t{e[0]+scalar,e[1]+scalar,e[2]+scalar,e[3]+scalar,
      e[4]+scalar,e[5]+scalar,e[6]+scalar,e[7]+scalar,
      e[8]+scalar};
  }
  /** @brief Component-wise subtraction of scalar. */
  Mat3t operator - ( T scalar ) const{
    return Mat3t{e[0]-scalar,e[1]-scalar,e[2]-scalar,e[3]-scalar,
      e[4]-scalar,e[5]-scalar,e[6]-scalar,e[7]-scalar,
      e[8]-scalar};
  }
  /** @brief Component-wise division by scalar. */
  Mat3t operator / ( T scalar ) const{
    return Mat3t{e[0]/scalar,e[1]/scalar,e[2]/scalar,e[3]/scalar,
      e[4]/scalar,e[5]/scalar,e[6]/scalar,e[7]/scalar,
      e[8]/scalar};
  }

  // ===== Products =====
  /**
   * @brief Matrix–matrix product (column-major; standard linear algebra).
   */
  Mat3t operator * ( const Mat3t& other ) const{
    Mat3t result;
    for (uint8_t x = 0;x<9;x+=3)
      for (uint8_t y = 0;y<3;y++)
        result.e[x+y] = e[0+x] * other.e[0+y]+
        e[1+x] * other.e[3+y]+
        e[2+x] * other.e[6+y];
    return result;
  }

  /**
   * @brief Transform a 3D vector.
   */
  Vec3t<T> operator * ( const Vec3t<T>& other ) const{
    return Vec3t<T>{(other.x*e[0]+other.y*e[1]+other.z*e[2]),
      (other.x*e[3]+other.y*e[4]+other.z*e[5]),
      (other.x*e[6]+other.y*e[7]+other.z*e[8])};
  }

  // ===== Factory helpers =====
  /** @brief Non-uniform scaling by components of \p scale. */
  static Mat3t scaling(const Vec3t<T>& scale){
    return scaling(scale.x, scale.y, scale.z);
  }
  /** @brief Non-uniform scaling by components. */
  static Mat3t scaling(T x, T y, T z){
    return {x, 0, 0,
      0, y, 0,
      0, 0, z};
  }
  /** @brief Rotation about the X-axis by \p degree (degrees). */
  static Mat3t rotationX(T degree){
    const T angle{deg2Rad(degree)};
    const T cosAngle = cosf(angle);
    const T sinAngle = sinf(angle);

    return {1, 0, 0,
      0, cosAngle, sinAngle,
      0, -sinAngle, cosAngle};
  }
  /** @brief Rotation about the Y-axis by \p degree (degrees). */
  static Mat3t rotationY(T degree){
    const T angle{deg2Rad(degree)};
    const T cosAngle{cosf(angle)};
    const T sinAngle{sinf(angle)};

    return {cosAngle, 0, -sinAngle,
      0, 1, 0,
      sinAngle, 0, cosAngle};
  }
  /** @brief Rotation about the Z-axis by \p degree (degrees). */
  static Mat3t rotationZ(T degree){
    const T angle{deg2Rad(degree)};
    const T cosAngle{cosf(angle)};
    const T sinAngle{sinf(angle)};

    return {cosAngle, sinAngle, 0,
      -sinAngle, cosAngle, 0,
      0, 0, 1};
  }

  /** @brief Transpose of a 3×3 matrix. */
  static Mat3t transpose(const Mat3t& m){
    return {m.e[0],m.e[3],m.e[6],
      m.e[1],m.e[4],m.e[7],
      m.e[2],m.e[5],m.e[8]};
  }

  /** @brief Determinant of a 3×3 matrix. */
  static T det(const Mat3t& m){
    return m.e[0]*(m.e[4]*m.e[8]-m.e[5]*m.e[7])-m.e[1]*(m.e[3]*m.e[8]-m.e[5]*m.e[6])+m.e[2]*(m.e[3]*m.e[7]-m.e[4]*m.e[6]);
  }

  /**
   * @brief Matrix inverse with precomputed determinant.
   * @param m Input matrix.
   * @param det Determinant of \p m.
   * @return Inverse of \p m.
   * @warning No zero-check is performed; caller must ensure \p det ≠ 0.
   */
  static Mat3t inverse(const Mat3t& m, T det) {
    T Q = T(1.0/det);

    return {
      (m.e[4]*m.e[8]-m.e[5]*m.e[7])*Q, (m.e[2]*m.e[7]-m.e[1]*m.e[8])*Q, (m.e[1]*m.e[5]-m.e[2]*m.e[4])*Q,
      (m.e[5]*m.e[6]-m.e[3]*m.e[8])*Q, (m.e[0]*m.e[8]-m.e[2]*m.e[6])*Q, (m.e[2]*m.e[3]-m.e[0]*m.e[5])*Q,
      (m.e[3]*m.e[7]-m.e[4]*m.e[6])*Q, (m.e[1]*m.e[6]-m.e[0]*m.e[7])*Q, (m.e[0]*m.e[4]-m.e[1]*m.e[3])*Q
    };
  }

  /**
   * @brief Matrix inverse computed via adjugate and determinant.
   * @param m Input matrix.
   * @return Inverse of \p m (determinant computed internally).
   */
  static Mat3t inverse(const Mat3t& m) {
    return Mat3t::inverse(m, Mat3t::det(m));
  }


private:
  std::array<T, 9> e; ///< Elements in column-major order.

  /** @brief Convert degrees to radians. */
  static T deg2Rad(const T d) {
    return T(3.14159265358979323846)*d/T(180);
  }
};

/** @brief Single-precision 3×3 matrix. */
typedef Mat3t<float> Mat3;

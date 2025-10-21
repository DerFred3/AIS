#pragma once

#include "Vec3.h"
#include "Vec4.h"
#include "Mat4.h"

/**
 * @file ColorConversion.h
 * @brief Color space conversion helpers among RGB/HSV/HSL/CMY/CMYK/YUV.
 *
 * Header‑only collection of small, templated utilities for converting colors
 * between common color spaces. The functions are written for generic scalar
 * component types (e.g., float, double) and operate on the project's vector
 * and matrix types (Vec3t/Vec4t/Mat4t).
 *
 * Unless stated otherwise, inputs are expected in the following ranges:
 *  - RGB/CMY components in [0,1],
 *  - HSV/HSL use hue in degrees [0,360) and saturation/value/lightness in [0,1],
 *  - YUV components follow the simple linear transforms defined below (not BT.601).
 */
namespace ColorConversion {
  /**
   * @brief Convert linear RGB to HSV.
   * @tparam T Scalar type.
   * @param other RGB triplet (r,g,b) in [0,1].
   * @return HSV triplet (h in degrees [0,360), s in [0,1], v in [0,1]).
   */
  template <typename T> Vec3t<T> rgbToHsv(const Vec3t<T>& other) {
    const T minComp = std::min(other.r, std::min(other.g,other.b));
    const T maxComp = std::max(other.r, std::max(other.g,other.b));
    const T delta = maxComp - minComp;

    T h = T(0);
    if (maxComp != minComp) {
      if (maxComp == other.r)
        h = fmod((T(60) * ((other.g - other.b) / delta) + T(360)), T(360));
      else if (maxComp == other.g)
        h = fmod((T(60) * ((other.b - other.r) / delta) + T(120)), T(360));
      else if (maxComp == other.b)
        h = fmod((T(60) * ((other.r - other.g) / delta) + T(240)), T(360));
    }

    const T s = (maxComp == T(0)) ? T(0) : (delta / maxComp);
    const T v = maxComp;

    return {h,s,v};
  }

  /**
   * @brief Convert HSV to linear RGB.
   * @tparam T Scalar type.
   * @param other HSV triplet (h in degrees, s,v in [0,1]).
   * @return RGB triplet in [0,1].
   * @note Hue is wrapped to [0,360) before conversion.
   */
  template <typename T> Vec3t<T> hsvToRgb(const Vec3t<T>& other) {
    const T h = T(int32_t(other.x) % 360) / T(60);
    const T s = std::max(T(0), std::min(T(1), other.y));
    const T v = std::max(T(0), std::min(T(1), other.z));

    if (s == 0) return {v,v,v}; // Achromatic (grey)

    const int32_t i = int32_t(floor(h));
    const T f = h - T(i);
    const T p = v * (T(1) - s);
    const T q = v * (T(1) - s * f);
    const T t = v * (T(1) - s * (T(1) - f));

    switch(i) {
      case 0: return {v,t,p};
      case 1: return {q,v,p};
      case 2: return {p,v,t};
      case 3: return {p,q,v};
      case 4: return {t,p,v};
      default: return {v,p,q};
    }
  }

  /**
   * @brief Convert HSL to HSV.
   * @tparam T Scalar type.
   * @param other HSL triplet (h in degrees, s,l in [0,1]).
   * @return HSV triplet (h in degrees, s,v in [0,1]).
   */
  template <typename T> Vec3t<T> hslToHsv(const Vec3t<T>& other) {
    const T h = other.x;
    const T s = other.y;
    const T l = other.z;

    const T v = s*std::min(l,T(1)-l)+l;
    return {h,(v > T(0)) ?  T(2)-T(2)*l/v : T(0), v};
  }

  /**
   * @brief Convert HSV to HSL.
   * @tparam T Scalar type.
   * @param other HSV triplet (h in degrees, s,v in [0,1]).
   * @return HSL triplet (h in degrees, s,l in [0,1]).
   */
  template <typename T> Vec3t<T> hsvToHsl(const Vec3t<T>& other) {
    const T h = other.x;
    const T s = other.y;
    const T v = other.z;

    T l = v-v*s/T(2);
    T m = std::min(l,T(1)-l);
    return {h,(m > T(0)) ? (v-l)/m : l, l};
  }

  /**
   * @brief Convert RGB to CMY.
   * @tparam T Scalar type.
   * @param other RGB triplet in [0,1].
   * @return CMY triplet in [0,1].
   */
  template <typename T> Vec3t<T> rgbToCmy(const Vec3t<T>& other) {
    return {T(1)-other.x,T(1)-other.y,T(1)-other.z};
  }

  /**
   * @brief Convert RGB to CMYK.
   * @tparam T Scalar type.
   * @param other RGB triplet in [0,1].
   * @return CMYK quadruplet in [0,1] where w = K (black).
   */
  template <typename T> Vec4t<T> rgbToCmyk(const Vec3t<T>& other) {
    const Vec3t<T> cmy = rgbToCmy(other);
    const T minVal = std::min(cmy.x,std::min(cmy.y,cmy.z));
    return {cmy-minVal,minVal};
  }

  /**
   * @brief Convert CMY to RGB.
   * @tparam T Scalar type.
   * @param other CMY triplet in [0,1].
   * @return RGB triplet in [0,1].
   */
  template <typename T> Vec3t<T> cmyToRgb(const Vec3t<T>& other){
    return {T(1)-other.x,T(1)-other.y,T(1)-other.z};
  }

  /**
   * @brief Convert CMYK to RGB.
   * @tparam T Scalar type.
   * @param other CMYK quadruplet in [0,1] (w = K).
   * @return RGB triplet in [0,1].
   */
  template <typename T> Vec3t<T> cmykToRgb(const Vec4& other) {
    return {T(1)-(other.x+other.w),T(1)-(other.y+other.w),T(1)-(other.z+other.w)};
  }

  /**
   * @brief Convert RGB to a simple YUV (luma/chroma) space via a linear matrix.
   * @tparam T Scalar type.
   * @param other RGB triplet in [0,1].
   * @return YUV triplet; coefficients match the matrix below (not broadcast standards).
   */
  template <typename T> Vec3t<T> rgbToYuv(const Vec3t<T>& other) {
    const Mat4t<T> c{ T(0.299),  T(0.587),  T(0.114),  T(0),
      T(-0.147), T(-0.289), T(0.436),  T(0),
      T(0.615),  T(-0.515), T(-0.100), T(0),
      T(0),      T(0),      T(0),      T(1)
    };
    return (c * Vec4t<T>(other,1)).xyz;
  }

  /**
   * @brief Convert the simple YUV (above) back to RGB.
   * @tparam T Scalar type.
   * @param other YUV triplet.
   * @return RGB triplet in [0,1].
   */
  template <typename T> Vec3t<T> yuvToRgb(const Vec3t<T>& other) {
    const Mat4t<T> c{ T(1), T(0),       T(1.140),  T(0),
      T(1), T(-0.395),  T(-0.581), T(0),
      T(1), T(2.032),   T(0),      T(0),
      T(0), T(0),       T(0),      T(1)
    };
    return (c * Vec4t<T>(other,1)).xyz;
  }
}

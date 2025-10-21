#pragma once

#include <random>
#include <algorithm>

/**
 * @file Rand.h
 * @brief Simple random number utility wrapping \c std::mt19937 with common ranges.
 *
 * Provides a small helper class, \c Random, for drawing floating-point values
 * from frequently used ranges and for shuffling containers. The generator is an
 * instance of \c std::mt19937 seeded either from \c std::random_device or an
 * explicit seed. Distributions include [0,1), [-1,1), [0,\pi), [0,0.5), and
 * [0.5,1).
 *
 * @note Thread-safety: \c Random is not inherently thread-safe. Guard access if
 *       you share a single instance across threads, or use thread-local
 *       instances.
 */
class Random {
public:
  /**
   * @brief Construct and seed from \c std::random_device.
   */
  Random();

  /**
   * @brief Construct with a deterministic seed.
   * @param seed Seed value for the underlying PRNG.
   */
  Random(uint32_t seed);

  /**
   * @brief Draw a random float in [0, 0.5).
   * @return Value \f$ x \in [0,0.5) \f$.
   */
  float rand005();

  /**
   * @brief Draw a random float in [0.5, 1).
   * @return Value \f$ x \in [0.5,1) \f$.
   */
  float rand051();

  /**
   * @brief Draw a random float in [0, 1).
   * @return Value \f$ x \in [0,1) \f$.
   */
  float rand01();

  /**
   * @brief Draw a random float in [-1, 1).
   * @return Value \f$ x \in [-1,1) \f$.
   */
  float rand11();

  /**
   * @brief Draw a random float in [0, \f$\pi\f$).
   * @return Value \f$ x \in [0,\pi) \f$ (radians).
   */
  float rand0Pi();

  /**
   * @brief Draw a value uniformly in the half-open interval [a, b).
   * @tparam T Arithmetic type (typically float/double/int). For integral types,
   *         the result is produced by scaling a [0,1) float and casting.
   * @param a Lower bound (inclusive).
   * @param b Upper bound (exclusive conceptually; for integers, distribution is
   *          effectively over the discrete set starting at a).
   * @return A value in the range [a, b).
   * @warning For integral \c T the mapping is via cast from float and thus not
   *          perfectly uniform over the integers.
   */
  template <typename T> T rand(T a, T b) {
    return a+T(rand01()*(b-a));
  }

  /**
   * @brief Shuffle a container in-place using Fisher–Yates.
   * @tparam T Element type contained in the vector.
   * @param a Container to shuffle.
   * @note Uses this instance's PRNG via \c rand<size_t>().
   */
  template <typename T> void shuffle(std::vector<T>& a) {
    for (size_t i=0;i<a.size();++i) {
      size_t r = Random::rand<size_t>(0,i+1);
      std::swap(a[i], a[r]);
    }
  }

private:
  /** @brief Non-deterministic seed source (may be deterministic on some systems). */
  std::random_device rd;
  /** @brief Mersenne Twister engine used for sampling. */
  std::mt19937 gen;
  /** @brief Uniform distribution over [0,1). */
  std::uniform_real_distribution<float> dis01;
  /** @brief Uniform distribution over [-1,1). */
  std::uniform_real_distribution<float> dis11;
  /** @brief Uniform distribution over [0,\pi). */
  std::uniform_real_distribution<float> disPi;
  /** @brief Uniform distribution over [0,0.5). */
  std::uniform_real_distribution<float> dis005;
  /** @brief Uniform distribution over [0.5,1). */
  std::uniform_real_distribution<float> dis051;

};

/**
 * @brief Global PRNG instance for convenience functions.
 *
 * Declared here and defined in a corresponding translation unit. Useful for
 * quick sampling without managing an instance. Prefer explicit dependency
 * injection for testability.
 */
extern Random staticRand;

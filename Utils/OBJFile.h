#pragma once

#include <vector>
#include <array>
#include <string>
#include <sstream>

#include "Vec3.h"

/**
 * @file OBJFile.h
 * @brief Minimal Wavefront OBJ loader collecting vertices, triangle indices, and normals.
 *
 * Parses a subset of the Wavefront OBJ format sufficient for simple triangle
 * meshes. The constructor reads the file, extracts vertex positions ("v"),
 * vertex normals ("vn"; optional), and triangular faces ("f"). Indices are
 * stored zero-based. If normals are not provided, they are computed as
 * area-weighted vertex normals from face geometry.
 *
 * @details
 *  - Faces are expected to be triangles; non-triangular faces are ignored.
 *  - Only position and normal records are recognized; texture coordinates are
 *    ignored.
 *  - When @p normalize is true, positions are recentered to the AABB center and
 *    uniformly scaled to fit into the unit cube (longest AABB side maps to 1).
 */
class OBJFile {
public:
  /**
   * @brief Load an OBJ file.
   * @param filename Path to the .obj file.
   * @param normalize If true, recenter to AABB center and scale uniformly by the
   *                  longest extent so the model fits into a unit cube.
   * @throws std::runtime_error On I/O failures or malformed input (thrown from
   *         stream operations or conversions).
   */
  OBJFile(const std::string& filename, bool normalize=false);

  /**
   * @brief Index triple forming one triangle (zero-based vertex indices).
   * @note Indices reference entries in @ref vertices.
   */
  typedef std::array<size_t, 3> IndexType;

  /** @brief Triangle index buffer (each element is one face as three indices). */
  std::vector<IndexType> indices;
  /** @brief Vertex positions parsed from "v" records (x,y,z). */
  std::vector<Vec3> vertices;
  /**
   * @brief Per-vertex normals.
   * @details If the file lacks "vn" records, these are computed by summing face
   *          normals around each vertex and normalizing.
   */
  std::vector<Vec3> normals;

private:
  /** @brief Remove leading ASCII whitespace from a string in-place. */
  void ltrim(std::string &s);
  /** @brief Remove trailing ASCII whitespace from a string in-place. */
  void rtrim(std::string &s);
  /** @brief Remove leading and trailing ASCII whitespace from a string in-place. */
  void trim(std::string &s);
  /**
   * @brief Split a string into whitespace-delimited tokens from @p startpos.
   * @param str Input string.
   * @param startpos Starting position within @p str to begin tokenization.
   * @return Vector of tokens in order.
   */
  std::vector<std::string> tokenize(const std::string& str, size_t startpos);

  /**
   * @brief Convert a decimal string to a value of type @p T.
   * @tparam T Target type (e.g., size_t, float).
   * @param str Input string (decimal representation).
   * @return Parsed value of type @p T. If parsing fails, default-initialized @p T is returned.
   */
  template <typename T> T fromStr(const std::string& str) {
    T t;
    std::istringstream iss{str};
    iss >> std::dec >> t;
    return t;
  }


};

/* quaternion.h  Basic quaternion class
 * CS 248 HW 3
 * Steven Bell <sebell@stanford.edu>
 * 17 April 2013
 */

#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include <glm.hpp>

class Quat
{
public:
  Quat(); // Identity quaternion constructor
  Quat(glm::vec3 axis, float angle); // Axis angle constructor
  Quat(float w, float x, float y, float z);

  Quat conj(); // Conjugate
  Quat operator-(); // Negation (no params)
  Quat rotate(glm::vec3 axis, float alpha); // Rotate quaternion by an axis/angle
  glm::vec3 rotateVec(glm::vec3); // Rotate a vector by this quaternion
  glm::mat4 rotationMatrix(); // Return the rotation matrix represented by this quaternion
  void print(void);

  // These operations take two quaternions
  // Because it's not immediately obvious which is the "base" and "other",
  // make these static methods that take two input quaternions.
  static float dot(const Quat& a, const Quat& b);
  static Quat cross(const Quat& a, const Quat& b);
  static Quat slerp(const Quat& a, const Quat& b, float interp); // SLERP

// No reason to make these private
  float mw, mx, my, mz;

};

#endif


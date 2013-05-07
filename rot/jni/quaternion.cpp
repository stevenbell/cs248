/* Implementation for quaternion class
 * CS 248 HW 3
 * Steven Bell <sebell@stanford.edu>
 * 17 April 2013
 */

#include "quaternion.h"

Quat::Quat(void)
{
  mw = 1.0;
  mx = my = mz = 0.0;
}

Quat::Quat(glm::vec3 axis, float angle)
{
  axis = glm::normalize(axis);
  mw = cos(angle / 2);
  mx = axis.x * sin(angle / 2);
  my = axis.y * sin(angle / 2);
  mz = axis.z * sin(angle / 2);
}

Quat::Quat(float w, float x, float y, float z)
{
  mw = w;
  mx = x;
  my = y;
  mz = z;
}

// Conjugate
Quat Quat::conj(void)
{
  return(Quat(mw, -mx, -my, -mz));
}

// Negation
Quat Quat::operator-(void)
{
  return(Quat(-mw, -mx, -my, -mz));
}

Quat Quat::rotate(glm::vec3 axis, float angle)
{
  return(cross(*this, Quat(axis, angle)));
}

float Quat::dot(const Quat& a, const Quat& b)
{
  return(a.mw*b.mw + a.mx*b.mx + a.my*b.my + a.mz*b.mz);
}

Quat Quat::cross(const Quat& a, const Quat& b)
{
  return(Quat(a.mw*b.mw - a.mx*b.mx - a.my*b.my - a.mz*b.mz,
              a.mw*b.mx + a.mx*b.mw + a.my*b.mz - a.mz*b.my,
              a.mw*b.my - a.mx*b.mz + a.my*b.mw + a.mz*b.mx,
              a.mw*b.mz + a.mx*b.my - a.my*b.mx + a.mz*b.mw));
}

Quat Quat::slerp(const Quat& a, const Quat& b, float interp)
{
  float alpha = acos(Quat::dot(a, b));
  float aInterp, bInterp;
  if(alpha > 0.01){
    aInterp = sin((1.0f- interp) * alpha) / sin(alpha);
    bInterp = sin(interp * alpha) / sin(alpha);
  }
  else{
    aInterp = 1.0f - interp;
    bInterp = interp;
  }
  return(Quat(a.mw * aInterp + b.mw * bInterp, 
              a.mx * aInterp + b.mx * bInterp,
              a.my * aInterp + b.my * bInterp,
              a.mz * aInterp + b.mz * bInterp));
}

glm::mat4 Quat::rotationMatrix()
{
  glm::mat4 m;
  m[0][0] = 1.0f - 2*my*my - 2*mz*mz;
  m[0][1] =        2*mx*my - 2*mw*mz;
  m[0][2] =        2*mx*mz + 2*mw*my;
  m[0][3] = 0.0f;

  m[1][0] =        2*mx*my + 2*mw*mz;
  m[1][1] = 1.0f - 2*mx*mx - 2*mz*mz;
  m[1][2] =        2*my*mz - 2*mw*mx;
  m[1][3] = 0.0f;

  m[2][0] =        2*mx*mz - 2*mw*my;
  m[2][1] =        2*mw*mx + 2*my*mz;
  m[2][2] = 1.0f - 2*mx*mx - 2*my*my;
  m[2][3] = 0.0f;

  m[3][0] = m[3][1] = m[3][2] = 0.0f;
  m[3][3] = 1.0f;

/*
  printf("rotation:\n");
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      printf("%f ", m[i][j]);
    }
    printf("\n");
  }
*/
  return m;
}

void Quat::print()
{
  printf("w: %0.3f, x: %0.3f, y: %0.3f z: %0.3f\n", mw, mx, my, mz);
}

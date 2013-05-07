/* spline.h  Class representing Catmull-Rom splines
 * CS 248 HW 3
 * Steven Bell <sebell@stanford.edu>
 * 17 April 2013
 */

#ifndef __SPLINE_H__
#define __SPLINE_H__

#include <vector>

class Spline
{
public:
  void addKeypoint(float time, float value);
  float getValue(float time);
  void print(void);

private:
  std::vector<float> mKeyTimes;
  std::vector<float> mKeyValues;
};

#endif


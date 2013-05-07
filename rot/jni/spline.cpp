/* spline.cpp  Demonstration of Catmull-Rom splines
 * CS 248 HW 3
 * Steven Bell <sebell@stanford.edu>
 * 24 April 2013
 */

#include <cstdio>
#include <cmath>
#include "spline.h"

void Spline::addKeypoint(float time, float value)
{
  // Do a dumb linear search to insert the new point
  // These will generally be small, so it's ok.
  std::vector<float>::iterator it;

  // If the list is empty or if all current points are earlier
  // This depends on short-circuiting OR
  if(mKeyTimes.size() == 0 || time > mKeyTimes.back()){
    mKeyTimes.push_back(time);
    mKeyValues.push_back(value);
  }
  else{
    for(it = mKeyTimes.begin(); it < mKeyTimes.end(); it++){
      if(time < *it){ // Found the first key that is after
        printf("Inserted new key");
        mKeyTimes.insert(it, time); // Insert before it
        // TODO BUG: insert corresponding data
        break; // And quit
      }
    }
  }
}

float Spline::getValue(float time)
{
  // If this is before the first, then return the start value
  if(time <= mKeyTimes.front()){
    return(mKeyValues.front());
  }
  // If this is after the end, then return the end value
  else if(time >= mKeyTimes.back()){
    return(mKeyValues.back());
  }
  else{
    // TODO: properly handle endpoints
    // Use indexing rather than an iterator for simplicity accessing the parallel vector
    // Start at 1, since we know we're greater than the first
    for(int i = 1; i < mKeyTimes.size(); i++){
      if(time < mKeyTimes[i]){
        float c3 = -0.5f * mKeyValues[i-2] + 1.5f * mKeyValues[i-1] +
                   -1.5f * mKeyValues[i] + 0.5f * mKeyValues[i+1]; 
        float c2 = 1.0f * mKeyValues[i-2] - 2.5f * mKeyValues[i-1] +
                   2.0f * mKeyValues[i] - 0.5f * mKeyValues[i+1]; 
        float c1 = -0.5f * mKeyValues[i-2] + 0.5f * mKeyValues[i]; 
        float c0 = mKeyValues[i-1]; 

        float interp = (time - mKeyTimes[i-1]) / (mKeyTimes[i] - mKeyTimes[i-1]);

  //printf("keys:  %f %f %f %f\n", mKeyValues[i-2], mKeyValues[i-1], mKeyValues[i], mKeyValues[i+1]);
  //printf("%f:%f  %f %f %f %f\n", time, interp, c3, c2, c1, c0);
        // We have our value, just break out
        return(pow(interp, 3) * c3 + pow(interp, 2) * c2 + interp * c1 + c0);

        // Linear interpolation for comparison
        // return(mKeyValues[i-1] * (1 - interp) + mKeyValues[i] * interp);
      }
    }
  }
  printf("Failed to interpolate time %f\n", time);
  return(0); // If we get here, something went wrong
}

void Spline::print(void)
{
  for(int i = 0; i < mKeyTimes.size(); i++){
    printf("t: %f  v: %f\n", mKeyTimes[i], mKeyValues[i]);
  }
}


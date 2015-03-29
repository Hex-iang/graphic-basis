/**********************************************************************
 * Random header
 * poisson distribution generator
 **********************************************************************/
#include "vector.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>

class Rand
{
public:
  float min_val;
  float max_val;

  Rand(const float & _min_val, const float & _max_val): min_val(_min_val), max_val(_max_val){}
  Vector generate()
  {
    return Vector( random(), random(), random()).normalize();
  }
private:
  float random()
  {
    float r = ((float) rand()) / (float) RAND_MAX;
    return min_val + r * (max_val - min_val);
  }
};

class Poisson
{
public:
  float min_dist;
  float min_val;
  float max_val;
  vector<Vec3<float> > history; 

  Poisson(const float & _dist, const float & _min_val, const float & _max_val): 
  min_dist(_dist), min_val(_min_val), max_val(_max_val) {}

  void reset(){ history.clear(); }

  Vec3<float> generate()
  {
    bool valid;
    Vec3<float> p;

    do{
      valid = true;
      p = Vec3<float>(random() , random(), 0).normalize();
      for (unsigned int i = 0; i < history.size(); ++i)
      {
        if ( (history[i] - p).length() < min_dist )
          valid = false; 
      }
    }while(!valid);
    
    history.push_back(p);
    
    return p;
  }
  float num() const { return (float) history.size(); }
private:
  float random()
  {
    float r = ((float) rand()) / (float) RAND_MAX;
    return min_val + r * (max_val - min_val);
  }
};

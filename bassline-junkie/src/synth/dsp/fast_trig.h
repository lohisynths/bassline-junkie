/*
 * fast_trig.h
 *
 *  Created on: Apr 7, 2026
 *      Author: alax
 */

#ifndef BASSLINE_JUNKIE_SRC_SYNTH_FAST_TRIG_H_
#define BASSLINE_JUNKIE_SRC_SYNTH_FAST_TRIG_H_


#include <cmath>

namespace bassline
{
namespace math
{


// https://www.gamedev.net/forums/topic/621589-extremely-fast-sin-approximation/
union el
{
    double z1;
    int z2;
};


inline double fast_sin(double x) {
    int k;
    double y;
    double z;

    el zz;

    z  = x;
    z *= 0.3183098861837907;
    z += 6755399441055744.0;

    zz.z1 = z;

    //k  = *((int *) &z);
    k = zz.z2;

    z  = k;
    z *= 3.1415926535897932;
    x -= z;
    y  = x;
    y *= x;
    z  = 0.0073524681968701;
    z *= y;
    z -= 0.1652891139701474;
    z *= y;
    z += 0.9996919862959676;
    x *= z;
    k &= 1;
    k += k;
    z  = k;
    z *= x;
    x -= z;
    return x;
}

inline double fastPow(double a, double b) {
  union {
      double d;
    int x[2];
  } u = { a };
  u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
  u.x[0] = 0;
  return u.d;
}


//https://stackoverflow.com/a/1640399
static unsigned long x=123456789, y=362436069, z=521288629;

inline unsigned long fastRand(void) {          //period 2^96-1
unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

   t = x;
   x = y;
   y = z;
   z = t ^ x ^ y;

  return z;
}

//http://nghiaho.com/?p=997
inline double fastAtan(double x)
{
    return M_PI_4*x - x*(fabs(x) - 1)*(0.2447 + 0.0663*fabs(x));
}

inline double atan(double x)
{
    return fastAtan(x);
}

inline double sin(double x)
{
    return fast_sin(x);
    //return std::sin((long double)x);
}
inline double cos(double x)
{
    return std::cos(x);
}

inline double fabs(double x)
{
    return std::fabs(x);
}

inline double floor(double x)
{
    return std::floor(x);
}

inline double abs(double x)
{
    return std::floor(x);
}

inline double sqrt(double x)
{
    return std::sqrt(x);
}

inline double exp(double x)
{
    return std::exp(x);
}

inline double ceil(double x)
{
    return std::ceil(x);
}

inline double log(double x)
{
    return std::log(x);
}

inline double pow(double x, double y)
{
    return fastPow(x, y);
}

inline double atan2(double x, double y)
{
    return std::atan2(x, y);
}

inline double fmod(double x, double y)
{
    return std::fmod(x,y);
}
#define RAND_MAX_2  2147483647

inline unsigned long rand()
{
    unsigned long output = fastRand() % RAND_MAX_2;
    return output;
}

}
}


#endif /* BASSLINE_JUNKIE_SRC_SYNTH_FAST_TRIG_H_ */

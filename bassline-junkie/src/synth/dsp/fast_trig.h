/*
 * fast_trig.h
 *
 *  Created on: Apr 7, 2026
 *      Author: alax
 */

#ifndef BASSLINE_JUNKIE_SRC_SYNTH_FAST_TRIG_H_
#define BASSLINE_JUNKIE_SRC_SYNTH_FAST_TRIG_H_


#include <cmath>
#include <cstdint>
#include <cstring>

namespace bassline
{
namespace math
{


inline double fast_sin(double x) {
    constexpr double kInvPi = 0.31830988618379067154;
    constexpr double kPi = 3.14159265358979323846;
    constexpr double kMaxReducedInput = 1048576.0;

    if (!std::isfinite(x) || std::fabs(x) > kMaxReducedInput) {
        return std::sin(x);
    }

    const auto turns = static_cast<std::int64_t>(std::nearbyint(x * kInvPi));
    const double reduced = x - static_cast<double>(turns) * kPi;
    const double reduced2 = reduced * reduced;

    double y = reduced * (1.0 + reduced2 * (
        -1.66666666666666324348e-1 + reduced2 * (
         8.33333333332248946124e-3 + reduced2 * (
        -1.98412698298579493134e-4 + reduced2 * (
         2.75573137070700676789e-6 + reduced2 *
        -2.50507602534068634195e-8)))));

    if (turns & 1) {
        y = -y;
    }

    return y;
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
    return std::atan(x);
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
    return std::pow(x, y);
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

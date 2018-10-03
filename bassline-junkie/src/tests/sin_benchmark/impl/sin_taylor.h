/*
 * sin_taylor.h
 *
 *  Created on: Oct 3, 2018
 *      Author: alax
 */

#ifndef SRC_TESTS_SIN_BENCHMARK_IMPL_SIN_TAYLOR_H_
#define SRC_TESTS_SIN_BENCHMARK_IMPL_SIN_TAYLOR_H_

// http://lolengine.net/blog/2011/12/21/better-function-approximations
template<typename T>
T sin_taylor(T x)
{
    static const T a0 =  1.0;
    static const T a1 = -1.666666666666666666666666666666e-1;  /* -1/3! */
    static const T a2 =  8.333333333333333333333333333333e-3;  /*  1/5! */
    static const T a3 = -1.984126984126984126984126984126e-4;  /* -1/7! */
    static const T a4 =  2.755731922398589065255731922398e-6;  /*  1/9! */
    static const T a5 = -2.505210838544171877505210838544e-8;  /* -1/11! */
    static const T a6 =  1.605904383682161459939237717015e-10; /*  1/13! */
    T x2 = x * x;
    return x * (a0 + x2 * (a1 + x2 * (a2 + x2
             * (a3 + x2 * (a4 + x2 * (a5 + x2 * a6))))));
}

#endif /* SRC_TESTS_SIN_BENCHMARK_IMPL_SIN_TAYLOR_H_ */

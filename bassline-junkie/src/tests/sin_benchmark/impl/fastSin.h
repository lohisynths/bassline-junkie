/*
 * fastSin.h
 *
 *  Created on: Oct 3, 2018
 *      Author: alax
 */

#ifndef SRC_TESTS_SIN_BENCHMARK_IMPL_FASTSIN_H_
#define SRC_TESTS_SIN_BENCHMARK_IMPL_FASTSIN_H_

// https://stackoverflow.com/a/6091846
template<typename T>
T fastSin(T x ){
    const T PI = 3.14159265358979323846;
    x = std::fmod(x + PI, PI * (T)(2)) - PI; // restrict x so that -M_PI < x < M_PI
    const T B = (T)(4.0)/PI;
    const T C = (T)(-4.0)/(PI*PI);
    const T P = 0.225;
    T y = B * x + C * x * std::abs(x);
    return P * (y * std::abs(y) - y) + y;
}


#endif /* SRC_TESTS_SIN_BENCHMARK_IMPL_FASTSIN_H_ */

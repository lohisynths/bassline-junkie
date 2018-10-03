/*
 * fast_sin.h
 *
 *  Created on: Oct 3, 2018
 *      Author: alax
 */

#ifndef SRC_TESTS_SIN_BENCHMARK_IMPL_FAST_SIN_H_
#define SRC_TESTS_SIN_BENCHMARK_IMPL_FAST_SIN_H_


// https://www.gamedev.net/forums/topic/621589-extremely-fast-sin-approximation/
double fast_sin(double x) {
    int32_t k;
    double y;
    double z;
    z  = x;
    z *= 0.3183098861837907;
    z += 6755399441055744.0;
    k  = *((int32_t *) &z);
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


#endif /* SRC_TESTS_SIN_BENCHMARK_IMPL_FAST_SIN_H_ */

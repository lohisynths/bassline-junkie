/*
 * fast_sin_w_round.h
 *
 *  Created on: Oct 3, 2018
 *      Author: alax
 */

#ifndef SRC_TESTS_SIN_BENCHMARK_IMPL_FAST_SIN_W_ROUND_H_
#define SRC_TESTS_SIN_BENCHMARK_IMPL_FAST_SIN_W_ROUND_H_

// https://www.gamedev.net/forums/topic/621589-extremely-fast-sin-approximation/
// on raspi have to define NO_FAST_TRUNCATE
#define NO_FAST_TRUNCATE
template<typename T, typename Tt>
Tt fast_round(T x) {
#ifndef NO_FAST_TRUNCATE
    const T MAGIC_ROUND = 6755399441055744.0; /* http://stereopsis.com/sree/fpu2006.html */
    union {
        double d;
        struct {
#ifdef BIG_ENDIAN
            Tt hw;
            Tt lw;
#else // BIG_ENDIAN
            Tt lw;
            Tt hw;
#endif
        };
    } fast_trunc;
    fast_trunc.d = x;
    fast_trunc.d += MAGIC_ROUND;
    return fast_trunc.lw;
#else //
    if (x < 0) {
        return (Tt) (x - 0.5);
    } else {
        return (Tt) (x + 0.5);
    }
#endif // NO_FAST_TRUNCATE
}

template<typename T, typename Tt>
T fast_sin_w_round(T x) {
    const T PI = 3.14159265358979323846264338327950288;
    const T INVPI = 0.31830988618379067153776752674502872;
    const T A = 0.00735246819687011731341356165096815;
    const T B = -0.16528911397014738207016302002888890;
    const T C = 0.99969198629596757779830113868360584;
    Tt k;
    T x2;
    /* find offset of x from the range -pi/2 to pi/2 */
    k = fast_round<T, Tt>(INVPI * x);
    /* bring x into range */
    x -= k * PI;
    /* calculate sine */
    x2 = x * x;
    x = x * (C + x2 * (B + A * x2));
    /* if x is in an odd pi count we must flip */
    if (k % 2)
        x = -x;
    return x;
}



#endif /* SRC_TESTS_SIN_BENCHMARK_IMPL_FAST_SIN_W_ROUND_H_ */

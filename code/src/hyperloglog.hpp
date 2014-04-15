#ifndef HYPERLOGLOG_H
#define HTPERLOGLOG_H

/**
 * @file hyperloglog.hpp
 * @brief HyperLogLog cardinality estimator
 * @date Created 2013/3/20
 * @author Hideaki Ohno
 */

#include<vector>
#include<cmath>
#include"murmur3.h"

#define HLL_HASH_SEED 313

static const double pow_2_32 =  4294967296.0;
static const double neg_pow_2_32 = -4294967296.0;

/** @class HyperLogLog
 *  @brief Implement of 'HyperLogLog' estimate cardinality algorithm
 */
class HyperLogLog {

  private:
  public:
    uint8_t  b;     /// register bit width
    uint32_t m;     /// register size
    std::vector<uint8_t> M;
    double alphaMM; /// alpha * m^2

    uint8_t rho(uint32_t x, uint8_t b) {
        uint8_t v = 1;
        while (v <= b && !(x & 0x80000000)) {
            v++;
            x <<= 1;
        }
        return v;
    }

    /**
     * Constructor
     *
     * @param[in] b_ bit width (register size will be pow(2,b_))
     */
    HyperLogLog(uint8_t b_) : b(b_), m(1<<b), M(m+1,0) {
       double alpha;
       switch(m){
           case 16:
               alpha = 0.673;
               break;
           case 32:
               alpha = 0.697;
               break;
           case 64:
               alpha = 0.709;
               break;
           default:
               alpha = 0.7213/(1.0 + 1.079/m);
       }
       alphaMM = alpha * m * m;
    }

    /**
     * Add element to the estimator
     *
     * @param[in] str string to add
     * @param[in] len length of string
     */
    void add(const char* str, uint32_t len){
        uint32_t hash;
        MurmurHash3_x86_32(str,len,HLL_HASH_SEED, (void*)&hash);
        uint32_t index = hash >> ( 32 - b );
        uint8_t rank = rho((hash << b), 32 - b);
        if( rank > M[index] ){
            M[index] = rank;
        }
    }

    /**
     * Estimate cardinality value.
     *
     * @return Estimated cardinality value.
     */
    double estimate(){
        double estimate;
        double sum = 0.0;
        for (uint32_t i = 0; i < m; i++) {
            sum += 1.0/pow(2.0, M[i]);
        }
        estimate = alphaMM/sum; // E in the original paper
        if( estimate <= 2.5 * m ) {
            uint32_t zeros = 0;
            for (uint32_t i = 0; i < m; i++) {
                if (M[i] == 0) {
                    zeros++;
                }
            }
            if( zeros != 0 ) {
                estimate = m * log((double)m/zeros);
            }
        } else if (estimate > (1.0/30.0) * pow_2_32) {
            estimate = neg_pow_2_32 * log(1.0 - ( estimate/pow_2_32 ) );
        }
        return estimate;
    }

    void merge(HyperLogLog& h2) {
      // merge h2 into myself
      for (uint32_t i = 0; i < m; i++) {
        if (h2.M[i] > M[i])
          M[i] = h2.M[i];
      }
    }

};

#endif


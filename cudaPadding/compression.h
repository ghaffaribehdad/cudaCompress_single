#pragma once

#include <cuda_runtime.h>
#include <vector>

typedef unsigned int uint;


// h_data : host pointer to the data
// size: dimension of data in 4D
void compress(int4 size, float * h_data, std::vector<uint> & bitStream, const float & Quant_step);

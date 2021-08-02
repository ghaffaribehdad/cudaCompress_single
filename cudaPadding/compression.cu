#include "compression.h"

#include <vector>

#include <cuda_runtime.h>
#include <cuda_profiler_api.h>

#include <cudaCompress/Instance.h>
#include <cudaCompress/Encode.h>
#include <cudaCompress/util/Bits.h>
#include <cudaCompress/util/DWT.h>
#include <cudaCompress/util/Quantize.h>
#include <cudaCompress/util/YCoCg.h>
#include <cudaCompress/Timing.h>
using namespace cudaCompress;


#include "../cudaCompress/src/examples/tthread/tinythread.h"

#include "../cudaCompress/src/examples/tools/entropy.h"
#include "../cudaCompress/src/examples/tools/imgtools.h"
#include "../cudaCompress/src/examples/tools/rawfile.h"

#include "../cudaCompress/src/examples/cudaUtil.h"

#include "../cudaCompress/src/examples/CompressImage.h"
#include "../cudaCompress/src/examples/CompressHeightfield.h"
#include "../cudaCompress/src/examples/CompressVolume.h"

void compress(int4 size, float * h_data, std::vector<uint> & bitStreams, const float & Quant_step)
{
	// a device pointer to the data sitted on GPU
	float* d_data = nullptr;

	const bool doRLEOnlyOnLvl0 = true;

	const unsigned int elemCountTotal = size.x*size.y*size.z*size.w;
	

	// Allocate GPU Memory
	cudaMalloc(&d_data, elemCountTotal * sizeof(float));
	
	// Copy the uncompressed data into memory
	cudaMemcpy(d_data, h_data, elemCountTotal * sizeof(float), cudaMemcpyHostToDevice);


	uint huffmanBits = 0;


	GPUResources::Config config = CompressVolumeResources::getRequiredResources(size.x, size.y, size.z, 1, huffmanBits);
	GPUResources shared;

	shared.create(config);
	CompressVolumeResources res;
	res.create(shared.getConfig());

	// To keep the compressed 
	

	
	compressVolumeFloat(shared, res, d_data, size.x, size.y, size.z, 2, bitStreams, Quant_step, doRLEOnlyOnLvl0);
	


	//// Set the device memory to 0
	//cudaSafeCall(cudaMemset(d_data, 0, elemCountTotal * sizeof(float)));

	//// Pinning the Host memory
	//cudaSafeCall(cudaHostRegister(bitStreams.data(), bitStreams.size() * sizeof(uint), cudaHostRegisterDefault));
	//




	//decompressVolumeFloat(shared, res, d_data, size.x, size.y, size.z, 2, bitStreams, 0.01f, doRLEOnlyOnLvl0);



	cudaFree(d_data);

	res.destroy();
	shared.destroy();
	 
}



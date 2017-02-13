/*******************************************************
 * Copyright (c) 2014, ArrayFire
 * All rights reserved.
 *
 * The original file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

#include <arrayfire.h>

#include <cstdio>
#include <cstdlib>
#include <time.h>

typedef unsigned int uint;

// stencil for derivation
static const float h_derive[] = {
	-1.0,	0.0,	1.0
};


int main(int argc, char *argv[]){
	const double lambda = 0.015;

	const double tau = .1;
	const uint iterations = 500;

	const char* path = "photo.jpg";
    try{
        // Select a device and display arrayfire info
        const int device = 0;
        af::setDevice(device);
        
		//display device info
		af::info();

		//load iamge and convert to native scale
		af::array img = af::loadImage(path) / 255.;
		const af::dim4 dims = img.dims();
		const dim_t width = dims[0];
		const dim_t height = dims[1];

		af::array res = af::constant(0, width, height);

		const af::array k_dx(3, 1, h_derive);
		const af::array k_dy(1, 3, h_derive);

		af::array g = af::constant(0.0, width, height);
		af::array gdx = af::constant(0.0, width, height);
		af::array gdy = af::constant(0.0, width, height);

		af::array gtdx = af::constant(0.0, width, height);
		af::array gtdy = af::constant(0.0, width, height);

		af::array dx = af::constant(0.0, width, height);
		af::array dy = af::constant(0.0, width, height);
		af::array grad = af::constant(0.0, width, height);

		printf("starting iterations now\n");
		const double lambdaSq = lambda * lambda;
		af::Window window(height, width, path);

		unsigned int time = clock();
		for(uint i = 0; i < iterations; ++i){
			dx = af::convolve(img, k_dx);
			dy = af::convolve(img, k_dy);

			grad = dx*dx + dy*dy;
			g = 1.0f / (1.0f + (grad / lambdaSq));

			gdx = af::convolve(g * dx, k_dx);
			gdy = af::convolve(g * dy, k_dy);

			res = img + tau * (gdx + gdy);
			img = res;

			window.image(img, path);
			window.show();
		}
		time = clock() - time;
		printf("Executed in %f seconds\n", ((float)time / (float)CLOCKS_PER_SEC));

		while(!window.close()){
			window.image(img, path);
			window.show();
		}

    }catch(af::exception& e){
        fprintf(stderr, "%s\n", e.what());
        throw;
    }

    return 0;
}
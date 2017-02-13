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

#define N 128
#define DEGREE 3


// generate noisy parabola
static af::array randomPoints(const int count, const int addNoise = 5, const int degree = 3){
	const float range = count / 2;

	af::array line = af::seq(-range + 0.5, range - 0.5);
	af::array noise = af::randu(count, 1, f32) * addNoise - (addNoise / 2);
	af::array out = (line * line) + noise;

	return out;
}	

// create vandermonde matrix of matching degree
static af::array setupVandermonde(const af::array& in, const int degree){
	const dim_t dim = in.dims(0);
	af::array out(dim, degree);

	for (int y = 0; y < degree; ++y)
		out.col(y) = af::pow(in, (float)y);

	return out;
}

// get coefficients and create sampled points
static af::array reconstructPoly(const af::array& x, const af::array& coeffs){
	const dim_t dim = x.dims(0);
	const dim_t degree = coeffs.dims(0);

	af::array out(dim, 1);
	af::array val(1,1);

	af::array terms(degree, dim);
	for (int i = 0; i < degree; ++i){
		terms(i, af::span) = af::pow(x, i);
	}
	af::array exps = tile(coeffs, 1, dim);
	return af::flat(af::sum(terms*exps));
}


int main(int argc, char *argv[]){
    try{
        // Select a device and display arrayfire info
        int device = 0;
        af::setDevice(device);      
		
		unsigned int time = clock();

		// initialization
		af::array x = af::seq(0, N - 1, 1);
		af::array y = randomPoints(N, 10*N);
		af::array X = setupVandermonde(x, DEGREE);

		af::array coefficients = af::solve(X, y);
		af::array out = reconstructPoly(x, coefficients);

		time = clock() - time;
		printf("Executed in %f seconds\n", ((float)time / (float)CLOCKS_PER_SEC));

		af::Window windowIn(512, 512, "Input Data");
		windowIn.plot(x, y);

		af::Window windowSq(512, 512, "Least Squares");
		windowSq.plot(x,out);
		windowSq.show();

		printf("Regression done\n");
		while(!windowIn.close() && !windowSq.close())
			windowIn.show();

    }catch (af::exception& e){
        fprintf(stderr, "%s\n", e.what());
        throw;
    }

    return 0;
}
// (C) 2021 by folkert van heusden <mail@vanheusden.com>
// released under Apache License v2.0
#include <math.h>
#include <fftw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "fft.h"

fft::fft(int n_samples_in_in, double *data)
{
	n_samples_in = n_samples_in_in;
	pin  = data;
	pout = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * n_samples_in + 1);

	/* init fftw */
	plan = fftw_plan_dft_r2c_1d(n_samples_in, pin, pout, FFTW_ESTIMATE);
	if (!plan)
		fprintf(stderr, "failed calculating plan for fft");
}

fft::~fft()
{
	fftw_free(pout);
}

void fft::do_fft(double *o)
{
	/* calc fft */
	fftw_execute(plan);

	for(int loop=0; loop<(n_samples_in / 2) + 1; loop++)
	{
		double real = pout[loop][0];
		double img = pout[loop][1];
		double mag = hypot(real, img); //sqrt(pow(real, 2.0) + pow(img, 2.0));

		o[loop] = mag;
	}
}

void do_fft(const double *const in, const size_t n, double **out, size_t *n_out)
{
	double *temp = new double[n];
	memcpy(temp, in, n * sizeof(double));

	fft f(n, temp);

	size_t hn = n / 2 + 1;

	*out = (double *)malloc(hn * sizeof(double));
	*n_out = hn;

	f.do_fft(*out);

	delete [] temp;
}

std::pair<double, double> find_loudest_freq(const double *in, const size_t n, const int sample_rate)
{
	double *fd = NULL;
	size_t nfd = 0;
	do_fft(in, n, &fd, &nfd);

	double maxval = -1;
	size_t bin = 0;
	for(size_t i=0; i<nfd; i++) {
		if (fd[i] > maxval) {
			maxval = fd[i];
			bin = i;
		}
	}

	free(fd);

	double bin_size = double(n) / sample_rate;
	double freq_of_bin = bin / bin_size;

	return { freq_of_bin, maxval };
}

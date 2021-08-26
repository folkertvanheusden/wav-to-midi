// (C) 2021 by folkert van heusden <mail@vanheusden.com>
// released under Apache License v2.0
#include <fftw3.h>
#include <utility>

class fft
{
private:
	double *pin;
	fftw_complex *pout;
	fftw_plan plan;
	int n_samples_in;

public:
	fft(int n_samples_in, double *data_in);
	~fft();

	void do_fft(double *o);
};

std::pair<double, double> find_loudest_freq(const double *in, const size_t n, const int sample_rate);

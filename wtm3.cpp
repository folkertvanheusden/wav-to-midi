// (C) 2021 by folkert van heusden <mail@vanheusden.com>
// released under Apache License v2.0
#include <getopt.h>
#include <math.h>
#include <sndfile.h>
#include <stdio.h>
#include <MidiFile.h>

#include "fft.h"

void usage() {
	printf("-i x  sound file to process\n");
	printf("-o x  midi-file to write to\n");
	printf("-a    use velocity in output (you probably want this)\n");
	printf("-p    use pitchbend in output\n");
}

int main(int argc, char *argv[])
{
	const char *in = nullptr, *out = nullptr;
	bool vel = false, pb = false;
	int c = 0;

	while((c = getopt(argc, argv, "i:o:aph")) != -1) {
		if (c == 'i')
			in = optarg;
		else if (c == 'o')
			out = optarg;
		else if (c == 'a')
			vel = true;
		else if (c == 'p')
			pb = true;
		else if (c == 'h') {
			usage();
			return 0;
		}
		else {
			usage();
			return 1;
		}
	}

	if (!in) {
		fprintf(stderr, "no input-file selected\n\n");
		usage();
		return 1;
	}

	if (!out) {
		fprintf(stderr, "no output-file selected\n\n");
		usage();
		return 1;
	}

	// read whole audio file in memory (should be mono)
	SF_INFO sfinfo { 0 };
	SNDFILE *fh = sf_open(in, SFM_READ, &sfinfo);
	if (!fh) {
		perror("sf_open");
		return 1;
	}

	if (sfinfo.channels != 1) {
		fprintf(stderr, "should be mono sample. currently %u channels.\n", sfinfo.channels);
		sf_close(fh);
		return 1;
	}

	int n_samples = sf_seek(fh, 0, SEEK_END);
	sf_seek(fh, 0, SEEK_SET);

	double *samples = new double[n_samples]();
	if (sf_read_double(fh, samples, n_samples) != n_samples) {
		fprintf(stderr, "SIP: short read\n");
		sf_close(fh);
		exit(1);
	}

	sf_close(fh);

	// target sample rate is 520Hz: 31250 bits per second, 2 messages of 3 bytes (on + off), 10 bits per byte
	double max_hz = 31250 / double(3 * 10 * 2);
	double ratio = sfinfo.samplerate / max_hz;

	// to midi
	uint8_t pnote = 255;

	int t = 0;

	smf::MidiFile outputfile;
        outputfile.absoluteTicks();

	outputfile.setTicksPerQuarterNote(max_hz / 4);

	outputfile.addTimbre(0, 0, 0, 55 - 1);

	if (pb)
		outputfile.setPitchBendRange(0, 0, 0, 1.0);

	std::vector<std::pair<double, double> > fm;

	double loudest = -1.0;

	double offset = 0;
	while(offset < n_samples) {
		auto e = find_loudest_freq(&samples[int(offset)], ratio, sfinfo.samplerate);
		fm.push_back(e);

		if (e.second > loudest)
			loudest = e.second;

		offset += ratio;
	}

	for(auto e : fm) {
		double f = e.first, m = e.second;

        	double dnote = 69 + 12.0 * log(f / 440.0);
		uint8_t note = dnote;

		if (pnote != note || vel || pb) {
			if (pnote != 255 && pnote != note)
				outputfile.addNoteOff(0, t, 0, pnote);

			double velocity = vel ? m * 127.0 / loudest : 127;
			if (velocity > 127)
				velocity = 127;

			if (pnote != note)
				outputfile.addNoteOn(0, t, 0, note, velocity);

			if (pb) {
				double bend = dnote - note;

				if (!isinf(bend))
					outputfile.addPitchBend(0, t, 0, bend);
			}

			pnote = note;
		}

		t++;
	}

        outputfile.sortTracks();
        outputfile.write(out);

	return 0;
}

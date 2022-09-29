wav-to-midi_a3
--------------
To clone it:

```sh
git clone https://github.com/folkertvanheusden/wav-to-midi.git
cd wav-to-midi
git submodule update --init --recursive
```

To build it:

This program requires the libsndfile1-dev libfftw3-dev packages.

```sh
mkdir build && cd build
cmake ../
make
```

To run it:

```sh
./wtm3 -i test.wav -o test.mid -a
```

-a is optional but it gives the best results.


copyright
---------
This program was written by Folkert van Heusden <mail@vanheusden.com> and released under the apache license v2.0.

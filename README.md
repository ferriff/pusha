pusha
=====

Pulse Shape Analysis

Standalone version of the ECAL local reconstruction code for signal amplitude and time.

Requires `ROOT`, and to run the `FixedAlphaBeta` algo it also needs `CLHEP`.

The default version is running the `RecWeights` algo and can be compiled with:

   ``g++ pusha.cc `root-config --libs --cflags` ``

The switch between the two as well as other instructions on how to compile on
`lxplus` using `CLHEP` will come later.

Enjoy.

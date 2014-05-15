pusha
=====

Pulse Shape Analysis

Standalone version of the ECAL local reconstruction code for signal amplitude and time.

Requires `ROOT`, and to run the `FixedAlphaBeta` algo it also needs `CLHEP`.

The default version is running the `RecWeights` algo and can be compiled with:

   ``g++ pusha.cc `root-config --libs --cflags` ``

The switch between the two as well as other instructions on how to compile on
`lxplus` using `CLHEP` will come later.

To run `bias.cc` for the ideal amplitude and time bias studies, do the following:
   * compile the needed library with
     `g++ -shared -fPIC -o libEcalShapes.so EBShape.cc EEShape.cc EcalShapeBase.cc`
   * compile and link this code with
     `g++ bias.cc -L. -lEcalShapes`
   * set the correct `LD_LIBRARY_PATH` with
     `source ./setup.sh`
   * run with
     `./a.out > biases.dat`

Enjoy. :)

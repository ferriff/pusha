#include <stdio.h>

// compile the needed library with
//    g++ -shared -fPIC -o libEcalShapes.so EBShape.cc EEShape.cc EcalShapeBase.cc
//
// compile and link this code with
//    g++ bias.cc -L. -lEcalShapes
//
// run with
//    ./a.out > biases.dat
// after having done a
//    source ./setup.sh

#include "EBShape.h"
#include "EEShape.h"

void biases(EcalShapeBase & shape, float * wA, float * wT)
{
        //EcalShapeBase & shape = ees;
        //float * wA = wA_ee;
        //float * wT = wT_ee;

        //fprintf(stderr, "#EBShape: timeOfMax = %f; timeOfThr = %f\n", shape.timeOfMax(), shape.timeOfThr());
        const double rT = shape.timeToRise() ;
        const double tm = shape.timeOfMax();   // w.r.t. timeOfThr()
        const double tT = shape.timeOfThr();
        //fprintf(stderr, "--> %f\n", shape(tm - tT)); // the position of the maximum
        const double t0 = tT - tm + 5 * 25; // the sample is the sample, no half bin is to be considered here

        float refA = shape(5 * 25. - t0);
        float refT = 0;

        for (int iphase = 0; iphase < 1000; ++iphase) {
                float vA = 0., vT = 0.;
                float phase = -500. + iphase * .7;
                //float phase = -62.;
                for (int iw = 0; iw < 10; ++iw) {
                        vA += wA[iw] * shape(iw * 25. - phase - t0);
                        vT += wT[iw] * shape(iw * 25. - phase - t0);
                        //fprintf(stderr, "%f %f\n", (float)iw, shape(iw * 25. - phase - t0));
                }
                fprintf(stdout, "%f %f %f\n", phase, vA / refA, - vT / vA * 25.);
        }
}


int main()
{
        EBShape ebs;
        float wA_eb[] = {-0.382846, -0.382846, -0.382846, 0., 0.18463, 0.424762, 0.349301, 0.176883, 0.0129613, 0.};
        float wT_eb[] = {0.0406631, 0.0406631, 0.0406631, 0., 1.32518, -0.0499708, -0.504338, -0.502484, -0.390372, 0.};

        EEShape ees;
        float wA_ee[] = {-0.382067, -0.382067, -0.382067, 0., 0.204788, 0.415302, 0.339506, 0.172017, 0.0145885, 0.};
        float wT_ee[] = {0.0579372, 0.0579372, 0.0579372, 0., 2.05482, 0.0127631, -0.787037, -0.830325, -0.624029, 0.};

        biases(ebs, wA_eb, wT_eb);
        fprintf(stdout, "\n\n");
        biases(ees, wA_ee, wT_ee);

        return 0;
}

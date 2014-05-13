// compile with
//   g++ pusha.cc `root-config --libs --cflags` `clhep-config --libs`
// vim makeprg:
//   makeprg=g++\ pusha.cc\ `root-config\ --libs\ --cflags`\ `clhep-config\ --libs`

#include <stdio.h>

#define STANDALONE 1

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"

#ifndef STANDALONE
#include "RecoLocalCalo/EcalRecAlgos/interface/EcalUncalibRecHitRecAbsAlgo.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/isFinite.h"

#include <vector>
#include <string>

#else

#include "cmssw.h"

// to reconstruct with the FixedAlphaBeta fit algorithm
// also change the definition of `algo' in the `main'
// function below
//#include "EcalUncalibRecHitFixedAlphaBetaAlgo.h"
#include "EcalUncalibRecHitRecWeightsAlgo.h"

#endif


int main()
{
        //typedef std::vector<float> EBDataFrame;
        double pedVec[3] = {200., 200., 200.};
        double gainRatios[3] = {1., 2., 6.};

        // amplitude and time reconstruction method
        //EcalUncalibRecHitFixedAlphaBetaAlgo<EBDataFrame> algo;
        EcalUncalibRecHitRecWeightsAlgo<EBDataFrame> algo;

        EcalDigiCollection digi;
        EBDetId id; // in case, one EBDetId is 838904341
        digi.push_back(id.rawId());
        EcalDataFrame samples(digi.back());

        // pulse shape definition
        samples.setSample(0, EcalMGPASample(200, 1));
        samples.setSample(1, EcalMGPASample(200, 1));
        samples.setSample(2, EcalMGPASample(200, 1));
        samples.setSample(3, EcalMGPASample(205, 1));
        samples.setSample(4, EcalMGPASample(320, 1));
        samples.setSample(5, EcalMGPASample(350, 1));
        samples.setSample(6, EcalMGPASample(330, 1));
        samples.setSample(7, EcalMGPASample(296, 1));
        samples.setSample(8, EcalMGPASample(268, 1));
        samples.setSample(9, EcalMGPASample(245, 1));

        for (int i = 0; i < 10; ++i) {
                printf("%d --> %d %d\n", i, samples[i].adc(), samples[i].gainId());
        }

        // for FixedAlphaBeta fit
        //EcalUncalibratedRecHit urh(algo.makeRecHit(samples, pedVec, gainRatios, 0, 0));
        // provide weights for RecWeights algorithm
        // ** from trivial data/, _CMS flavour **
        EcalWeightSet::EcalWeightMatrix weights[2];
        typedef ROOT::Math::SVector<double,EcalDataFrame::MAXSAMPLES> SV;

        double w_aBGS[] = {-0.3812788, -0.3812788, -0.3812788, 0, 0.235699, 0.4228363, 0.3298652, 0.1575187, -0.002082776, 0};
        SV ww_amplitudeBGS(w_aBGS, sizeof(w_aBGS) / sizeof(double));
        double w_aAGS[] = {0, 0, 0, 0, 0, 1, 0, 0, 0, 0};
        SV ww_amplitudeAGS(w_aAGS, sizeof(w_aAGS) / sizeof(double));

        double w_pBGS[] = {0.305792, 0.305792, 0.305792, 0, 0.01323796, -0.07549743, -0.03141308, 0.05030892, 0.1259876, 0};
        SV ww_pedestalsBGS(w_pBGS, sizeof(w_pBGS) / sizeof(double));
        double w_pAGS[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        SV ww_pedestalsAGS(w_pAGS, sizeof(w_pAGS) / sizeof(double));

        double w_jBGS[] = {0.04066309, 0.04066309, 0.04066309, 0, 1.325176, -0.04997078, -0.504338, -0.5024844, -0.3903718, 0};
        SV ww_jitterBGS(w_jBGS, sizeof(w_jBGS) / sizeof(double));
        double w_jAGS[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        SV ww_jitterAGS(w_jAGS, sizeof(w_jAGS) / sizeof(double));

        weights[0].Place_in_row(ww_amplitudeBGS, 0, 0);
        weights[0].Place_in_row(ww_pedestalsBGS, 1, 0);
        weights[0].Place_in_row(ww_jitterBGS, 2, 0);

        weights[1].Place_in_row(ww_amplitudeAGS, 0, 0);
        weights[1].Place_in_row(ww_pedestalsAGS, 1, 0);
        weights[1].Place_in_row(ww_jitterAGS, 2, 0);

        const EcalWeightSet::EcalWeightMatrix * wei = &weights[0];
        
        // for Weight algo fit
        EcalUncalibratedRecHit urh(algo.makeRecHit(samples, pedVec, 0, gainRatios, &wei));

        printf("amplitude: %f %f %f\n", urh.amplitude(), urh.pedestal(), urh.jitter());

        return 0;
}

#include <vector>
#include <stdint.h>

typedef uint32_t DetId;

namespace edm {

        class Suppress_LogDebug_ 
        { 
                // With any decent optimization, use of Suppress_LogDebug_ (...)
                // including streaming of items to it via operator<<
                // will produce absolutely no executable code.
                public:
                        template< class T >
                                Suppress_LogDebug_ &operator<< (T const&) { return *this; } // Change log 12
                        Suppress_LogDebug_ &operator<< (std::ostream&(*)(std::ostream&)) { return *this; }  // Change log 12
                        Suppress_LogDebug_ &operator<< (std::ios_base&(*)(std::ios_base&)) { return *this; } // Change log 12
        };  // Suppress_LogDebug_

}

#define LogDebug(id) true ? edm::Suppress_LogDebug_() : edm::Suppress_LogDebug_()

class EcalUncalibratedRecHit {

        public:

                typedef DetId key_type;

                enum Flags {
                        kGood=-1,                 // channel is good (mutually exclusive with other states)  setFlagBit(kGood) reset flags_ to zero 
                        kPoorReco,                // channel has been badly reconstructed (e.g. bad shape, bad chi2 etc.)
                        kSaturated,               // saturated channel
                        kOutOfTime,               // channel out of time
                        kLeadingEdgeRecovered,    // saturated channel: energy estimated from the leading edge before saturation
                        kHasSwitchToGain6,        // at least one data frame is in G6
                        kHasSwitchToGain1         // at least one data frame is in G1

                };

                EcalUncalibratedRecHit() :
                        amplitude_(0.), pedestal_(0.), jitter_(0.), chi2_(10000.), OOTamplitude_(0.), OOTchi2_(10000.), flags_(0), aux_(0) { }

                EcalUncalibratedRecHit(const DetId& id, float ampl, float ped,
                                       float jit, float chi2, uint32_t flags = 0, uint32_t aux = 0):
                        amplitude_(ampl), pedestal_(ped), jitter_(jit), chi2_(chi2), OOTamplitude_(0.), OOTchi2_(10000.), flags_(flags), aux_(aux), id_(id) { }


                float amplitude() const { return amplitude_; }
                float pedestal() const { return pedestal_; }
                float jitter() const { return jitter_; }
                float chi2() const { return chi2_; }
                float outOfTimeEnergy() const { return OOTamplitude_; }
                float outOfTimeChi2() const { return OOTchi2_; }

                uint32_t flags() const { return flags_; }
                float jitterError() const;
                uint8_t jitterErrorBits() const;
                DetId  id() const { return id_; }

                void setAmplitude( float amplitude ) { amplitude_ = amplitude; }
                void setPedestal( float pedestal ) { pedestal_ = pedestal; }
                void setJitter( float jitter ) { jitter_ = jitter; }
                void setChi2( float chi2 ) { chi2_ = chi2; }
                void setOutOfTimeEnergy( float energy ) { OOTamplitude_ = energy; }
                void setOutOfTimeChi2( float chi2 ){ OOTchi2_ = chi2; }

                void setJitterError( float jitterErr );
                void setFlags( uint32_t flags ) { flags_ = flags; }
                void setId( DetId id ) { id_ = id; }
                void setAux( uint32_t aux ) { aux_ = aux; }
                void setFlagBit(Flags flag);
                bool checkFlag(Flags flag) const;

                bool isSaturated() const;
                bool isJitterValid() const;
                bool isJitterErrorValid() const;

        private:
                float amplitude_;    //< Reconstructed amplitude
                float pedestal_;     //< Reconstructed pedestal
                float jitter_;       //< Reconstructed time jitter
                float chi2_;         //< Chi2 of the pulse
                float OOTamplitude_; //< Out-Of-Time reconstructed amplitude 
                float OOTchi2_;      //< Out-Of-Time Chi2 
                uint32_t flags_;     //< flag to be propagated to RecHit
                uint32_t aux_;       //< aux word; first 8 bits contain time (jitter) error
                DetId  id_;          //< Detector ID
};


namespace EcalWeightSet {
        typedef void EcalWeightMatrix;
        typedef void EcalChi2WeightMatrix;
}


template<class C> class EcalUncalibRecHitRecAbsAlgo
{
        public:
                enum { nWeightsRows = 3, iAmplitude = 0, iPedestal = 1, iTime = 2 };

                /// Constructor
                //EcalUncalibRecHitRecAbsAlgo() { };

                /// Destructor
                //virtual ~EcalUncalibRecHitRecAbsAlgo() { };

                /// make rechits from dataframes

                virtual EcalUncalibratedRecHit makeRecHit(const C& dataFrame, 
                                                          const double* pedestals,
                                                          const double* gainRatios,
                                                          const EcalWeightSet::EcalWeightMatrix** weights, 
                                                          const EcalWeightSet::EcalChi2WeightMatrix** chi2Matrix) = 0;

};

namespace edm {
        template <typename T>
                bool isFinite(T x);

        template <typename T>
                inline
                bool isNotFinite(T x) {
                        return !isFinite(x);
                }

        template<>
                inline
                bool isFinite(float x) {
                        const unsigned int mask =  0x7f800000;
                        union { unsigned int l; float d;} v;
                        v.d =x;
                        return (v.l&mask)!=mask;
                }

        template<>
                inline
                bool isFinite(double x) {
                        const unsigned long long mask = 0x7FF0000000000000LL;
                        union { unsigned long long l; double d;} v;
                        v.d =x;
                        return (v.l&mask)!=mask;
                }

        template<>
                inline
                bool isFinite(long double x) {
                        double xx=x;
                        return isFinite(xx);
                }

}


#include <vector>
#include <stdint.h>

//typedef uint32_t DetId;
//class DetId
//{
//        public:
//                uint32_t id() { return id_; }
//        private:
//                uint32_t id_;
//};
//
//typedef DetId EBDetId;

//FIXME shall be removed and implemented where the operator is defined
#include <ostream>

#include <stdint.h>
/** \class DetId

  Parent class for all detector ids in CMS.  The DetId is a 32-bit
  unsigned integer.  The four most significant bits ([31:28]) identify
  the large-scale detector (e.g. Tracker or Ecal) while the next three
  bits ([27:25]) identify a part of the detector (such as HcalBarrel
  (HB) for Hcal).

*/
class DetId {
        public:
                static const int kDetOffset          = 28;
                static const int kSubdetOffset       = 25;


                enum Detector { Tracker=1,Muon=2,Ecal=3,Hcal=4,Calo=5 };
                /// Create an empty or null id (also for persistence)
                DetId()  : id_(0) { }
                /// Create an id from a raw number
                DetId(uint32_t id) : id_(id) { }
                /// Create an id, filling the detector and subdetector fields as specified
                DetId(Detector det, int subdet)  {
                        id_=((det&0xF)<<28)|((subdet&0x7)<<25);
                }

                /// get the detector field from this detid
                Detector det() const { return Detector((id_>>kDetOffset)&0xF); }
                /// get the contents of the subdetector field (not cast into any detector's numbering enum)
                int subdetId() const { return ((id_>>kSubdetOffset)&0x7); }

                uint32_t operator()() const { return id_; }
                operator uint32_t() const { return id_; }

                /// get the raw id 
                uint32_t rawId() const { return id_; }
                /// is this a null id ?
                bool null() const { return id_==0; }

                /// equality
                bool operator==(DetId id) const { return id_==id.id_; }
                /// inequality
                bool operator!=(DetId id) const { return id_!=id.id_; }
                /// comparison
                bool operator<(DetId id) const { return id_<id.id_; }

        protected:
                uint32_t id_;
};

/// equality
inline bool operator==(uint32_t i, DetId id)  { return i==id(); }
inline bool operator==(DetId id, uint32_t i)  { return i==id(); }
/// inequality
inline bool operator!=(uint32_t i, DetId id)  { return i!=id(); }
inline bool operator!=(DetId id, uint32_t i) { return i!=id(); }
/// comparison
inline bool operator<(uint32_t i, DetId id) { return i<id(); }
inline bool operator<(DetId id, uint32_t i) { return id()<i; }

//typedef DetId EBDetId;
enum EcalSubdetector { EcalBarrel=1, EcalEndcap=2, EcalPreshower=3, EcalTriggerTower=4, EcalLaserPnDiode=5 };

class EBDetId : public DetId {
        public:
                enum { Subdet=EcalBarrel};
                static EcalSubdetector subdet() { return EcalBarrel;}
                /** Constructor of a null id */
                EBDetId() {}
                /** Constructor from a raw value */
                EBDetId(uint32_t rawid) : DetId(rawid) {}
                /** Constructor from crystal ieta and iphi 
                  or from SM# and crystal# */
                // fast
                //EBDetId(  int crystal_ieta, int crystal_iphi) : DetId(Ecal,EcalBarrel) { id_|=((crystal_ieta>0)?(0x10000|(crystal_ieta<<9)):((-crystal_ieta)<<9))|(crystal_iphi&0x1FF); }
                EBDetId(const DetId& id) : DetId(id){}
                /** Assignment operator from cell id */
                EBDetId& operator=(const DetId& id) {
                        id_=id.rawId();
                        return *this;
                }
};

std::ostream& operator<<(std::ostream& s,const EBDetId& id);

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

#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/counting_iterator.hpp>

#include<vector>
#include<algorithm>

namespace edm {

        class DataFrameContainer;

        /* a proxy to a fixed size array of 16bit words belonging to
         * a "channel" identified by an 32bit id
         *
         * FIXME interface to be finalized once use-cases fully identified
         * 
         */
        class DataFrame {
                public: 

                        typedef unsigned int size_type; // for persistency
                        typedef unsigned int id_type;
                        typedef unsigned short data_type;
                        typedef data_type * iterator;
                        typedef data_type const * const_iterator;


                        inline
                                DataFrame() : m_id(0), m_data(0), m_size(0){}
                        inline
                                DataFrame(id_type i, data_type const * idata, size_type isize) :
                                        m_id(i), m_data(idata), m_size(isize) {}

                        inline
                                DataFrame(DataFrameContainer const & icont,
                                          size_type i);
                        inline
                                void set(DataFrameContainer const & icont,
                                         size_type i);
                        inline
                                data_type & operator[](size_type i) {
                                        return data()[i];
                                }

                        inline
                                data_type operator[](size_type i) const {
                                        return m_data[i];
                                }

                        inline
                                iterator begin() { return data();}

                        inline
                                iterator end() { return data()+m_size;}

                        inline
                                const_iterator begin() const { return m_data;}

                        inline
                                const_iterator end() const { return m_data+m_size;}


                        inline
                                id_type id() const { return m_id;}

                        inline
                                size_type size() const { return m_size; }

                private:
                        data_type * data() {
                                return const_cast<data_type *>(m_data);
                        }

                        id_type m_id;
                        data_type const * m_data;
                        size_type m_size;
        };
}


namespace edm {

        /** an optitimized container that linearized a "vector of vector".
         *  It corresponds to a set of fixed size array of 16bit words each belonging
         *  to a "channel" identified by an 32bit id
         *
         * FIXME interface to be finalized once use-cases fully identified
         *
         * although it can be sorted internally it is strongly adviced to
         * fill it already sorted....
         *
         */
        class DataFrameContainer {
                public:
                        typedef unsigned int size_type; // for persistency
                        typedef unsigned int id_type;
                        typedef unsigned short data_type;
                        typedef std::vector<id_type> IdContainer;
                        typedef std::vector<data_type> DataContainer;
                        typedef std::vector<id_type>::iterator IdIter;
                        typedef std::vector<data_type>::iterator DataIter;
                        typedef std::pair<IdIter,DataIter> IterPair;
                        typedef std::vector<id_type>::const_iterator const_IdIter;
                        typedef std::vector<data_type>::const_iterator const_DataIter;
                        typedef std::pair<const_IdIter,const_DataIter> const_IterPair;

                        struct IterHelp {
                                typedef DataFrame result_type;
                                IterHelp(DataFrameContainer const & iv) : v(iv){}

                                DataFrame const & operator()(int i) const {
                                        frame.set(v,i);
                                        return frame;
                                } 
                                private:
                                DataFrameContainer const & v;
                                mutable DataFrame frame;
                        };

                        typedef boost::transform_iterator<IterHelp,boost::counting_iterator<int> > const_iterator;

                        DataFrameContainer() :
                                m_subdetId(0), m_stride(0),
                                m_ids(), m_data(){}

                        explicit DataFrameContainer(size_t istride, int isubdet=0, size_t isize=0) :
                                m_subdetId(isubdet), m_stride(istride),
                                m_ids(isize), m_data(isize*m_stride){}

                        void swap(DataFrameContainer & rh) {
                                std::swap(m_subdetId,rh.m_subdetId);
                                std::swap(m_stride,rh.m_stride);
                                m_ids.swap(rh.m_ids);
                                m_data.swap(rh.m_data);
                        }

                        DataFrameContainer& operator=(DataFrameContainer const& rhs) {
                                DataFrameContainer temp(rhs);
                                this->swap(temp);
                                return *this;
                        }

                        void swap(IdContainer & iic, DataContainer & idc) {
                                m_ids.swap(iic);
                                m_data.swap(idc);
                        }

                        void reserve(size_t isize) {
                                m_ids.reserve(isize);
                                m_data.reserve(isize*m_stride);
                        }

                        void resize(size_t isize) {
                                m_ids.resize(isize);
                                m_data.resize(isize*m_stride);
                        }

                        void sort();

                        // FIXME not sure what the best way to add one cell to cont
                        void push_back(id_type iid, data_type const * idata) {
                                m_ids.push_back(iid);
                                size_t cs = m_data.size();
                                m_data.resize(m_data.size()+m_stride);
                                std::copy(idata,idata+m_stride,m_data.begin()+cs);
                        }
                        //make space for it
                        void push_back(id_type iid) {
                                m_ids.push_back(iid);
                                m_data.resize(m_data.size()+m_stride);
                        }
                        // overwrite back (very ad hoc interface...)
                        void set_back(id_type iid, data_type const * idata) {
                                m_ids.back() = iid;
                                size_t cs = m_data.size()-m_stride;
                                std::copy(idata,idata+m_stride,m_data.begin()+cs);
                        }
                        void set_back(id_type iid) {
                                m_ids.back() = iid;
                        }
                        void set_back(data_type const * idata) {
                                size_t cs = m_data.size()-m_stride;
                                std::copy(idata,idata+m_stride,m_data.begin()+cs);
                        }

                        DataFrame back() {
                                return DataFrame(*this,size()-1);
                        }

                        void pop_back() {
                                m_ids.resize(m_ids.size()-1);
                                m_data.resize(m_data.size()-m_stride);
                        }

                        //---------------------------------------------------------

                        IterPair pair(size_t i) {
                                return IterPair(m_ids.begin()+i,m_data.begin()+i*m_stride);
                        }

                        const_IterPair pair(size_t i) const {
                                return const_IterPair(m_ids.begin()+i,m_data.begin()+i*m_stride);
                        }

                        DataFrame operator[](size_t i) {
                                return DataFrame(*this,i);
                        }

                        DataFrame operator[](size_t i) const {
                                return DataFrame(*this,i);
                        }

                        // slow interface
                        const_iterator find(id_type i) const {
                                const_IdIter p = std::lower_bound(m_ids.begin(),m_ids.end(),i);
                                return (p==m_ids.end() || (*p)!=i) ? end() :
                                        boost::make_transform_iterator(boost::counting_iterator<int>(p-m_ids.begin()),
                                                                       IterHelp(*this));
                        }

                        const_iterator begin() const {
                                return  boost::make_transform_iterator(boost::counting_iterator<int>(0),
                                                                       IterHelp(*this));
                        }
                        const_iterator end() const {
                                return  boost::make_transform_iterator(boost::counting_iterator<int>(size()),
                                                                       IterHelp(*this));
                        }


                        int subdetId() const { return m_subdetId; }

                        size_type stride() const { return m_stride; }

                        bool empty() const { return m_ids.empty();}

                        size_type size() const { return m_ids.size();}

                        data_type operator()(size_t cell, size_t frame) const {
                                return m_data[cell*m_stride+frame];
                        }

                        data_type const * frame(size_t cell) const {
                                return &m_data[cell*m_stride];
                        }

                        id_type id(size_t cell) const {
                                return m_ids[cell];
                        }

                        // IdContainer const & ids() const { return m_ids;}
                        // DataContainer const & data() const { return  m_data;}

                private:
                        // subdetector id (as returned by  DetId::subdetId())
                        int m_subdetId;

                        // can be a enumerator, or a template argument
                        size_type m_stride;

                        IdContainer m_ids;
                        DataContainer m_data;

        };

        inline
                DataFrame::DataFrame(DataFrameContainer const & icont,
                                     size_type i) :
                        m_id(icont.id(i)), m_data(icont.frame(i)), m_size(icont.stride()){}

        inline
                void DataFrame::set(DataFrameContainer const & icont,
                                    size_type i) {
                        m_id=icont.id(i); 
                        m_data=icont.frame(i);
                        m_size=icont.stride();
                }

        // Free swap function
        inline
                void
                swap(DataFrameContainer& lhs, DataFrameContainer& rhs) {
                        lhs.swap(rhs);
                }

}

// The standard allows us to specialize std::swap for non-templates.
// This ensures that DataFrameContainer::swap() will be used in algorithms.

namespace std {
        template <> inline void swap(edm::DataFrameContainer& lhs, edm::DataFrameContainer& rhs) {  
                lhs.swap(rhs);
        }
}

class EcalMGPASample {
        public:
                EcalMGPASample() { theSample=0; }
                EcalMGPASample(uint16_t data) { theSample=data; }
                EcalMGPASample(int adc, int gainId);

                /// get the raw word
                uint16_t raw() const { return theSample; }
                /// get the ADC sample (12 bits)
                int adc() const { return theSample&0xFFF; }
                /// get the gainId (2 bits)
                int gainId() const { return (theSample>>12)&0x3; }
                /// for streaming
                uint16_t operator()() const { return theSample; }
                operator uint16_t () const { return theSample; }

        private:
                uint16_t theSample;
};

std::ostream& operator<<(std::ostream&, const EcalMGPASample&);

EcalMGPASample::EcalMGPASample(int adc,int gainId) {
        theSample=(adc&0xFFF) | ((gainId&0x3)<<12);
}

std::ostream& operator<<(std::ostream& s, const EcalMGPASample& samp) {
        s << "ADC=" << samp.adc() << ", gainId=" << samp.gainId();
        return s;
}

class EcalDigiCollection : public edm::DataFrameContainer {
        public:
                typedef edm::DataFrameContainer::size_type size_type;
                static const size_type MAXSAMPLES = 10;
                explicit EcalDigiCollection(size_type istride=MAXSAMPLES, int isubdet=0)  : 
                        edm::DataFrameContainer(istride, isubdet){}
                void swap(DataFrameContainer& other) {this->DataFrameContainer::swap(other);}
};

inline
void swap(EcalDigiCollection& lhs, EcalDigiCollection& rhs) {
        lhs.swap(rhs);
}

class EcalDataFrame {
        public:
                EcalDataFrame() {}
                //EcalDataFrame(DetId i) :  m_data(i) {}
                EcalDataFrame(edm::DataFrame const & iframe) : m_data(iframe){} 

                virtual ~EcalDataFrame() {} 

                DetId id() const { return m_data.id();}

                int size() const { return m_data.size();}

                EcalMGPASample operator[](int i) const { return m_data[i];}
                EcalMGPASample sample(int i) const { return m_data[i]; }

                // get the leading sample (the first non saturated sample)
                // starting from the fourth sample
                // (it relies on the fact that the unpaker will discard fixed gain0 DataFrame)
                // .. sample numbering: [0, 9]
                // .. return -1 in case of no saturation
                int lastUnsaturatedSample() const;
                // just the boolean method
                bool isSaturated() const { return ( lastUnsaturatedSample() != -1 ); }

                // FIXME (shall we throw??)
                void setSize(int){}
                // void setPresamples(int ps);
                void setSample(int i, EcalMGPASample sam) { m_data[i]=sam; }

                bool hasSwitchToGain6() const; 
                bool hasSwitchToGain1() const; 

                static const int MAXSAMPLES = 10;

                edm::DataFrame const & frame() const { return m_data;}
                edm::DataFrame & frame() { return m_data;}

        private:

                edm::DataFrame m_data;

};

class EBDataFrame : public EcalDataFrame 
{
        public:
                typedef EBDetId key_type; ///< For the sorted collection
                typedef EcalDataFrame Base;

                EBDataFrame() {}
                // EBDataFrame(DetId i) :  Base(i) {}
                EBDataFrame(edm::DataFrame const & base) : Base(base) {}
                EBDataFrame(EcalDataFrame const & base) : Base(base) {}

                virtual ~EBDataFrame() {}

                key_type id() const { return Base::id(); }
};

std::ostream& operator<<(std::ostream&, const EBDataFrame&);


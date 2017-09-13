/*-----------------------------------------------------------/
 iedkToolkit.h
 
 github.com/azuremous
 Created by Jung un Kim a.k.a azuremous on 9/8/17.
 /----------------------------------------------------------*/

#include "IEmoStateDLL.h"
#include "Iedk.h"
#include "IedkErrorCode.h"
#include "IEmoStatePerformanceMetric.h"
#include "IEegData.h"

#include "ofUtils.h"
#include "ofEvents.h"

namespace ofxIedkToolKit {
    struct dataChannel{
        double alpha, low_beta, high_beta, gamma, theta;
    };
    
    typedef enum {
        USE_EVENT_EMOTION,
        USE_EVENT_MOTION,
        USE_EVENT_MENTAL,
        USE_EVENT_FACIAL,
        DISUSE_EVENT_EMOTION,
        DISUSE_EVENT_MOTION,
        DISUSE_EVENT_MENTAL,
        DISUSE_EVENT_FACIAL,
    }IEDK_EVENT_TYPE;
    
    typedef enum {
        EMOTIV_INSIGHT,
        EMOTIV_EPOCPLUS,
    }EMOTIV_DEVICE_TYPE;
    
    class iedkToolkit {
    private:
        
        const vector<IEE_DataChannel_t> insightChannelList;
        const vector<IEE_DataChannel_t> epocplusChannelList;
        
        EmoEngineEventHandle eEvent;
        EmoStateHandle eState;
        IEE_Event_t currentEvent;
        
        int state;
        unsigned int userID;
        int deviceSize;
        vector<int>IEEQuality;
        
        vector<dataChannel>channels;
        float excitement, relaxation, stress, engagement, interest, focus;
        
        bool isReady;
        bool bConnected;
        
        bool useEmotionEvent;
        bool useMotionEvent;
        bool useMentalEvent;
        bool useFacialEvent;
        
        string debugEventType;
        string licenseInfo;
        
        EMOTIV_DEVICE_TYPE deviceType;
        
    protected:
        void update(ofEventArgs &event);
        
        void checkStatus();
        void checkIEEQuality();
        void updateEmotion();
        void getAverageBandPower();
        int getQuality(IEE_InputChannels_t ch);
        
    public:
        
        iedkToolkit();
        ~iedkToolkit();
        
        void setDevice(EMOTIV_DEVICE_TYPE type);
        void addEvent(IEDK_EVENT_TYPE event);
        void connect(int _id = 0, bool use = false);
        void disconnect();
        
        float getBatteryStatus();
        
        inline int size() const { return deviceSize; }
        inline int getChannelSize() { return channels.size(); }
        inline int getIEEQuality(int n) { return IEEQuality[n]; }
        
        inline int getExcitement() { return (int)(excitement * 100.); }
        inline int getRelaxation() { return (int)(relaxation * 100.); }
        inline int getStress() { return (int)(stress * 100.); }
        inline int getEngagement() { return (int)(engagement * 100.); }
        inline int getInterest() { return (int)(interest * 100.); }
        inline int getFocus() { return (int)(focus * 100.); }
        
        bool isConnected() const { return isReady; }
        
        vector<dataChannel> getBandPowerData() const { return channels; }
        vector<int> getIEEQuality() const { return IEEQuality; }
        string GetEventName() const { return debugEventType; }
        
    };

}

typedef ofxIedkToolKit::iedkToolkit ofxIedk;

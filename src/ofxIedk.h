/*-----------------------------------------------------------/
 ofxIedk.h
 
 github.com/azuremous
 Created by Jung un Kim a.k.a azuremous on 9/8/17.
 /----------------------------------------------------------*/

#include "IEmoStateDLL.h"
#include "Iedk.h"
#include "IedkErrorCode.h"
#include "IEmoStatePerformanceMetric.h"

#include "ofUtils.h"
#include "ofEvents.h"

const IEE_DataChannel_t channelList[] = { IED_AF3, IED_AF4, IED_T7, IED_T8, IED_Pz };
const string header[] = { "Theta", "Alpha", "Low_beta", "High_beta", "Gamma"};
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

class ofxIedk {
private:
    
    
    EmoEngineEventHandle eEvent;
    EmoStateHandle eState;
    IEE_Event_t currentEvent;
    
    int state;
    unsigned int userID;
    int channelSize;
    int deviceSize;
    
    double alpha, low_beta, high_beta, gamma, theta;
    float excitement, relaxation, stress, engagement, interest, focus;
    
    bool isReady;
    bool bConnected;
    bool bUpdateEmotion;
    
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
    float getEmotion(IEE_PerformanceMetricAlgo_enum type);
    IEE_EEG_ContactQuality_t getQuality(IEE_InputChannels_t ch);
    
public:
    
    ofxIedk();
    ~ofxIedk();

    void setDevice(EMOTIV_DEVICE_TYPE type);
    void addEvent(IEDK_EVENT_TYPE event);
    void connect(int _id = 0);
    void disconnect();
    
    float getBatteryStatus();

    inline int size() const { return deviceSize; }
    
    inline int getExcitement() { return (int)(excitement * 100.); }
    inline int getRelaxation() { return (int)(relaxation * 100.); }
    inline int getStress() { return (int)(stress * 100.); }
    inline int getEngagement() { return (int)(engagement * 100.); }
    inline int getInterest() { return (int)(interest * 100.); }
    inline int getFocus() { return (int)(focus * 100.); }
    
    bool isConnected() const { return isReady; }
    bool isUpdatedEmotions() const { return bUpdateEmotion; }

    string GetEventName() const { return debugEventType; }
    
};

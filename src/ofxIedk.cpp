#include "ofxIedk.h"

using namespace ofxIedkToolKit;

//--------------------------------------------------------------
iedkToolkit::iedkToolkit()
:state(0)
,userID(0)
,deviceSize(0)
,isReady(false)
,bConnected(false)
,useEmotionEvent(false)
,useMotionEvent(false)
,useMentalEvent(false)
,useFacialEvent(false)
,debugEventType("")
,licenseInfo("")
,insightChannelList{ IED_AF3, IED_AF4, IED_T7, IED_T8, IED_Pz }
,epocplusChannelList{ IED_AF3, IED_F7, IED_F3, IED_FC5, IED_T7, IED_P7, IED_O1, IED_O2, IED_P8, IED_T8, IED_FC6, IED_F4, IED_F8, IED_AF4 }
,deviceType(EMOTIV_INSIGHT)

{
    eEvent = IEE_EmoEngineEventCreate();
    eState = IEE_EmoStateCreate();
    IEE_EmoInitDevice(); //use BTLE
    if (IEE_EngineConnect() != EDK_OK) {
        ofLog(OF_LOG_ERROR, "Emotiv Driver start up failed.");
    }
    setDevice(deviceType);
    ofAddListener(ofEvents().update, this, &iedkToolkit::update);
}

//--------------------------------------------------------------
iedkToolkit::~iedkToolkit(){
    IEE_EngineDisconnect();
    IEE_EmoStateFree(eState);
    IEE_EmoEngineEventFree(eEvent);
}

//--------------------------------------------------------------
void iedkToolkit::setDevice(EMOTIV_DEVICE_TYPE type){
    deviceType = type;
    if(deviceType == EMOTIV_INSIGHT) { channels.resize(insightChannelList.size()); }
    //AF3, AF4, T7, T8, Pz
    else if(deviceType == EMOTIV_EPOCPLUS) { channels.resize(epocplusChannelList.size()); }
    //AF3, F7, F3, FC5, T7, P7, O1, O2, P8, T8, FC6, F4, F8, AF4
    IEEQuality.resize(channels.size());
}

//--------------------------------------------------------------
void iedkToolkit::addEvent(IEDK_EVENT_TYPE event){
    switch(event){
        case USE_EVENT_EMOTION:
            useEmotionEvent = true;
            break;
        case USE_EVENT_MOTION:
            useMotionEvent = true;
            break;
        case USE_EVENT_MENTAL:
            useMentalEvent = true;
            break;
        case USE_EVENT_FACIAL:
            useFacialEvent = true;
            break;
        case DISUSE_EVENT_EMOTION:
            useEmotionEvent = false;
            break;
        case DISUSE_EVENT_MOTION:
            useMotionEvent = false;
            break;
        case DISUSE_EVENT_MENTAL:
            useMentalEvent = false;
            break;
        case DISUSE_EVENT_FACIAL:
            useFacialEvent = false;
            break;
    }
}

//--------------------------------------------------------------
void iedkToolkit::connect(int _id, bool use){
    if(deviceSize > 0 && !bConnected) {
        if(deviceType == EMOTIV_INSIGHT){
            string name = IEE_GetInsightDeviceName(_id);
            IEE_ConnectInsightDevice(_id);
            ofLog(OF_LOG_VERBOSE, "connect to "+ name);//OF_LOG_VERBOSE
        }else if(deviceType == EMOTIV_EPOCPLUS){
            string name = IEE_GetEpocPlusDeviceName(_id);
            IEE_ConnectEpocPlusDevice(_id);
            ofLog(OF_LOG_VERBOSE, "connect to "+ name);//OF_LOG_VERBOSE
        }
        bConnected = true;
    }
    //for Multi Dongle Connection
    //IEE_DataAcquisitionEnable(_id, use);
    //IEE_DataUpdateHandle();
}

//--------------------------------------------------------------
void iedkToolkit::disconnect(){
    IEE_DisconnectDevice();
    bConnected = false;
}

//--------------------------------------------------------------
float iedkToolkit::getBatteryStatus(){
    int batteryLevel, maxBatteryLevel;
    int wirelessStrength = IS_GetWirelessSignalStatus(eState);
    if(wirelessStrength != NO_SIG){
        IS_GetBatteryChargeLevel(eState, &batteryLevel, &maxBatteryLevel);
        return (float) batteryLevel / (float) maxBatteryLevel;
    }
    return -1;
}

/*protected*///--------------------------------------------------------------
void iedkToolkit::checkStatus(){
    if(bConnected){
        state = IEE_EngineGetNextEvent(eEvent);
        
        if(state == EDK_OK){
            currentEvent = IEE_EmoEngineEventGetType(eEvent);
            IEE_EmoEngineEventGetUserId(eEvent, &userID);
            
            switch (currentEvent) {
                case IEE_UserAdded:
                    
                    isReady = true;
                    debugEventType = "connected";
                    ofLog(OF_LOG_NOTICE, "A headset connected. reday to start");
                    IEE_FFTSetWindowingType(userID, IEE_HAMMING);
                    
                    break;
                    
                case IEE_UserRemoved:
                    
                    isReady = false;
                    debugEventType = "disconnected";
                    ofLog(OF_LOG_NOTICE, "A headset has been disconnected");
                    
                    break;
                    
                case IEE_EmoStateUpdated:
                    
                    debugEventType = "updated";
                    IEE_EmoEngineEventGetEmoState(eEvent, eState);
                    
                    break;
                    
                case IEE_UnknownEvent:
                    
                    debugEventType = "unknown event";
                    
                    break;
                    
                case IEE_EmulatorError:
                    debugEventType = "emulator error";
                    break;
                    
                case IEE_ReservedEvent:
                    debugEventType = "reserved event";
                    break;
                    
                case IEE_ProfileEvent:
                    debugEventType = "profile Event";
                    break;
                    
                case IEE_MentalCommandEvent:
                    debugEventType = "mentalCommand event";
                    break;
                    
                case IEE_FacialExpressionEvent:
                    debugEventType = "facialExpression event";
                    break;
                    
                case IEE_InternalStateChanged:
                    debugEventType = "internal state changed";
                    break;
                    
                case IEE_AllEvent:
                    debugEventType = "all event";
                    break;
            }
            
        }
    }
}

//--------------------------------------------------------------
void iedkToolkit::updateEmotion(){
    
    excitement = IS_PerformanceMetricGetInstantaneousExcitementScore(eState);
    relaxation = IS_PerformanceMetricGetRelaxationScore(eState);
    stress = IS_PerformanceMetricGetStressScore(eState);
    engagement = IS_PerformanceMetricGetEngagementBoredomScore(eState);
    interest = IS_PerformanceMetricGetInterestScore(eState);
    focus = IS_PerformanceMetricGetFocusScore(eState);

}

//--------------------------------------------------------------
void iedkToolkit::checkIEEQuality(){
    
    vector<IEE_DataChannel_t> list;
    if(deviceType == EMOTIV_INSIGHT){ list = insightChannelList; }
    else if(deviceType == EMOTIV_EPOCPLUS){ list = epocplusChannelList; }
    
    int wirelessStrength = IS_GetWirelessSignalStatus(eState);
    if(wirelessStrength != NO_SIG){
        for(int i = 0; i < channels.size(); i++){
            IEE_InputChannels_t input;
            switch(list[i]){
                case IED_AF3:
                    input = IEE_CHAN_AF3;
                    break;
                case IED_AF4:
                    input = IEE_CHAN_AF4;
                    break;
                case IED_T7:
                    input = IEE_CHAN_T7;
                    break;
                case IED_T8:
                    input = IEE_CHAN_T8;
                    break;
                case IED_Pz:
                    input = IEE_CHAN_Pz;
                    break;
            }
            IEEQuality[i] = getQuality(input);
        }
    }
    
    /*
     need add for epoc+
     F7, F3, FC5, , P7, O1, O2, P8, , FC6, F4, F8, 
     typedef enum IEE_InputChannels_enum {
     
     IEE_CHAN_CMS = 0,
     IEE_CHAN_DRL,
     IEE_CHAN_FP1,
     IEE_CHAN_AF3,
     IEE_CHAN_F7,
     IEE_CHAN_F3,
     IEE_CHAN_FC5,
     IEE_CHAN_T7,
     IEE_CHAN_P7,
     IEE_CHAN_Pz,
     IEE_CHAN_O1 = IEE_CHAN_Pz,
     IEE_CHAN_O2,
     IEE_CHAN_P8,
     IEE_CHAN_T8,
     IEE_CHAN_FC6,
     IEE_CHAN_F4,
     IEE_CHAN_F8,
     IEE_CHAN_AF4,
     IEE_CHAN_FP2,
     } IEE_InputChannels_t;
     */
    
}

//--------------------------------------------------------------
void iedkToolkit::getAverageBandPower(){
    
    vector<IEE_DataChannel_t> list;
    if(deviceType == EMOTIV_INSIGHT){ list = insightChannelList; }
    else if(deviceType == EMOTIV_EPOCPLUS){ list = epocplusChannelList; }
    
    for(int i = 0; i < channels.size(); i++){
        int result = IEE_GetAverageBandPowers(
                                              userID
                                              ,list[i]
                                              ,&channels[i].theta
                                              ,&channels[i].alpha
                                              ,&channels[i].low_beta
                                              ,&channels[i].high_beta
                                              ,&channels[i].gamma
                                              );
    }
}

//--------------------------------------------------------------
void iedkToolkit::update(ofEventArgs &event){
    checkStatus();
    checkIEEQuality();
    if(!bConnected) { deviceSize = IEE_GetInsightDeviceCount() + IEE_GetEpocPlusDeviceCount(); }
    if(!isReady){ return; }
    
    if(useEmotionEvent) { updateEmotion(); }
    if(useMotionEvent) {}
    if(useMentalEvent) {}
    if(useFacialEvent) {}
    
    getAverageBandPower();
}

//--------------------------------------------------------------
int iedkToolkit::getQuality(IEE_InputChannels_t ch){
    return (int)IS_GetContactQuality(eState, ch);
}
//--------------------------------------------------------------

//--------------------------------------------------------------

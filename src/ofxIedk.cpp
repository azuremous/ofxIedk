#include "ofxIedk.h"

//--------------------------------------------------------------
ofxIedk::ofxIedk()
:state(0)
,userID(0)
,channelSize(0)
,deviceSize(0)
,alpha(0)
,low_beta(0)
,high_beta(0)
,gamma(0)
,theta(0)
,isReady(false)
,bConnected(false)
,bUpdateEmotion(false)
,useEmotionEvent(false)
,useMotionEvent(false)
,useMentalEvent(false)
,useFacialEvent(false)
,debugEventType("")
,licenseInfo("")
{
    eEvent = IEE_EmoEngineEventCreate();
    eState = IEE_EmoStateCreate();
    channelSize = sizeof(channelList)/sizeof(channelList[0]);
    IEE_EmoInitDevice(); //use BTLE
    if (IEE_EngineConnect() != EDK_OK) {
        ofLog(OF_LOG_ERROR, "Emotiv Driver start up failed.");
    }
    ofAddListener(ofEvents().update, this, &ofxIedk::update);
}

//--------------------------------------------------------------
ofxIedk::~ofxIedk(){
    IEE_EngineDisconnect();
    IEE_EmoStateFree(eState);
    IEE_EmoEngineEventFree(eEvent);
}

//--------------------------------------------------------------
void ofxIedk::setDevice(EMOTIV_DEVICE_TYPE type){
    deviceType = type;
}

//--------------------------------------------------------------
void ofxIedk::addEvent(IEDK_EVENT_TYPE event){
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
void ofxIedk::connect(int _id){
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
}

//--------------------------------------------------------------
void ofxIedk::disconnect(){
    IEE_DisconnectDevice();
    bConnected = false;
}

//--------------------------------------------------------------
float ofxIedk::getBatteryStatus(){
    int batteryLevel, maxBatteryLevel;
    int wirelessStrength = IS_GetWirelessSignalStatus(eState);
    if(wirelessStrength != NO_SIG){
        IS_GetBatteryChargeLevel(eState, &batteryLevel, &maxBatteryLevel);
        return (float) batteryLevel / (float) maxBatteryLevel;
    }
    return -1;
}

/*protected*///--------------------------------------------------------------
void ofxIedk::checkStatus(){
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
void ofxIedk::updateEmotion(){
    
    excitement = getEmotion(PM_EXCITEMENT);
    relaxation = getEmotion(PM_RELAXATION);
    stress = getEmotion(PM_STRESS);
    engagement = getEmotion(PM_ENGAGEMENT);
    interest = getEmotion(PM_INTEREST);
    focus = getEmotion(PM_FOCUS);
    
    if(excitement != -1 && relaxation != -1 && stress != -1 && engagement != -1 && interest != -1 && focus != -1){
        bUpdateEmotion = true;
    }else{
        bUpdateEmotion = false;
    }

}

//--------------------------------------------------------------
void ofxIedk::checkIEEQuality(){
    int wirelessStrength = IS_GetWirelessSignalStatus(eState);
    if(wirelessStrength != NO_SIG){
        
    }
    
}

//--------------------------------------------------------------
void ofxIedk::getAverageBandPower(){
    for(int i = 0; i < channelSize; i++){
        int result = IEE_GetAverageBandPowers(
                                              userID
                                              ,channelList[i]
                                              ,&theta
                                              ,&alpha
                                              ,&low_beta
                                              ,&high_beta
                                              ,&gamma
                                              );
    }
}


//--------------------------------------------------------------
void ofxIedk::update(ofEventArgs &event){
    checkStatus();
    if(!bConnected) {
        deviceSize = IEE_GetInsightDeviceCount() + IEE_GetEpocPlusDeviceCount();
    }
    if(!isReady){ return; }
    if(useEmotionEvent) { updateEmotion(); }
    if(useMotionEvent) {}
    if(useMentalEvent) {}
    if(useFacialEvent) {}

    getAverageBandPower();
}

//--------------------------------------------------------------
float ofxIedk::getEmotion(IEE_PerformanceMetricAlgo_enum type){
    
    //return 0. - 1.
    if(IS_PerformanceMetricIsActive(eState, type)){
        switch (type) {
            case PM_EXCITEMENT:
                return IS_PerformanceMetricGetInstantaneousExcitementScore(eState);//short term
                //long term excitement
                //IS_PerformanceMetricGetExcitementLongTermScore(eState);
                break;
                
            case PM_RELAXATION:
                return IS_PerformanceMetricGetRelaxationScore(eState);
                break;
                
            case PM_STRESS:
                return IS_PerformanceMetricGetStressScore(eState);
                break;
                
            case PM_ENGAGEMENT:
                return IS_PerformanceMetricGetEngagementBoredomScore(eState);
                break;
                
            case PM_INTEREST:
                return IS_PerformanceMetricGetInterestScore(eState);
                break;
                
            case PM_FOCUS:
                return IS_PerformanceMetricGetFocusScore(eState);
                break;
            
        }
    }
    return -1;
}

//--------------------------------------------------------------
IEE_EEG_ContactQuality_t ofxIedk::getQuality(IEE_InputChannels_t ch){
    return IS_GetContactQuality(eState, ch);
}
//--------------------------------------------------------------

//--------------------------------------------------------------

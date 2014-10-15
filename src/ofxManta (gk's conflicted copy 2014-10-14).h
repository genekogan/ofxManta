#pragma once

#include "ofMain.h"
#include "Manta.h"


//------
class ofxMantaEvent : public ofEventArgs {
public:
    int row, col, id, value;
    ofxMantaEvent(int row, int col, int id, int value);
};


//------
class ofxManta : public Manta, public ofThread
{
public:
    ofxManta();
    ~ofxManta();
    
    bool setup();
    void close();
    void setAnimated(bool animated);
    bool getAnimated() {return animated;}
    bool getConnected() {return connected;}
    
    void draw(int x, int y, int w = 400);
    
    // get current state
    float getPad(int row, int column) {return pad[row][column];}
    float getSlider(int index) {return slider[index];}
    float getButton(int index) {return button[index];}
    float * getPadRef(int row, int column) {return &pad[row][column];}
    float * getSliderRef(int index) {return &slider[index];}
    float * getButtonRef(int index) {return &button[index];}

    
    
    
    void setLedManual(bool manual) {
        SetLEDControl(PadAndButton, !manual);
        
        //SetLEDControl(Button, !manual);
        SetLEDControl(Slider, !manual);
    }
    
    void setLedState(int row, int column, LEDState state) {
        SetPadLED(state, column + 8 * row);
    }
    
    void setSliderLed(int index, uint8_t value) {
        SetSliderLED(Amber, index, val);
        val *= 2;
    }
    uint8_t val =0;
    
    
    // add event listeners
    template <typename L, typename M>
    void addPadListener(L *listener, M method) {
        ofAddListener(padEvent, listener, method);
    }
    template <typename L, typename M>
    void addSliderListener(L *listener, M method) {
        ofAddListener(sliderEvent, listener, method);
    }
    template <typename L, typename M>
    void addButtonListener(L *listener, M method) {
        ofAddListener(buttonEvent, listener, method);
    }
    template <typename L, typename M>
    void addPadVelocityListener(L *listener, M method) {
        ofAddListener(padVelocityEvent, listener, method);
    }
    template <typename L, typename M>
    void addButtonVelocityListener(L *listener, M method) {
        ofAddListener(buttonVelocityEvent, listener, method);
    }

    // remove event listeners
    template <typename L, typename M>
    void removePadListener(L *listener, M method) {
        ofRemoveListener(padEvent, listener, method);
    }
    template <typename L, typename M>
    void removeSliderListener(L *listener, M method) {
        ofRemoveListener(sliderEvent, listener, method);
    }
    template <typename L, typename M>
    void removeButtonListener(L *listener, M method) {
        ofRemoveListener(buttonEvent, listener, method);
    }
    template <typename L, typename M>
    void removePadVelocityListener(L *listener, M method) {
        ofRemoveListener(padVelocityEvent, listener, method);
    }
    template <typename L, typename M>
    void removeButtonVelocityListener(L *listener, M method) {
        ofRemoveListener(buttonVelocityEvent, listener, method);
    }

protected:
    void redraw();
    void drawPad(int row, int col);
    void drawButtons();
    void drawSliders();

    // update process
    void update(ofEventArgs &data);
    void threadedFunction();
    void sendEventNotifications();

    // get events from Manta
    void PadEvent(int row, int column, int id, int value);
    void SliderEvent(int id, int value);
    void ButtonEvent(int id, int value);
    void PadVelocityEvent(int row, int column, int id, int value);
    void ButtonVelocityEvent(int id, int value);

    // ofEvent notifiers
    ofEvent<ofxMantaEvent> padEvent;
    ofEvent<ofxMantaEvent> sliderEvent;
    ofEvent<ofxMantaEvent> buttonEvent;
    ofEvent<ofxMantaEvent> padVelocityEvent;
    ofEvent<ofxMantaEvent> buttonVelocityEvent;
    
    vector<ofxMantaEvent *> padEvents;
    vector<ofxMantaEvent *> sliderEvents;
    vector<ofxMantaEvent *> buttonEvents;
    vector<ofxMantaEvent *> padVelocityEvents;
    vector<ofxMantaEvent *> buttonVelocityEvents;
    
    float pad[6][8];
    float slider[2];
    float button[4];
    
    bool connected;
    
    int width, height;
    ofFbo fbo;
    bool animated, toRedrawPads, toRedrawSliders, toRedrawButtons;
    bool padsToRedraw[48];
};


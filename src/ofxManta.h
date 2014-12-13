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
    
    // led control
    void setLedManual(bool manual);
    void setPadLedState(int row, int column, LEDState state);
    void setSliderLedState(int index, LEDState state, int value);
    void setButtonLedState(int index, LEDState state);
    
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
    
    int getDrawWidth() {return width;}
    int getDrawHeight() {return height;}
    
    
    
    
    
    
    
    //vector<int> selectedPads, selectedSliders, selectedButtons;
    
    //    void keyPressed(ofKeyEventArgs &e) {
    //        if (e.key == OF_KEY_SHIFT)    shift = true;
    //    }
    //    void keyReleased(ofKeyEventArgs &e) {
    //        if (e.key == OF_KEY_SHIFT)    shift = false;
    //    }
    
    int viewSelection = 0;
    
    void setSelectionView(int selection) {
        if (selection >= numSelectionSets ||
            selection==viewSelection)  return;
        viewSelection = selection;
        for (int i=0; i<48; i++) {
            padsToRedraw[i] = true;
        }
        toRedrawPads = true;
        toRedrawSliders = true;
        redrawComponents();
    }
    
    int numSelectionSets = 4;
    
    void clearSelection() {
        for (int s=0; s<numSelectionSets; s++) {
            for (int i=0; i<48; i++) {
                padSelection[s][i] = false;
                padsToRedraw[i] = true;
            }
            for (int i=0; i<2; i++) {
                sliderSelection[s][i] = false;
            }
            for (int i=0; i<4; i++) {
                buttonSelection[s][i] = false;
            }
        }
        toRedrawPads = true;
        toRedrawSliders = true;
        toRedrawButtons = true;
    }
    
    void addSliderToSelection(int idx, int selection=0) {
        if (selection >= numSelectionSets)  return;
        //selectedSliders.push_back(idx);
        sliderSelection[selection][idx] = true;
        
        toRedrawSliders = true;
    }
    void addPadToSelection(int idx, int selection=0) {
        if (selection >= numSelectionSets)  return;
        padSelection[selection][idx] = true;
        padsToRedraw[idx] = true;
        toRedrawPads = true;
//        redrawComponents();
        //redraw();
        //drawPad(floor(idx/8), idx%8);
    }
    void addButtonToSelection(int idx, int selection=0) {
        if (selection >= numSelectionSets)  return;
        buttonSelection[selection][idx] = true;
        toRedrawButtons = true;
    }
    
    map<int, bool> padSelection[4];
    map<int, bool> sliderSelection[4];
    map<int, bool> buttonSelection[4];
    
    vector<int> getPadSelection(int selection) {
        vector<int> padsSelected;
        for (int i=0; i<48; i++) {
            if (padSelection[selection][i]) {
                padsSelected.push_back(i);
            }
        }
        return padsSelected;
    }
    vector<int> getSliderSelection(int selection) {
        vector<int> slidersSelected;
        for (int i=0; i<2; i++) {
            if (sliderSelection[selection][i]) {
                slidersSelected.push_back(i);
            }
        }
        return slidersSelected;
    }
    vector<int> getButtonSelection(int selection) {
        vector<int> buttonsSelected;
        for (int i=0; i<4; i++) {
            if (buttonSelection[selection][i]) {
                buttonsSelected.push_back(i);
            }
        }
        return buttonsSelected;
    }
    
    
    
protected:
    
    ofColor ledColors[2];
    ofColor selectionColors[4];
    
    // draw manta interface
    void redraw();
    void redrawComponents();
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
    
    LEDState padLedState[6][8];
    LEDState sliderLedState[2];
    LEDState buttonLedState[4];

    float pad[6][8];
    float slider[2];
    float button[4];
    
    bool connected;
    
    int width, height;
    ofFbo fbo;
    bool animated, toRedrawPads, toRedrawSliders, toRedrawButtons;
    bool padsToRedraw[48];
    
    
    
    
    
    
};


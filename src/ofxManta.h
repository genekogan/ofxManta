#pragma once

#include "ofMain.h"
#include "Manta.h"


#define MANTA_MAX_PAD_VALUE 196
#define MANTA_MAX_SLIDER_VALUE 4096
#define MANTA_MAX_BUTTON_VALUE 255


class ofxMantaEvent : public ofEventArgs
{
public:
    int row, col, id, value;
    ofxMantaEvent(int row, int col, int id, int value);
};


class ofxManta : public Manta, public ofThread
{
public:
    int nTimesDraw;
    
    
    
    ofxManta();
    virtual ~ofxManta();
    
    virtual bool setup();
    void close();
    void setAnimated(bool animated);
    bool getAnimated() {return animated;}
    
    virtual void update();
    virtual void draw(int x, int y, int w = 400);

    // get current state
    ofParameter<float> & getPad(int row, int column) {return pad[row][column];}
    ofParameter<float> & getSlider(int index) {return slider[index];}
    ofParameter<float> & getButton(int index) {return button[index];}
    int getSizeSelection();
    
    // led control
    void setLedManual(bool manual);
    void setPadLedState(int row, int column, LEDState state);
    void setSliderLedState(int index, LEDState state, int value);
    void setButtonLedState(int index, LEDState state);
    void markAllPads(LEDState state);
    void markAllSliders(LEDState state, int value);
    void markAllButtons(LEDState state);
    
    // color control
    void setPadColor(int row, int column, ofColor color);
    void setSliderColor(int index, ofColor color);
    void setButtonColor(int index, ofColor color);
    
    // drawing
    int getDrawWidth() {return width;}
    int getDrawHeight() {return height;}

    // add event listeners
    template <typename L, typename M> void addPadListener(L *listener, M method) {ofAddListener(eventTypeLookup[PAD], listener, method);}
    template <typename L, typename M> void addSliderListener(L *listener, M method) {ofAddListener(eventTypeLookup[SLIDER], listener, method);}
    template <typename L, typename M> void addButtonListener(L *listener, M method) {ofAddListener(eventTypeLookup[BUTTON], listener, method);}
    template <typename L, typename M> void addPadVelocityListener(L *listener, M method) {ofAddListener(eventTypeLookup[PAD_VELOCITY], listener, method);}
    template <typename L, typename M> void addButtonVelocityListener(L *listener, M method) {ofAddListener(eventTypeLookup[BUTTON_VELOCITY], listener, method);}
    
    // remove event listeners
    template <typename L, typename M> void removePadListener(L *listener, M method) {ofRemoveListener(eventTypeLookup[PAD], listener, method);}
    template <typename L, typename M> void removeSliderListener(L *listener, M method) {ofRemoveListener(eventTypeLookup[SLIDER], listener, method);}
    template <typename L, typename M> void removeButtonListener(L *listener, M method) {ofRemoveListener(eventTypeLookup[BUTTON], listener, method);}
    template <typename L, typename M> void removePadVelocityListener(L *listener, M method) {ofRemoveListener(eventTypeLookup[PAD_VELOCITY], listener, method);}
    template <typename L, typename M> void removeButtonVelocityListener(L *listener, M method) {ofRemoveListener(eventTypeLookup[BUTTON_VELOCITY], listener, method);}
    
protected:

    enum MantaEventType {
        PAD, SLIDER, BUTTON, PAD_VELOCITY, BUTTON_VELOCITY
    };

    struct MantaEvent {
        ofxMantaEvent *event;
        MantaEventType type;
        MantaEvent(MantaEventType type, ofxMantaEvent *event) {
            this->type = type;
            this->event = event;
        }
    };
    
    // draw manta interface
    void redraw();
    void redrawComponents();
    void drawPad(int row, int col);
    void drawButtons();
    void drawSliders();
    
    // update process
    void update(ofEventArgs &data) {update();}
    void threadedFunction();
    void sendEventNotifications();
    
    // freezing
    void setPadFreezeEnabled(bool toFreezePads);
    void enablePadToggle(int row, int column, bool toggle);
    void freezePads();
    void clearPadStates();

    // get events from Manta
    void PadEvent(int row, int column, int id, int value);
    void SliderEvent(int id, int value);
    void ButtonEvent(int id, int value);
    void PadVelocityEvent(int row, int column, int id, int value);
    void ButtonVelocityEvent(int id, int value);

    // ofEvent notifiers
    map<MantaEventType, ofEvent<ofxMantaEvent> > eventTypeLookup;
    vector<MantaEvent*> events;
    
    ofEvent<ofxMantaEvent> padEvent;
    ofEvent<ofxMantaEvent> sliderEvent;
    ofEvent<ofxMantaEvent> buttonEvent;
    ofEvent<ofxMantaEvent> padVelocityEvent;
    ofEvent<ofxMantaEvent> buttonVelocityEvent;

    // selection
    void addPadToSelection(int row, int col);
    void addSliderToSelection(int idx);
    void addButtonToSelection(int idx);
    void removePadFromSelection(int row, int col);
    void removeSliderFromSelection(int idx);
    void removeButtonFromSelection(int idx);
    void clearPadSelection();
    void clearSliderSelection();
    void clearButtonSelection();
    void clearSelection();
    bool getPadSelected(int idx);
    bool getSliderSelected(int idx);
    bool getButtonSelected(int idx);
    vector<int> getPadSelection() {return getSelection(padSelection);}
    vector<int> getSliderSelection() {return getSelection(sliderSelection);}
    vector<int> getButtonSelection() {return getSelection(buttonSelection);}

    // LED + Colors
    ofColor ledColors[2];
    LEDState padLedState[6][8];
    LEDState sliderLedState[2];
    LEDState buttonLedState[4];
    ofColor padColor[6][8];
    ofColor sliderColor[2];
    ofColor buttonColor[4];
    
    // selection
    vector<int> getSelection(map<int, bool> & selected);
    map<int, bool> padSelection;
    map<int, bool> sliderSelection;
    map<int, bool> buttonSelection;

    // internal
    ofParameter<float> pad[6][8];
    ofParameter<float> slider[2];
    ofParameter<float> button[4];

    bool padFrozen[48];
    bool padReleased[48];
    bool padToggle[48];
    bool toFreezePads;

    int width, height;
    ofFbo fbo;
    bool animated, toRedrawPads, toRedrawSliders, toRedrawButtons;
    bool padsToRedraw[48];
    float padMult, sliderMult, buttonMult;
    float lastReleasedTime;
};


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
    
    // led control
    void setLedManual(bool manual);
    void setPadLedState(int row, int column, LEDState state);
    void setSliderLedState(int index, LEDState state, int value);
    void setButtonLedState(int index, LEDState state);
    void markAllPads(LEDState state);
    void markAllSliders(LEDState state, int value);
    void markAllButtons(LEDState state);
    
    // selection
    void setSelectionView(int selection);
    void setSelectionView(bool drawAllSelectionLayers) {this->drawAllSelectionLayers = drawAllSelectionLayers;}
    void addPadToSelection(int row, int col, int selection=0);
    void addSliderToSelection(int idx, int selection=0);
    void addButtonToSelection(int idx, int selection=0);
    void removePadFromSelection(int row, int col, int selection=0);
    void removeSliderFromSelection(int idx, int selection=0);
    void removeButtonFromSelection(int idx, int selection=0);
    void clearPadSelection(int selection);
    void clearSliderSelection(int selection);
    void clearButtonSelection(int selection);
    void clearSelection(int selection);
    void clearPadSelection();
    void clearSliderSelection();
    void clearButtonSelection();
    void clearSelection();

    bool getPadSelected(int idx, int selection=0);
    bool getSliderSelected(int idx, int selection=0);
    bool getButtonSelected(int idx, int selection=0);
    vector<int> getPadSelection(int selection=0) {return getSelection(padSelection, selection);}
    vector<int> getSliderSelection(int selection=0) {return getSelection(sliderSelection, selection);}
    vector<int> getButtonSelection(int selection=0) {return getSelection(buttonSelection, selection);}

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

    // selection
    vector<int> getSelection(map<int, bool> selected[4], int selection);
    
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

    // LED + Colors
    ofColor ledColors[2];
    ofColor selectionColors[4];

    LEDState padLedState[6][8];
    LEDState sliderLedState[2];
    LEDState buttonLedState[4];
    
    // selection
    map<int, bool> padSelection[4];
    map<int, bool> sliderSelection[4];
    map<int, bool> buttonSelection[4];

    int numSelectionSets;
    int viewSelection;

    // internal
    ofParameter<float> pad[6][8];
    ofParameter<float> slider[2];
    ofParameter<float> button[4];

    int width, height;
    ofFbo fbo;
    bool animated, toRedrawPads, toRedrawSliders, toRedrawButtons;
    bool padsToRedraw[48];
    bool drawAllSelectionLayers;
    
    float padMult, sliderMult, buttonMult;
};


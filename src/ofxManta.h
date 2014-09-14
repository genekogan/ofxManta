#pragma once

#include "ofMain.h"
#include "Manta.h"


class ofxMantaEvent : public ofEventArgs {
public:
    int row, col, id, value;
    ofxMantaEvent(int row, int col, int id, int value);
};

class ofxManta : public Manta
{
public:
    ~ofxManta();
    void setup();
    void draw(int x, int y, int w = 400);
    void close();
    
    float getPad(int row, int column);
    float getSlider(int index);
    float getButton(int index);

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
    
private:

    void update(ofEventArgs &data);

    void PadEvent(int row, int column, int id, int value);
    void SliderEvent(int id, int value);
    void ButtonEvent(int id, int value);
    void PadVelocityEvent(int row, int column, int id, int value);
    void ButtonVelocityEvent(int id, int value);

    ofEvent<ofxMantaEvent> padEvent;
    ofEvent<ofxMantaEvent> sliderEvent;
    ofEvent<ofxMantaEvent> buttonEvent;
    ofEvent<ofxMantaEvent> padVelocityEvent;
    ofEvent<ofxMantaEvent> buttonVelocityEvent;

    float pad[6][8];
    float slider[2];
    float button[4];
};


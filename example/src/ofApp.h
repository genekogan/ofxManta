#pragma once

#include "ofMain.h"
#include "ofxManta.h"



class ofApp : public ofBaseApp {

public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void PadEvent(ofxMantaEvent & evt);
    void SliderEvent(ofxMantaEvent & evt);
    void ButtonEvent(ofxMantaEvent & evt);
    void PadVelocityEvent(ofxMantaEvent & evt);
    void ButtonVelocityEvent(ofxMantaEvent & evt);

    ofxManta manta;
    
    void keyPressed(int key) {
        if (key==' ') {
            bool con = manta.setup();
            cout << "set up " << con << endl;
        }
    }
};


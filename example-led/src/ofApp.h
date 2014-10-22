#pragma once

#include "ofMain.h"
#include "ofxManta.h"


class ofApp : public ofBaseApp {

public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);

    ofxManta manta;
    
    int sliderValue;
};


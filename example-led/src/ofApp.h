#pragma once

#include "ofMain.h"
#include "ofxManta.h"


class ofApp : public ofBaseApp {

public:
    void setup();
    void update();
    void draw();
    void exit();

    ofxManta manta;
    
    int column;
};


#include "ofApp.h"

//----------
void ofApp::setup(){
    
    // connect to manta
    manta.setup();
    
}

//----------
void ofApp::update(){

    if (ofGetFrameNum() % 60 == 0) {

        // set led to be controllable only programmatically
        manta.setLedManual(false);
        
        // clear previous column
        manta.setLedState(0, column, Manta::Off);
        manta.setLedState(1, column, Manta::Off);
        manta.setLedState(2, column, Manta::Off);
        manta.setLedState(3, column, Manta::Off);
        manta.setLedState(4, column, Manta::Off);
        manta.setLedState(5, column, Manta::Off);

        // next column
        column = (column + 1) % 8;
        
        // make column red
        manta.setLedState(0, column, Manta::Red);
        manta.setLedState(1, column, Manta::Red);
        manta.setLedState(2, column, Manta::Red);
        manta.setLedState(3, column, Manta::Red);
        manta.setLedState(4, column, Manta::Red);
        manta.setLedState(5, column, Manta::Red);

        
        manta.setSliderLed(0, column);

        
        // permit user to control led's by hand
        manta.setLedManual(true);
        
        
        
    }
}

//----------
void ofApp::draw(){
    manta.draw(20, 20, 800);
}

//----------
void ofApp::exit() {
    manta.close();
}

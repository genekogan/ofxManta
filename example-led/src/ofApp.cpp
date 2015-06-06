#include "ofApp.h"

//----------
void ofApp::setup(){
    
    // connect to manta
    manta.setup();
    
    // set manual input
    // manual makes the LEDs unresponsive to your fingers.
    // setting it back to non-manual (finger) mode will erase
    // all amber LEDs but keep red ones
    manta.setLedManual(true);
    
    // set pads
    manta.setPadLedState(0, 1, Manta::Red);
    manta.setPadLedState(0, 2, Manta::Red);
    manta.setPadLedState(1, 3, Manta::Amber);
    manta.setPadLedState(1, 4, Manta::Amber);

    // set buttons
    manta.setButtonLedState(1, Manta::Amber);
    manta.setButtonLedState(3, Manta::Red);
}

//----------
void ofApp::update(){
    if (ofGetFrameNum() % 60 == 0) {
        sliderValue = (sliderValue + 1) % 8;
        manta.setSliderLedState(0, Manta::Amber, sliderValue);
    }
}

//----------
void ofApp::keyPressed(int key) {
    
}

//----------
void ofApp::draw(){
    manta.draw(20, 20, 800);
}

//----------
void ofApp::exit() {
    
}

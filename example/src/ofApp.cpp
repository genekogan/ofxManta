#include "ofApp.h"

//----------
void ofApp::setup(){
    // connect to manta
    manta.setup();
    
    // set up listeners
    manta.addPadListener(this, &ofApp::PadEvent);
    manta.addSliderListener(this, &ofApp::SliderEvent);
    manta.addButtonListener(this, &ofApp::ButtonEvent);
    manta.addPadVelocityListener(this, &ofApp::PadVelocityEvent);
    manta.addButtonVelocityListener(this, &ofApp::ButtonVelocityEvent);
    
    // optionally, you can turn animation off to stop redrawing manta interface
    //manta.setAnimated(false);
}

//----------
void ofApp::update(){
    float padValue = manta.getPad(0, 0);      // get value of top left pad
    float sliderValue = manta.getSlider(0);   // get value of first slider
    float buttonValue = manta.getButton(0);   // get value of first button
}

//----------
void ofApp::draw(){
    manta.draw(20, 20, 800);
}

//----------
void ofApp::exit() {
    manta.removePadListener(this, &ofApp::PadEvent);
    manta.removeSliderListener(this, &ofApp::SliderEvent);
    manta.removeButtonListener(this, &ofApp::ButtonEvent);
    manta.removePadVelocityListener(this, &ofApp::PadVelocityEvent);
    manta.removeButtonVelocityListener(this, &ofApp::ButtonVelocityEvent);

    manta.close();
}

//----------
void ofApp::PadEvent(ofxMantaEvent & evt) {
    cout << "Pad event: id " << evt.id << ", row "<< evt.row <<", column "<< evt.col << ", value "<< evt.value << endl;
}

//----------
void ofApp::SliderEvent(ofxMantaEvent & evt) {
    cout << "Slider event: id " << evt.id << ", value "<< evt.value << endl;
}

//----------
void ofApp::ButtonEvent(ofxMantaEvent & evt) {
    cout << "Button event: id " << evt.id << ", value "<< evt.value << endl;
}

//----------
void ofApp::PadVelocityEvent(ofxMantaEvent & evt) {
    cout << "Pad velocity event: id " << evt.id << ", row "<< evt.row <<", column "<< evt.col << ", value "<< evt.value << endl;
}

//----------
void ofApp::ButtonVelocityEvent(ofxMantaEvent & evt) {
    cout << "Button velocity event: id " << evt.id << ", value "<< evt.value << endl;
}

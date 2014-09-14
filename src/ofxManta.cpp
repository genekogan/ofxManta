#include "ofxManta.h"

//--------
ofxMantaEvent::ofxMantaEvent(int row, int col, int id, int value) {
    this->row = row;
    this->col = col;
    this->id = id;
    this->value = value;
}

//--------
void ofxManta::setup() {
    Connect();
    ofAddListener(ofEvents().update, this, &ofxManta::update);
}

//--------
void ofxManta::close() {
    ofRemoveListener(ofEvents().update, this, &ofxManta::update);
    Disconnect();
}

//--------
void ofxManta::update(ofEventArgs &data) {
    HandleEvents();
}

//--------
float ofxManta::getPad(int row, int column) {
    return pad[row][column];
}

//--------
float ofxManta::getSlider(int index) {
    return slider[index];
}

//--------
float ofxManta::getButton(int index) {
    return button[index];
}

//--------
void ofxManta::draw(int x, int y, int w) {
    int h = w * 310.0 / 400.0;
    
    ofPushStyle();
    ofPushMatrix();
    
    ofTranslate(x, y);
    ofSetColor(0);
    ofRect(0, 0, w, h);

    // draw sliders (outline)
    ofPushMatrix();
    ofSetColor(255);
    ofTranslate(0.08*w, 0.05*h);
    ofRect(0, 0, 0.65*w, 0.05*h);
    ofTranslate(-0.03*w, 0.08*h);
    ofRect(0, 0, 0.65*w, 0.05*h);
    ofPopMatrix();

    // draw sliders (value)
    ofPushMatrix();
    ofSetColor(0, 255, 0);
    ofTranslate(0.08*w, 0.05*h);
    ofRect(0, 0, 0.65*w*slider[0], 0.05*h);
    ofTranslate(-0.03*w, 0.08*h);
    ofRect(0, 0, 0.65*w*slider[1], 0.05*h);
    ofPopMatrix();

    // draw buttons (outline)
    ofSetCircleResolution(32);
    ofPushMatrix();
    ofSetColor(255);
    ofTranslate(0, 0.025*h);
    ofTranslate(0.8*w, 0.05*h);
    ofCircle(0, 0, 0.02*w);
    ofTranslate(0.1*w, 0);
    ofCircle(0, 0, 0.02*w);
    ofTranslate(-0.05*w, 0.08*h);
    ofCircle(0, 0, 0.02*w);
    ofTranslate(0.1*w, 0);
    ofCircle(0, 0, 0.02*w);
    ofPopMatrix();

    // draw buttons (value)
    ofSetCircleResolution(32);
    ofPushMatrix();
    ofSetColor(0, 255, 0);
    ofTranslate(0, 0.025*h);
    ofTranslate(0.8*w, 0.05*h);
    ofCircle(0, 0, 0.02*w*(button[0]>0.0));
    ofTranslate(0.1*w, 0);
    ofCircle(0, 0, 0.02*w*(button[1]>0.0));
    ofTranslate(-0.05*w, 0.08*h);
    ofCircle(0, 0, 0.02*w*(button[2]>0.0));
    ofTranslate(0.1*w, 0);
    ofCircle(0, 0, 0.02*w*(button[3]>0.0));
    ofPopMatrix();
    
    // draw pads
    ofSetCircleResolution(6);
    for (int r=0; r<6; r++) {
        for (int c=0; c<8; c++) {
            ofPushMatrix();
            ofTranslate(ofMap(c+0.5, 0, 8, 0.01*w, 0.94*w),
                        ofMap(r+0.5, 6, 0, 0.24*h, 0.97*h));
            if (r%2!=0) ofTranslate(0.93*w/16.0, 0);
            ofRotate(90);
            
            ofSetColor(255);
            ofCircle(0, 0, w/20.0);
            ofSetColor(0, 255, 0);
            ofCircle(0, 0, w*pad[r][c]/(20.0*203.0));

            ofPopMatrix();
        }
    }
    
    ofPopMatrix();
    ofPopStyle();
}

//--------
void ofxManta::PadEvent(int row, int column, int id, int value) {
    //cout << "Pad event: " << ofGetElapsedTimeMicros() << ", id " << id << ", row "<< row <<", column "<< column << ", value "<< value << endl;
    ofxMantaEvent padEventArgs(row, column, id, value);
    ofNotifyEvent(padEvent, padEventArgs, this);
    pad[row][column] = value;
}

//--------
void ofxManta::SliderEvent(int id, int value) {
    if (value == 65535) value = -1;
    ofxMantaEvent padEventArgs(NULL, NULL, id, value);
    ofNotifyEvent(sliderEvent, padEventArgs, this);
    if (value == -1) return;
    slider[id] = ofClamp(value/4090.0, 0.0, 1.0);
}

//--------
void ofxManta::ButtonEvent(int id, int value) {
    ofxMantaEvent padEventArgs(NULL, NULL, id, value);
    ofNotifyEvent(buttonEvent, padEventArgs, this);
    button[id] = ofClamp(value / 254.0, 0.0, 1.0);
}

//--------
void ofxManta::PadVelocityEvent(int row, int column, int id, int value) {
    ofxMantaEvent padEventArgs(row, column, id, value);
    ofNotifyEvent(padVelocityEvent, padEventArgs, this);
}

//--------
void ofxManta::ButtonVelocityEvent(int id, int value) {
    ofxMantaEvent padEventArgs(NULL, NULL, id, value);
    ofNotifyEvent(buttonVelocityEvent, padEventArgs, this);
}

//--------
ofxManta::~ofxManta() {
    this->close();
}
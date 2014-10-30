#include "ofxManta.h"

//--------
ofxMantaEvent::ofxMantaEvent(int row, int col, int id, int value) {
    this->row = row;
    this->col = col;
    this->id = id;
    this->value = value;
}

//--------
ofxManta::ofxManta() {
    width = 100;
    height = width * 310.0 / 400.0;
    redraw();
    animated = true;
}

//--------
bool ofxManta::setup() {
    try {
        Connect();
        setLedManual(false);
        ofAddListener(ofEvents().update, this, &ofxManta::update);
        startThread();
        connected = true;
    }
    catch(runtime_error &e) {
        ofLog(OF_LOG_ERROR, ofToString(e.what()));
        connected = false;
    }
    return connected;
}

//--------
void ofxManta::setAnimated(bool animated) {
    this->animated = animated;
    if (!animated) {
        redraw();
    }
}

//--------
void ofxManta::close() {
    if (connected) {
        stopThread();
        ofRemoveListener(ofEvents().update, this, &ofxManta::update);
        Disconnect();
        connected = false;
    }
}

//--------
void ofxManta::update(ofEventArgs &data) {
    sendEventNotifications();
    if (animated) {
        redrawComponents();
    }
}

//--------
void ofxManta::draw(int x, int y, int w) {
    if (w != width) {
        width = w;
        height = width * 310.0 / 400.0;
        fbo.allocate(width, height);
        redraw();
    }
    fbo.draw(x, y);
}

//--------
void ofxManta::redraw() {
    fbo.begin();
    
    ofPushStyle();
    ofPushMatrix();
    
    ofSetColor(0);
    ofRect(0, 0, width, height);

    // draw sliders
    drawSliders();
    
    // draw buttons
    drawButtons();
    
    // draw pads
    for (int r=0; r<6; r++) {
        for (int c=0; c<8; c++) {
            drawPad(r, c);
        }
    }
    
    if (!connected) {
        ofSetColor(255, 0, 0);
        ofDrawBitmapString("Manta is not connected", 6, 11);
    }

    ofPopMatrix();
    ofPopStyle();
    
    fbo.end();
    
    toRedrawButtons = false;
    toRedrawPads = false;
    toRedrawSliders = false;
}

//--------
void ofxManta::redrawComponents() {
    fbo.begin();
    
    ofPushMatrix();
    ofPushStyle();
    
    if (toRedrawPads) {
        for (int idx=0; idx<48; idx++) {
            if (padsToRedraw[idx]) {
                drawPad(floor(idx/8), idx%8);
                padsToRedraw[idx] = false;
            }
        }
        toRedrawPads = false;
    }
    if (toRedrawButtons) {
        drawButtons();
        toRedrawButtons = false;
    }
    if (toRedrawSliders) {
        drawSliders();
        toRedrawSliders = false;
    }

    ofPopStyle();
    ofPopMatrix();

    fbo.end();
}

//--------
void ofxManta::drawPad(int row, int col) {
    ofPushMatrix();
    
    ofTranslate(ofMap(col + 0.5, 0, 8, 0.01 * width,  0.94 * width),
                ofMap(row + 0.5, 6, 0, 0.24 * height, 0.97 * height));
    if (row % 2 != 0)  ofTranslate(0.93 * width / 16.0, 0);
    ofRotate(90);

    ofSetCircleResolution(6);
    ofSetLineWidth(4);

    if (padLedState[row][col] == Off) {
        ofSetColor(0);
        ofNoFill();
        ofCircle(0, 0, width / 20.0);
    }

    ofSetColor(255);
    ofFill();
    ofCircle(0, 0, width / 20.0);
    
    if (animated) {
        ofSetColor(0, 255, 0);
        ofCircle(0, 0, width * pad[row][col] / (20.0*203.0));
    }
    
    if (padLedState[row][col] == Amber) {
        ofSetColor(255, 255, 0);
        ofNoFill();
        ofCircle(0, 0, width / 20.0);
    }
    else if (padLedState[row][col] == Red) {
        ofSetColor(255, 0, 0);
        ofNoFill();
        ofCircle(0, 0, width / 20.0);
    }
    
    ofPopMatrix();
}

//--------
void ofxManta::drawButtons() {
    ofPushMatrix();
    ofPushStyle();
    
    // draw buttons (outline)
    ofSetCircleResolution(32);
    ofPushMatrix();
    ofSetColor(255);
    ofTranslate(0, 0.025*height);
    ofTranslate(0.8*width, 0.05*height);
    ofCircle(0, 0, 0.02*width);
    ofTranslate(0.1*width, 0);
    ofCircle(0, 0, 0.02*width);
    ofTranslate(-0.05*width, 0.08*height);
    ofCircle(0, 0, 0.02*width);
    ofTranslate(0.1*width, 0);
    ofCircle(0, 0, 0.02*width);
    ofPopMatrix();
    
    // draw button values
    ofPushMatrix();

    ofNoFill();
    ofSetLineWidth(3);
    
    ofTranslate(0, 0.025*height);
    
    ofTranslate(0.8*width, 0.05*height);
    if (buttonLedState[0] == Red) {
        ofSetColor(255, 0, 0);
        ofCircle(0, 0, 0.02*width);
    }
    else if (buttonLedState[0] == Amber) {
        ofSetColor(255, 255, 0);
        ofCircle(0, 0, 0.02*width);
    }
    
    ofTranslate(0.1*width, 0);
    if (buttonLedState[1] == Red) {
        ofSetColor(255, 0, 0);
        ofCircle(0, 0, 0.02*width);
    }
    else if (buttonLedState[1] == Amber) {
        ofSetColor(255, 255, 0);
        ofCircle(0, 0, 0.02*width);
    }
    
    ofTranslate(-0.05*width, 0.08*height);
    if (buttonLedState[2] == Red) {
        ofSetColor(255, 0, 0);
        ofCircle(0, 0, 0.02*width);
    }
    else if (buttonLedState[2] == Amber) {
        ofSetColor(255, 255, 0);
        ofCircle(0, 0, 0.02*width);
    }
    
    ofTranslate(0.1*width, 0);
    if (buttonLedState[3] == Red) {
        ofSetColor(255, 0, 0);
        ofCircle(0, 0, 0.02*width);
    }
    else if (buttonLedState[3] == Amber) {
        ofSetColor(255, 255, 0);
        ofCircle(0, 0, 0.02*width);
    }
    
    ofFill();
    ofPopMatrix();
    
    // draw buttons (value)
    if (animated) {
        ofSetCircleResolution(32);
        ofSetColor(0, 255, 0);
        ofTranslate(0, 0.025*height);
        ofTranslate(0.8*width, 0.05*height);
        ofCircle(0, 0, 0.02*width*(button[0]>0.0));
        ofTranslate(0.1*width, 0);
        ofCircle(0, 0, 0.02*width*(button[1]>0.0));
        ofTranslate(-0.05*width, 0.08*height);
        ofCircle(0, 0, 0.02*width*(button[2]>0.0));
        ofTranslate(0.1*width, 0);
        ofCircle(0, 0, 0.02*width*(button[3]>0.0));
    }
    
    ofPopStyle();
    ofPopMatrix();
}

//--------
void ofxManta::drawSliders() {
    ofPushStyle();
    
    // draw sliders (outline)
    ofPushMatrix();
    ofSetColor(255);
    ofTranslate(0.08*width, 0.05*height);
    ofRect(0, 0, 0.65*width, 0.05*height);
    ofTranslate(-0.03*width, 0.08*height);
    ofRect(0, 0, 0.65*width, 0.05*height);
    ofPopMatrix();
    
    
    ofPushMatrix();
    ofNoFill();
    ofSetLineWidth(3);
    ofTranslate(0.08*width, 0.05*height);
    if (sliderLedState[0] == Red) {
        ofSetColor(255, 0, 0);
        ofRect(0, 0, 0.65*width, 0.05*height);
    }
    else if (sliderLedState[0] == Amber) {
        ofSetColor(255, 255, 0);
        ofRect(0, 0, 0.65*width, 0.05*height);
    }
    ofTranslate(-0.03*width, 0.08*height);
    if (sliderLedState[1] == Red) {
        ofSetColor(255, 0, 0);
        ofRect(0, 0, 0.65*width, 0.05*height);
    }
    else if (sliderLedState[1] == Amber) {
        ofSetColor(255, 255, 0);
        ofRect(0, 0, 0.65*width, 0.05*height);
    }
    ofFill();
    ofPopMatrix();
    
    // draw sliders (value)
    if (animated) {
        ofPushMatrix();
        ofSetColor(0, 255, 0);
        ofTranslate(0.08*width, 0.05*height);
        ofRect(0, 0, 0.65*width*slider[0], 0.05*height);
        ofTranslate(-0.03*width, 0.08*height);
        ofRect(0, 0, 0.65*width*slider[1], 0.05*height);
        ofPopMatrix();
    }
    
    ofPopStyle();
}

//--------
void ofxManta::threadedFunction() {
    while(isThreadRunning()) {
        if(lock()) {
            HandleEvents();
            unlock();
        }
        else {
            ofLogWarning("threadedFunction()") << "Unable to lock mutex.";
        }
    }
}

//--------
void ofxManta::sendEventNotifications() {
    if (!connected) return;
    if(lock()) {
        for (int i=0; i<padEvents.size(); i++) {
            ofNotifyEvent(padEvent, *padEvents[i], this);
            delete padEvents[i];
        }
        for (int i=0; i<sliderEvents.size(); i++) {
            ofNotifyEvent(sliderEvent, *sliderEvents[i], this);
            delete sliderEvents[i];
        }
        for (int i=0; i<buttonEvents.size(); i++) {
            ofNotifyEvent(buttonEvent, *buttonEvents[i], this);
            delete buttonEvents[i];
        }
        for (int i=0; i<padVelocityEvents.size(); i++) {
            ofNotifyEvent(padVelocityEvent, *padVelocityEvents[i], this);
            delete padVelocityEvents[i];
        }
        for (int i=0; i<buttonVelocityEvents.size(); i++) {
            ofNotifyEvent(buttonVelocityEvent, *buttonVelocityEvents[i], this);
            delete buttonVelocityEvents[i];
        }
        
        padEvents.clear();
        sliderEvents.clear();
        buttonEvents.clear();
        padVelocityEvents.clear();
        buttonVelocityEvents.clear();
        
        unlock();
    }
    else {
        ofLogWarning("threadedFunction()") << "Unable to lock mutex.";
    }
}

//--------
void ofxManta::setLedManual(bool manual) {
    SetLEDControl(PadAndButton, manual);
    SetLEDControl(Slider, manual);
}

//--------
void ofxManta::setPadLedState(int row, int column, LEDState state) {
    SetPadLED(state, column + 8 * row);
    padLedState[row][column] = state;
    padsToRedraw[column+8*row] = true;
    toRedrawPads = true;
}

//--------
void ofxManta::setSliderLedState(int index, LEDState state, int value) {
    SetSliderLED(Off, index, 255);
    uint8_t mask =  (1 << (7-value));
    SetSliderLED(state, index, mask);
    sliderLedState[index] = state;
    toRedrawSliders = true;
}

//--------
void ofxManta::setButtonLedState(int index, LEDState state) {
    SetButtonLED(state, index);
    buttonLedState[index] = state;
    toRedrawButtons = true;
}

//--------
void ofxManta::PadEvent(int row, int column, int id, int value) {
    ofxMantaEvent *padEventArgs = new ofxMantaEvent(row, column, id, value);
    padEvents.push_back(padEventArgs);
    padsToRedraw[column+8*row] = (value != pad[row][column]);
    pad[row][column] = value;
    toRedrawPads = animated;
}

//--------
void ofxManta::SliderEvent(int id, int value) {
    if (value == 65535) value = -1;
    ofxMantaEvent *sliderEventArgs = new ofxMantaEvent(NULL, NULL, id, value);
    sliderEvents.push_back(sliderEventArgs);
    toRedrawSliders = animated;
    if (value == -1) return;
    slider[id] = ofClamp(value/4090.0, 0.0, 1.0);
}

//--------
void ofxManta::ButtonEvent(int id, int value) {
    ofxMantaEvent *buttonEventArgs = new ofxMantaEvent(NULL, NULL, id, value);
    buttonEvents.push_back(buttonEventArgs);
    button[id] = ofClamp(value / 254.0, 0.0, 1.0);
    toRedrawButtons = animated;
}

//--------
void ofxManta::PadVelocityEvent(int row, int column, int id, int value) {
    ofxMantaEvent *padVelocityEventArgs = new ofxMantaEvent(row, column, id, value);
    padVelocityEvents.push_back(padVelocityEventArgs);
}

//--------
void ofxManta::ButtonVelocityEvent(int id, int value) {
    ofxMantaEvent *buttonVelocityEventArgs = new ofxMantaEvent(NULL, NULL, id, value);
    buttonVelocityEvents.push_back(buttonVelocityEventArgs);
}

//--------
ofxManta::~ofxManta() {
    this->close();
}
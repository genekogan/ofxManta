#include "ofxManta.h"

ofxMantaEvent::ofxMantaEvent(int row, int col, int id, int value)
{
    this->row = row;
    this->col = col;
    this->id = id;
    this->value = value;
}

ofxManta::ofxManta() : Manta()
{
    for (int r=0; r<6; r++) {
        for (int c=0; c<8; c++) {
            pad[r][c].set("pad("+ofToString(r+1)+","+ofToString(c+1)+")", 0, 0, 1);
            padColor[r][c] = ofColor::white;
        }
    }
    for (int i=0; i<2; i++) {
        slider[i].set("slider("+ofToString(i+1)+")", 0, 0, 1);
        sliderColor[i] = ofColor::white;
    }
    for (int i=0; i<4; i++) {
        button[i].set("button("+ofToString(i+1)+")", 0, 0, 1);
        buttonColor[i] = ofColor::white;
    }
    
    eventTypeLookup[PAD] = padEvent;
    eventTypeLookup[SLIDER] = sliderEvent;
    eventTypeLookup[BUTTON] = buttonEvent;
    eventTypeLookup[PAD_VELOCITY] = padVelocityEvent;
    eventTypeLookup[BUTTON_VELOCITY] = buttonVelocityEvent;
    
    padMult = 1.0 / MANTA_MAX_PAD_VALUE;
    sliderMult = 1.0 / MANTA_MAX_SLIDER_VALUE;
    buttonMult = 1.0 / MANTA_MAX_BUTTON_VALUE;

    width = 100;
    height = width * 310.0 / 400.0;
    redraw();
    animated = true;
    lastReleasedTime = ofGetElapsedTimef();
    
    ledColors[0] = ofColor(255, 255, 0); // amber
    ledColors[1] = ofColor(255, 0, 0);   // red

    fbo.begin();
    ofClear(0, 0);
    fbo.end();
    
    ofAddListener(ofEvents().update, this, &ofxManta::update);
}

bool ofxManta::setup()
{
    try
    {
        Connect();
        if (IsConnected())
        {
            ofLog(OF_LOG_NOTICE, "Successfully connected to Manta.");
            setLedManual(false);
            startThread();
            redraw();
        }
    }
    catch(runtime_error &e)
    {
        if (ofGetFrameNum() < 1) {
            ofLog(OF_LOG_ERROR, ofToString(e.what()));
        }
        redraw();
    }
    return IsConnected();
}

void ofxManta::close()
{
    setLedManual(true);
    markAllPads(Manta::Off);
    markAllSliders(Manta::Off, 0);
    markAllButtons(Manta::Off);
    ofSleepMillis(100); // give it a moment to turn off lights
    if (IsConnected())
    {
        stopThread();
        ofRemoveListener(ofEvents().update, this, &ofxManta::update);
        Disconnect();
    }
}

void ofxManta::setAnimated(bool animated)
{
    this->animated = animated;
    if (!animated) {
        redraw();
    }
}

void ofxManta::update()
{
    if (IsConnected())
    {
        sendEventNotifications();
        if (animated) {
            redrawComponents();
        }
    }
    // try to reconnect
    else if (ofGetFrameNum() % 120 == 0) {
        setup();
    }
}

void ofxManta::draw(int x, int y, int w)
{
    if (w != width)
    {
        width = w;
        height = width * 310.0 / 400.0;
        fbo.allocate(width, height);
        redraw();
    }
    ofSetColor(255);
    fbo.draw(x, y);
}

void ofxManta::redraw()
{
    fbo.begin();
    
    ofPushStyle();
    ofPushMatrix();
    ofEnableAlphaBlending();
    
    ofSetColor(0);
    ofRect(0, 0, width, height);

    // draw sliders
    drawSliders();
    
    // draw buttons
    drawButtons();
    
    // draw pads
    for (int r=0; r<6; r++)
    {
        for (int c=0; c<8; c++) {
            drawPad(r, c);
        }
    }
    
    if (!IsConnected())
    {
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

void ofxManta::redrawComponents()
{
    fbo.begin();
    
    ofPushMatrix();
    ofPushStyle();
    
    if (toRedrawPads)
    {
        for (int idx=0; idx<48; idx++)
        {
            if (padsToRedraw[idx])
            {
                drawPad(floor(idx/8), idx%8);
                padsToRedraw[idx] = false;
            }
        }
        toRedrawPads = false;
    }
    if (toRedrawButtons)
    {
        drawButtons();
        toRedrawButtons = false;
    }
    if (toRedrawSliders)
    {
        drawSliders();
        toRedrawSliders = false;
    }

    ofPopStyle();
    ofPopMatrix();

    fbo.end();
}

void ofxManta::drawPad(int row, int col)
{
    ofPushMatrix();
    
    ofTranslate(ofMap(col + 0.5, 0, 8, 0.01 * width,  0.94 * width),
                ofMap(row + 0.5, 6, 0, 0.24 * height, 0.97 * height));
    if (row % 2 != 0)  ofTranslate(0.93 * width / 16.0, 0);
    ofRotate(90);

    ofSetCircleResolution(6);
    ofSetLineWidth(4);

    if (padLedState[row][col] == Off)
    {
        ofSetColor(0);
        ofNoFill();
        ofCircle(0, 0, width / 20.0);
    }

    ofSetColor(255);
    ofFill();
    ofCircle(0, 0, width / 20.0);
    
    if (animated)
    {
        ofSetColor(0, 255, 0);
        ofCircle(0, 0, width * pad[row][col] / (20.0*203.0));
    }
    
    // draw border if led state on
    if (padLedState[row][col] == Amber)
    {
        ofSetColor(255, 255, 0);
        ofNoFill();
        ofCircle(0, 0, width / 20.0);
    }
    else if (padLedState[row][col] == Red)
    {
        ofSetColor(255, 0, 0);
        ofNoFill();
        ofCircle(0, 0, width / 20.0);
    }
    if (padColor[row][col] != ofColor::white)
    {
        ofSetColor(padColor[row][col]);
        ofNoFill();
        ofCircle(0, 0, width / 20.0);
    }
    if (padSelection[col + 8 * row])
    {
        ofSetColor(0, 255, 0);
        ofNoFill();
        ofCircle(0, 0, width / 20.0);
    }

    // if pad is a toggle
    if (padToggle[col + 8 * row])
    {
        ofSetColor(0);
        ofDrawBitmapString("T", 0, 4);
    }
    
    ofPopMatrix();
}

void ofxManta::drawButtons()
{
    ofPushMatrix();
    ofPushStyle();

    // draw buttons (blackout outline)
    ofSetCircleResolution(32);
    ofPushMatrix();
    ofSetLineWidth(5);
    ofNoFill();
    ofSetColor(0);
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

    // draw buttons (outline)
    ofSetCircleResolution(32);
    ofPushMatrix();
    ofSetLineWidth(3);
    ofFill();
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
    if (buttonLedState[0] == Red)
    {
        ofSetColor(ledColors[1]);
        ofCircle(0, 0, 0.02*width);
    }
    else if (buttonLedState[0] == Amber)
    {
        ofSetColor(ledColors[0]);
        ofCircle(0, 0, 0.02*width);
    }
    if (buttonColor[0] != ofColor::white)
    {
        ofSetColor(buttonColor[0]);
        ofCircle(0, 0, 0.02*width);
    }
    if (buttonSelection[0])
    {
        ofSetColor(0, 255, 0);
        ofCircle(0, 0, 0.02*width);
    }
    
    ofTranslate(0.1*width, 0);
    if (buttonLedState[1] == Red)
    {
        ofSetColor(ledColors[1]);
        ofCircle(0, 0, 0.02*width);
    }
    else if (buttonLedState[1] == Amber)
    {
        ofSetColor(ledColors[0]);
        ofCircle(0, 0, 0.02*width);
    }
    if (buttonColor[1] != ofColor::white)
    {
        ofSetColor(buttonColor[1]);
        ofCircle(0, 0, 0.02*width);
    }
    if (buttonSelection[1])
    {
        ofSetColor(0, 255, 0);
        ofCircle(0, 0, 0.02*width);
    }
    
    ofTranslate(-0.05*width, 0.08*height);
    if (buttonLedState[2] == Red)
    {
        ofSetColor(ledColors[1]);
        ofCircle(0, 0, 0.02*width);
    }
    else if (buttonLedState[2] == Amber)
    {
        ofSetColor(ledColors[0]);
        ofCircle(0, 0, 0.02*width);
    }
    if (buttonColor[2] != ofColor::white)
    {
        ofSetColor(buttonColor[2]);
        ofCircle(0, 0, 0.02*width);
    }
    if (buttonSelection[2])
    {
        ofSetColor(0, 255, 0);
        ofCircle(0, 0, 0.02*width);
    }
    
    ofTranslate(0.1*width, 0);
    if (buttonLedState[3] == Red) {
        ofSetColor(ledColors[1]);
        ofCircle(0, 0, 0.02*width);
    }
    else if (buttonLedState[3] == Amber)
    {
        ofSetColor(ledColors[0]);
        ofCircle(0, 0, 0.02*width);
    }
    if (buttonColor[3] != ofColor::white)
    {
        ofSetColor(buttonColor[3]);
        ofCircle(0, 0, 0.02*width);
    }
    if (buttonSelection[3])
    {
        ofSetColor(0, 255, 0);
        ofCircle(0, 0, 0.02*width);
    }
    
    ofFill();
    ofPopMatrix();
    
    // draw buttons (value)
    if (animated)
    {
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

void ofxManta::drawSliders()
{
    ofPushStyle();
    
    // draw sliders (blackout outline)
    ofPushMatrix();
    ofNoFill();
    ofSetLineWidth(5);
    ofSetColor(0);
    ofTranslate(0.08*width, 0.05*height);
    ofRect(0, 0, 0.65*width, 0.05*height);
    ofTranslate(-0.03*width, 0.08*height);
    ofRect(0, 0, 0.65*width, 0.05*height);
    ofPopMatrix();
    
    // draw sliders (outline)
    ofPushMatrix();
    ofFill();
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
    if (sliderLedState[0] == Red)
    {
        ofSetColor(ledColors[1]);
        ofRect(0, 0, 0.65*width, 0.05*height);
    }
    else if (sliderLedState[0] == Amber)
    {
        ofSetColor(ledColors[0]);
        ofRect(0, 0, 0.65*width, 0.05*height);
    }
    if (sliderColor[0] != ofColor::white)
    {
        ofSetColor(sliderColor[0]);
        ofRect(0, 0, 0.65*width, 0.05*height);
    }
    if (sliderSelection[0])
    {
        ofSetColor(0, 255, 0);
        ofRect(0, 0, 0.65*width, 0.05*height);
    }
    
    ofTranslate(-0.03*width, 0.08*height);
    if (sliderLedState[1] == Red)
    {
        ofSetColor(ledColors[1]);
        ofRect(0, 0, 0.65*width, 0.05*height);
    }
    else if (sliderLedState[1] == Amber)
    {
        ofSetColor(ledColors[0]);
        ofRect(0, 0, 0.65*width, 0.05*height);
    }
    if (sliderColor[1] != ofColor::white)
    {
        ofSetColor(sliderColor[1]);
        ofRect(0, 0, 0.65*width, 0.05*height);
    }
    if (sliderSelection[1])
    {
        ofSetColor(0, 255, 0);
        ofRect(0, 0, 0.65*width, 0.05*height);
    }
    ofFill();
    ofPopMatrix();
    
    // draw sliders (value)
    if (animated)
    {
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

void ofxManta::threadedFunction()
{
    while (isThreadRunning())
    {
        if (lock())
        {
            try {
                HandleEvents();
            }
            catch(runtime_error &e)
            {
                Disconnect();
                unlock();
                stopThread();
                return;
            }
            unlock();
        }
        else
        {
            ofLogWarning("threadedFunction()") << "Unable to lock mutex.";
        }
    }
}

void ofxManta::sendEventNotifications()
{
    if (!IsConnected()) return;
    if(lock())
    {
        for (int i=0; i<events.size(); i++)
        {
            ofNotifyEvent(eventTypeLookup[events[i]->type], *events[i]->event, this);
            delete events[i];
        }
        events.clear();
        unlock();
    }
    else {
        ofLogWarning("threadedFunction()") << "Unable to lock mutex.";
    }
}

void ofxManta::setLedManual(bool manual)
{
    SetLEDControl(PadAndButton, manual);
    SetLEDControl(Slider, manual);
}

void ofxManta::setPadLedState(int row, int column, LEDState state)
{
    setLedManual(true);
    SetPadLED(state, column + 8 * row);
    padLedState[row][column] = state;
    padsToRedraw[column + 8 * row] = true;
    toRedrawPads = true;
    setLedManual(false);
}

void ofxManta::setSliderLedState(int index, LEDState state, int value)
{
    setLedManual(true);
    SetSliderLED(Off, index, 255);
    uint8_t mask = (1 << (7-value));
    SetSliderLED(state, index, mask);
    sliderLedState[index] = state;
    toRedrawSliders = true;
}

void ofxManta::setButtonLedState(int index, LEDState state)
{
    setLedManual(true);
    SetButtonLED(state, index);
    buttonLedState[index] = state;
    toRedrawButtons = true;
}

void ofxManta::markAllPads(LEDState state)
{
    for (int r=0; r<6; r++) {
        for (int c=0; c<8; c++) {
            setPadLedState(r, c, state);
        }
    }
}

void ofxManta::markAllSliders(LEDState state, int value)
{
    for (int i=0; i<2; i++) {
        setSliderLedState(0, state, value);
    }
}

void ofxManta::markAllButtons(LEDState state)
{
    for (int i=0; i<4; i++) {
        setButtonLedState(i, state);
    }
}

void ofxManta::setPadColor(int row, int column, ofColor color)
{
    padColor[row][column] = color;
}

void ofxManta::setSliderColor(int index, ofColor color)
{
    sliderColor[index] = color;
}

void ofxManta::setButtonColor(int index, ofColor color)
{
    buttonColor[index] = color;
}

void ofxManta::addPadToSelection(int row, int col)
{
    int idx = col + 8 * row;
    padSelection[idx] = true;
    padsToRedraw[idx] = true;
    toRedrawPads = true;
}

void ofxManta::addSliderToSelection(int idx)
{
    sliderSelection[idx] = true;
    toRedrawSliders = true;
}

void ofxManta::addButtonToSelection(int idx)
{
    buttonSelection[idx] = true;
    toRedrawButtons = true;
}

void ofxManta::removePadFromSelection(int row, int col)
{
    int idx = col + 8 * row;
    padSelection[idx] = false;
    padsToRedraw[idx] = true;
    toRedrawPads = true;
}

void ofxManta::removeSliderFromSelection(int idx)
{
    sliderSelection[idx] = false;
    toRedrawSliders = true;
}

void ofxManta::removeButtonFromSelection(int idx)
{
    buttonSelection[idx] = false;
    toRedrawButtons = true;
}

void ofxManta::clearPadSelection()
{
    for (int i=0; i<48; i++)
    {
        padSelection[i] = false;
        padsToRedraw[i] = true;
    }
    toRedrawPads = true;
}

void ofxManta::clearSliderSelection()
{
    for (int i=0; i<2; i++) {
        sliderSelection[i] = false;
    }
    toRedrawSliders = true;
}

void ofxManta::clearButtonSelection()
{
    for (int i=0; i<4; i++) {
        buttonSelection[i] = false;
    }
    toRedrawButtons = true;
}

void ofxManta::clearSelection()
{
    clearPadSelection();
    clearSliderSelection();
    clearButtonSelection();
}

bool ofxManta::getPadSelected(int idx)
{
    return padSelection[idx];
}

bool ofxManta::getSliderSelected(int idx)
{
    return sliderSelection[idx];
}

bool ofxManta::getButtonSelected(int idx)
{
    return buttonSelection[idx];
}

vector<int> ofxManta::getSelection(map<int, bool> & selected)
{
    vector<int> idx;
    map<int, bool>::iterator it = selected.begin();
    while (it != selected.end())
    {
        if (selected[it->first]) {
            idx.push_back(it->first);
        }
        ++it;
    }
    return idx;
}

int ofxManta::getSizeSelection()
{
    return getPadSelection().size() + getSliderSelection().size() + getButtonSelection().size();
}

void ofxManta::setPadFreezeEnabled(bool toFreezePads)
{
    this->toFreezePads = toFreezePads;
    setButtonLedState(1, toFreezePads ? Manta::Red : Manta::Off);
    setButtonColor(1, toFreezePads ? ofColor::blue : ofColor::white);
}

void ofxManta::enablePadToggle(int row, int column, bool toggle)
{
    padToggle[column + 8 * row] = toggle;
    padsToRedraw[column + 8 * row] = true;
    toRedrawPads = true;
    if (toggle) {
        pad[row][column] = 0;
    }
}

void ofxManta::freezePads()
{
    for (int r = 0; r < 6; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            if (getPad(r, c) > 0)
            {
                padFrozen[c + 8 * r] = true;
                padReleased[c + 8 * r] = false;
            }
        }
    }
}

void ofxManta::clearPadStates()
{
    for (int i = 0; i < 48; i++)
    {
        padFrozen[i] = false;
        padReleased[i] = true;
        enablePadToggle(floor(i / 8), i % 8, false);
    }
}

void ofxManta::PadEvent(int row, int column, int id, int value)
{
    if (!padFrozen[id] && !padToggle[id])
    {
        events.push_back(new MantaEvent(PAD, new ofxMantaEvent(row, column, id, value)));
        padsToRedraw[column + 8 * row] = (value != pad[row][column]);
        pad[row][column] = padMult * value;
        toRedrawPads = animated;
    }
    else if (padReleased[id]) {
        padFrozen[id] = false;
    }
}

void ofxManta::SliderEvent(int id, int value)
{
    if (value == 65535) value = -1;
    events.push_back(new MantaEvent(SLIDER, new ofxMantaEvent(NULL, NULL, id, value)));
    toRedrawSliders = animated;
    if (value == -1) return;
    slider[id] = sliderMult * value;
}

void ofxManta::ButtonEvent(int id, int value)
{
    events.push_back(new MantaEvent(BUTTON, new ofxMantaEvent(NULL, NULL, id, value)));
    button[id] = buttonMult * value;
    toRedrawButtons = animated;
}

void ofxManta::PadVelocityEvent(int row, int column, int id, int value)
{
    if (padToggle[column + 8 * row] && padReleased[column + 8 * row])
    {
        float newValue = 1.0 - pad[row][column];
        events.push_back(new MantaEvent(PAD, new ofxMantaEvent(row, column, id, newValue)));
        padsToRedraw[column + 8 * row] = true;
        pad[row][column] = newValue;
        toRedrawPads = animated;
        padReleased[column + 8 * row] = false;
        setPadLedState(row, column, newValue == 0.0 ? Manta::Off : Manta::Red);
    }
    if (value == 0) padReleased[id] = true;
    events.push_back(new MantaEvent(PAD_VELOCITY, new ofxMantaEvent(row, column, id, value)));
}

void ofxManta::ButtonVelocityEvent(int id, int value)
{
    if (toFreezePads && id==1 && value==0) {
        freezePads();
    }
    events.push_back(new MantaEvent(BUTTON_VELOCITY, new ofxMantaEvent(NULL, NULL, id, value)));
}

ofxManta::~ofxManta()
{
    this->close();
}
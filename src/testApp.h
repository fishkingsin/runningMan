#pragma once

#include "ofMain.h"
#include "ofxImageSequence.h"
#include "ofxDuration.h"
#include "ofxUI.h"
class Data
{
public:
    Data()
    {
        isLoop = false;
        alpha = ofRandom(100,200);
    }
    int startTime;
    float scale;

    bool isLoop;
    float alpha;
    vector<ofImage*> *image;
    int length;
};
class testApp : public ofBaseApp {
    
public:
    
    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void trackUpdated(ofxDurationEventArgs& args);
        void guiEvent(ofxUIEventArgs &e);
    int   appFPS;
    vector <ofImage*> images;
    vector <ofImage*> images2;
//    ofxImageSequence images;
//    ofxImageSequence images2;
    vector<Data*>data;
    float alpha;
    ofxDuration duration;
    ofFbo fbo;
     ofxUICanvas *gui;
    ofRectangle output;
};

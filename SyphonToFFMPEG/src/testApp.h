#pragma once

#include "ofMain.h"
#include "ofxVideoRecorder.h"
#include "ofxTiming.h"
#include "ofxFastFboReader.h"
#include "ofxSyphon.h"
class testApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed  (int key);
    void keyReleased(int key);

    void serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg);
    void serverUpdated(ofxSyphonServerDirectoryEventArgs &args);
    void serverRetired(ofxSyphonServerDirectoryEventArgs &arg);
    
    ofxSyphonServerDirectory dir;
    ofxSyphonClient client;
    int dirIdx;

    
    
    bool drawText, drawVideo;
    bool drawLoading;

    int syphonWidth, syphonHeight;
    ofTrueTypeFont font;
    ofFbo fbo;
    ofPixels pixels;
    ofImage img;
    ofxFastFboReader reader;
    ofxVideoRecorder    ffmpeg;
 
    bool bRecording;
    int sampleRate;
    int channels;
    string fileName;
    string fileExt;
    string ffmpegOutput;
    ofFbo recordFbo;
    ofPixels recordPixels;
    DelayTimer timer;

};

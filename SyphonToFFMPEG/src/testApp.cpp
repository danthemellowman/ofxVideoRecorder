#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(false);
    ofSetFrameRate(30);
    
    syphonWidth = 1280;
    syphonHeight = 720;
    
    //setup our directory
    dir.setup();
    //setup our client
    client.setup();

    //register for our directory's callbacks
    ofAddListener(dir.events.serverAnnounced, this, &testApp::serverAnnounced);
    ofAddListener(dir.events.serverRetired, this, &testApp::serverRetired);
    
    dirIdx = -1;
    

    
    reader.setAsync(false);
    
    bRecording = false;
    
    fbo.allocate(640, 480, GL_RGB);
    pixels.allocate(640, 480, 3);
    
    font.loadFont(OF_TTF_MONO, 24);
    timer.setFramerate(30);
    ofXml xml;
    xml.load("settings.xml");
    ffmpegOutput = xml.getValue("//output", "");
}

void testApp::exit() {
    if(bRecording){
        ffmpeg.close();
    }
}

//--------------------------------------------------------------
void testApp::update(){
    if(dir.isValidIndex(dirIdx)){
        if(fbo.getWidth() != client.getWidth()){
            fbo.allocate(client.getWidth(), client.getHeight(), GL_RGB);
            pixels.allocate(fbo.getWidth(), fbo.getHeight(), 3);
            if(bRecording){
                ffmpeg.close();
                ffmpeg.setupCustomOutput(fbo.getWidth(), fbo.getHeight(), 30, "-vcodec libx264 -pix_fmt yuv420p -b 1600k -f "+ffmpegOutput);
                ffmpeg.start();
            }
            ofSetWindowShape(fbo.getWidth(), fbo.getHeight());
        }
    }
    
    fbo.begin();
    ofClear(0, 0, 0);
    if(dir.isValidIndex(dirIdx))
        client.draw(0, 0);
    fbo.end();
    
    
    if(timer.tick() && bRecording){
        reader.readToPixels(fbo, pixels);
        ffmpeg.addFrame(pixels);
    }
    
}

//--------------------------------------------------------------
void testApp::draw(){
    ofEnableAlphaBlending();
    ofSetColor(255, 255, 255);
    fbo.draw(0, 0);
    
    stringstream ss;
    ss << "video queue size: " << ffmpeg.getVideoQueueSize() << endl
    << "FPS: " << ofGetFrameRate() << endl
    << "use the LEFT or RIGHT arrow keys to change syphon feeds " << endl
    << "Your FFMPEG Output is "<<ffmpegOutput << endl
    << "The r key toggle Recording"<< endl
    << "The c key closes the Recording"<< endl
    << "The s key reloads the settings.xml"<< endl;
    
    ofSetColor(255, 255, 255);
    ofDrawBitmapStringHighlight(ss.str(),15,15);
    
    if(bRecording){
        ofSetColor(255, 0, 0);
        ofCircle(ofGetWidth() - 20, 20, 5);
    }
    ofSetColor(255, 255, 255);
    ofDisableAlphaBlending();
}

//these are our directory's callbacks
void testApp::serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Announced")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    dirIdx = 0;
}

void testApp::serverRetired(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Retired")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    dirIdx = 0;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == '1'){
        drawVideo = !drawVideo;
    }
    if(key == '2'){
        drawText = !drawText;
    }
    if(key == '3'){
        drawLoading = !drawLoading;
    }
}


//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
    if(dir.size()> 0 && (key == OF_KEY_RIGHT || key == OF_KEY_LEFT)){
        
        
        if(key == OF_KEY_RIGHT){
            dirIdx++;
            if(dirIdx > dir.size() - 1)
                dirIdx = 0;
            
            client.set(dir.getDescription(dirIdx));
            string serverName = client.getServerName();
            string appName = client.getApplicationName();
            
            if(serverName == ""){
                serverName = "null";
            }
            if(appName == ""){
                appName = "null";
            }
            ofSetWindowTitle(serverName + ":" + appName);
        }
        
        if(key == OF_KEY_LEFT){
            dirIdx--;
            if(dirIdx < 0)
                dirIdx = dir.size() - 1;
            
            
            client.set(dir.getDescription(dirIdx));
            string serverName = client.getServerName();
            string appName = client.getApplicationName();
            
            if(serverName == ""){
                serverName = "null";
            }
            if(appName == ""){
                appName = "null";
            }
    
            ofSetWindowTitle(serverName + ":" + appName);
        }
    }
    
    if(key == 'r'){

        bRecording = !bRecording;
      
        if(bRecording && !ffmpeg.isInitialized()) {
            ffmpeg.setupCustomOutput(fbo.getWidth(), fbo.getHeight(), 30, "-vcodec libx264 -pix_fmt yuv420p -b 1600k -f "+ffmpegOutput);
            
            ffmpeg.start();
        }
    }
    
    if(key == 'c'){
        bRecording = false;
        ffmpeg.close();
    }
    if(key == 's'){
        ofXml xml;
        xml.load("settings.xml");
        ffmpegOutput = xml.getValue("//output", "");
    }
}



#include "testApp.h"


/*
 
 Image Sequence Example:
 In this example we are going to load a sequence of images from a folder.
 We know that the images are in order and the we saved them out at 24fps.
 the goal is to play them back at 24fps independent of the fps of the app.
 You can toggle the sequence to be independent of the app fps.
 
 Topics:
 - ofDirectory
 - ofImage
 - timing
 
 gif from: http://probertson.livejournal.com/32350.html
 
 */


//--------------------------------------------------------------
void testApp::setup() {
    ofBackground(0);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetVerticalSync(true);

    ofToggleFullscreen();
    // read the directory for the images
    // we know that they are named in seq
    //    ofDirectory dir;
    alpha = 255;
    //    images.loadSequence("runnging_cycle");
    //    images2.loadSequence("walking_cycle");
    ofDirectory dir;
    dir.allowExt("png");
    dir.allowExt("tif");
    dir.allowExt("tga");
    dir.allowExt("jpg");
    dir.allowExt("jpeg");
    int nFiles = dir.listDir("runnging_cycle");
    if(nFiles) {
        for(int i=0; i<dir.numFiles(); i++) {
            images.push_back(new ofImage() );
            images.back()->loadImage(dir.getPath(i));
            //            sequence.push_back(NULL);
        }
    } else ofLog(OF_LOG_ERROR, "Could not find folder " );
    nFiles = dir.listDir("walking_cycle");
    if(nFiles) {
        for(int i=0; i<dir.numFiles(); i++) {
            images2.push_back(new  ofImage() );
            images2.back()->loadImage(dir.getPath(i));
            //            sequence.push_back(NULL);
        }
    } else ofLog(OF_LOG_ERROR, "Could not find folder " );
    
    appFPS = 60;
    ofSetFrameRate(appFPS);
    fbo.allocate(ofGetWidth(), ofGetHeight(),GL_RGBA);

    alpha = 1.0;
    
    duration.setup(12345);
	//ofxDuration is an OSC receiver, with special functions to listen for Duration specific messages
	//optionally set up a font for debugging
	duration.setupFont("GUI/NewMedia Fett.ttf", 12);
	ofAddListener(duration.events.trackUpdated, this, &testApp::trackUpdated);
    gui = new ofxUICanvas(0, 0, ofGetWidth()*0.25, ofGetHeight());
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = (ofGetWidth()*0.25)-xInit;
    int dim = 32;
    gui->addLabel("RunningMan", OFX_UI_FONT_LARGE);
    gui->setDrawPaddingOutline(true);
    gui->setDrawBack(true);
    gui->setColorBack(ofColor(125,125,125,125));
    gui->addFPS();
	
    gui->addSlider("X", 0.0, ofGetScreenWidth()*2.0f, 0.0f, length-xInit, dim);
    gui->addSlider("Y", -ofGetScreenHeight(), ofGetScreenHeight(), 0.0f, length-xInit, dim);
    gui->addSlider("WIDTH", 0.0, ofGetScreenWidth()*2.0f ,  ofGetWidth(), length-xInit, dim);
    gui->addSlider("HEIGHT", 0.0, ofGetScreenWidth()*2.0f ,  ofGetHeight(), length-xInit, dim);
    gui->addSlider("BG_ALPHA", 0.0, 1 ,  1, length-xInit, dim);
    
	
	ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings("GUI/guiSettings.xml");
	gui->toggleVisible();
        ofHideCursor();
	ofAddListener(duration.events.trackUpdated, this, &testApp::trackUpdated);
    

}
void testApp::guiEvent(ofxUIEventArgs &e)
{
    if(e.widget->getName()=="X")
    {
        output.x = ((ofxUISlider*)e.widget)->getScaledValue();
    }else if(e.widget->getName()=="Y")
    {
        output.y = ((ofxUISlider*)e.widget)->getScaledValue();
    }else if(e.widget->getName()=="WIDTH")
    {
        output.width = ((ofxUISlider*)e.widget)->getScaledValue();
    }else if(e.widget->getName()=="HEIGHT")
    {
        output.height = ((ofxUISlider*)e.widget)->getScaledValue();
    }
    else if(e.widget->getName()=="BG_ALPHA")
    {
        alpha = ((ofxUISlider*)e.widget)->getScaledValue();
    }
    
}
void testApp::exit()
{
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
    while (!data.empty()) {
        data.pop_back();
    }
    while (!images.empty()) {
        images.pop_back();
    }
    while (!images2.empty()) {
        images2.pop_back();
    }
}
//--------------------------------------------------------------
//Or wait to receive messages, sent only when the track changed
void testApp::trackUpdated(ofxDurationEventArgs& args){
	
    if(args.track->name=="/ALPHA")
    {
//        ofLogVerbose("Duration Event") << "track type " << args.track->type << " updated with name " << args.track->name << " and value " << args.track->value << endl;
        alpha = args.track->value;
        ((ofxUISlider*)gui->getWidget("BG_ALPHA"))->setValue(alpha);
//        ofLogVerbose("testApp") << alpha;
    }
    else if(args.track->name=="/MINUS")
    {
        if(data.size()>0)
        {
            Data * d = data[(int)ofRandom(data.size())];
            d->isLoop = false;
        }
    }
    
//    if(ofGetFrameRate()>30)
    {
        if(args.track->name=="/LEFT_RUN")
        {
            Data* d = new Data();
            d->scale = 1;
            d->startTime = 0;
            d->image = &images;
            d->length = images.size();
            data.push_back(d);
        }
        if(args.track->name=="/RIGHT_RUN")
        {
            Data* d = new Data();
            d->scale = -1;
            d->startTime = 0;
            d->image = &images;
            d->length = images.size();
            data.push_back(d);
        }
        if(args.track->name=="/LEFT_WALK")
        {
            Data* d = new Data();
            d->scale = 1;
            d->startTime = 0;
            d->image = &images2;
            d->length = images2.size();
            data.push_back(d);
        }
        if(args.track->name=="/RIGHT_WALK")
        {
            Data* d = new Data();
            d->scale = -1;
            d->startTime = 0;
            d->image = &images2;
            d->length = images2.size();
            data.push_back(d);
        }
        if(args.track->name=="/LOW_FRAMERATE")
        {
            if(args.track->on)
            {
                ofSetFrameRate(30);
            }
            else{
                ofSetFrameRate(60);
            }
        }
    }
    
    
}
//--------------------------------------------------------------
void testApp::update() {
    vector<Data*>::iterator it;
    // draw the image sequence at the new frame count
    for( it = data.begin() ; it != data.end(); it++)
    {
        Data* d = *it;
        d->startTime++;
        if(!d->isLoop && d->startTime >= d->length){
            data.erase(it);
            it--;
            
        }
        else
        {
            d->startTime %= d->length;
        }
        
        
    }
    
    
}

//--------------------------------------------------------------
void testApp::draw() {
    ofBackground(0);

    fbo.begin();

    ofPushStyle();
    ofFill();
    ofEnableAlphaBlending();
    
    ofSetColor(0,0,0,alpha*255);
//    ofLogVerbose("testApp") << "alpha " << alpha;
    ofRect(0, 0, ofGetWidth(), ofGetHeight());
    ofPopStyle();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    
    
    vector<Data*>::iterator it;
    // draw the image sequence at the new frame count
    for( it = data.begin() ; it != data.end(); it++)
    {
        
        ofPushMatrix();
        Data* d = *it;
        ofTranslate((d->scale<0)?1024:0, 0);
        ofScale(d->scale,1);
        ofPushStyle();
        {
            ofSetColor(255,d->alpha);
            ofImage *img = d->image->operator[](int(d->startTime));
            img->draw(0,fbo.getHeight()-img->getHeight());
        }
        ofPopMatrix();
        ofPopStyle();
        
    }
    
    fbo.end();
    fbo.draw(output.x,output.y,output.width,output.height);
    ofRect(mouseX,output.y+output.height,10,10);
    ofPushMatrix();
    ofPushStyle();
    if(gui->isVisible())duration.draw(ofGetWidth()*0.25,0, ofGetWidth(), ofGetHeight());
    ofPopMatrix();
    ofPopStyle();   
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
//    if(ofGetFrameRate()>50)
    {
        
        if(key == OF_KEY_LEFT)      {
            Data* d = new Data();
            d->scale = 1;
            d->startTime = 0;
            d->image = &images;
            d->length = images.size();
            data.push_back(d);
        }
        if(key == OF_KEY_RIGHT)      {
            Data* d = new Data();
            d->scale = -1;
            d->startTime = 0;
            d->image = &images;
            d->length = images.size();
            data.push_back(d);
        }
        if(key == '1')      {
            Data* d = new Data();
            d->scale = 1;
            d->startTime = 0;
            d->image = &images2;
            d->length = images2.size();
            data.push_back(d);
            
        }
        if(key == '2')      {
            Data* d = new Data();
            d->scale = -1;
            d->startTime = 0;
            d->image = &images2;
            d->length = images2.size();
            data.push_back(d);
        }
        if(key == '3')      {
            Data* d = new Data();
            d->isLoop = true;
            d->scale = 1;
            d->startTime = 0;
            d->image = &images2;
            d->length = images2.size();
            data.push_back(d);
        }
        if(key == '4')      {
            Data* d = new Data();
            d->isLoop = true;
            d->scale = -1;
            d->startTime = 0;
            d->image = &images2;
            d->length = images2.size();
            data.push_back(d);
        }
        if(key == '5')      {
            Data* d = new Data();
            d->isLoop = true;
            d->scale = 1;
            d->startTime = 0;
            d->image = &images;
            d->length = images.size();
            data.push_back(d);
        }
        if(key == '6')      {
            Data* d = new Data();
            d->isLoop = true;
            d->scale = -1;
            d->startTime = 0;
            d->image = &images;
            d->length = images.size();
            data.push_back(d);
        }
    }
    if (key=='-')
    {
        if(data.size()>0)
        {
            Data * d = data[(int)ofRandom(data.size())];
            d->isLoop = false;
        }
    }
    if(key=='f')
    {
        ofToggleFullscreen();
    }
    if(key == '\t')
	{
		gui->toggleVisible();
        if(gui->isVisible())
        {
            ofShowCursor();
        }
        else{
            ofHideCursor();
        }
        
	}if(key== ' ')
    {
        ofSetFrameRate(30);
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    if(key== ' ')
    {
        ofSetFrameRate(60);
    }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    alpha = (x*1.0f/ofGetWidth());
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    
}
#include "testApp.h"

//PRESS 1 - 2 - 3 - 4 - 5 to change the effects
//--------------------------------------------------------------
void testApp::setup()
{
    //LOAD YOUR SONG INTO HERE
    //PUT MUSIC IN THE BIN/DATA/SOUNDS DIRECTORY
    chune.loadSound("sounds/soundfile.mp3");
    
    // the fft needs to be smoothed out, so we create an array of floats
	// for that purpose:
	fftSmoothed = new float[8192];
	for (int i = 0; i < 8192; i++){
		fftSmoothed[i] = 0;
	}
	
	nBandsToGet = 128;
    
    
    camWidth = 1280;
    camHeight = 720;
    
    vidGrabber.initGrabber(camWidth, camHeight);
    
    
    tex1.allocate(camWidth,camHeight,GL_RGB,GL_RGBA);
    
    //LOAD THE SHADERS
    wobbleShader.load("shaders/Wobble_GLSL");
    testShader.load("shaders/testEXP");
    chromaShader.load("shaders/ChromaAb_GLSL");
    chromaWobble.load("shaders/Wobble_Abberation");
    RGBPoster.load("shaders/RGBPost");
    ripple.load("shaders/ripple");
    switchInt = 0;
}

//--------------------------------------------------------------
void testApp::update(){
    ofSoundUpdate();
   if(!chune.getIsPlaying())
   {
    chune.play();
   }
   
    // (5) grab the fft, and put in into a "smoothed" array,
	//		by taking maximums, as peaks and then smoothing downward
	float * val = ofSoundGetSpectrum(nBandsToGet);		// request 128 values for fft
	for (int i = 0;i < nBandsToGet; i++){
        
		// let the smoothed calue sink to zero:
		fftSmoothed[i] *= 0.96f;
		
		// take the max, either the smoothed or the incoming:
		if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];
		
	}
  
    for(int i = 0; i<3; i++) avgSound[i] = 0;
    
    // smooth fft and calc average volume
	for (int i = 0;i < nBandsToGet; i++){
		fftSmoothed[i] *= 0.96f;
		if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];
    
        if(i < (nBandsToGet/3)) avgSound[0] += fftSmoothed[i];
        if(i < (nBandsToGet/3)*2 && i > (nBandsToGet/2)) avgSound[1] += fftSmoothed[i];
        if(i > (nBandsToGet/3)*2) avgSound[2] += fftSmoothed[i];
	}
    
    // calculate average loudness of the music for "volume"
    avgSound[0] /= nBandsToGet;
    
    vidGrabber.update();
    
    if (vidGrabber.isFrameNew()) {
        tex1=vidGrabber.getTextureReference();
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofEnableNormalizedTexCoords();

    
    switch(switchInt)
    {
        case 1:
            posterize();
            break;
        case 2:
            rgbPosterize();
            break;
        case 3:
            wobble();
            break;
        case 4:
            abberation();
            break;
        case 5:
            wobbleAbberation();
            break;
    
            default:
            posterize();
            break;
    }
     

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::posterize()
{
    tex1.bind();
    
    testShader.begin();
    testShader.setUniformTexture("tex", tex1, 0);
    testShader.setUniform1f("thresh", 4*avgSound[0]);
    
    ofPushMatrix();
    ofTranslate(0,0);
    glBegin(GL_QUADS);
    
    glTexCoord2f(0,0); glVertex3f(0,0,0);
    glTexCoord2f(1,0); glVertex3f(1280,0,0);
    glTexCoord2f(1,1); glVertex3f(1280,720,0);
    glTexCoord2f(0,1); glVertex3f(0,720,0);
    
    glEnd();
    ofPopMatrix();
    testShader.end();
    tex1.unbind();
}

//--------------------------------------------------------------
void testApp::rgbPosterize()
{
    tex1.bind();
    
    RGBPoster.begin();
    RGBPoster.setUniformTexture("tex", tex1, 0);
    RGBPoster.setUniform1f("threshG", 4*avgSound[0]);
    RGBPoster.setUniform1f("threshB", 2*avgSound[1]);
    RGBPoster.setUniform1f("threshR", 2*avgSound[2]);
    
    ofPushMatrix();
    ofTranslate(0,0);
    glBegin(GL_QUADS);
    
    glTexCoord2f(0,0); glVertex3f(0,0,0);
    glTexCoord2f(1,0); glVertex3f(1280,0,0);
    glTexCoord2f(1,1); glVertex3f(1280,720,0);
    glTexCoord2f(0,1); glVertex3f(0,720,0);
    
    glEnd();
    ofPopMatrix();
    testShader.end();
    tex1.unbind();
}

//--------------------------------------------------------------
void testApp::wobble()
{
    tex1.bind();
    wobbleShader.begin();
    wobbleShader.setUniformTexture("image", tex1, 0);
    wobbleShader.setUniform2f("amp", 6,6); //These settings are finicky. Be careful with your ranges
    wobbleShader.setUniform1f("radius", 150);
    wobbleShader.setUniform2f("freq", .0-(.08*avgSound[0]),.0-(.06*avgSound[1]));
    
    ofPushMatrix();
    ofTranslate(0, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(0,0,0);
    glTexCoord2f(1,0); glVertex3f(1280,0,0);
    glTexCoord2f(1,1); glVertex3f(1280,720,0);
    glTexCoord2f(0,1); glVertex3f(0,720,0);
    glEnd();
    ofPopMatrix();
    wobbleShader.end();
    tex1.unbind();
}

//--------------------------------------------------------------
void testApp::abberation()
{
    tex1.bind();
    
    chromaShader.begin();
    chromaShader.setUniformTexture("tex", tex1, 0);
    chromaShader.setUniform2f("windowSize", 1280, 720);
    chromaShader.setUniform1f("offsetALL", 50*avgSound[1]);
    chromaShader.setUniform2f("offsetVecR", 100*avgSound[0]*sin(ofGetElapsedTimeMillis()), 100*avgSound[0]*cos(ofGetElapsedTimeMillis()));
    chromaShader.setUniform2f("offsetVecG", 35*avgSound[1]*cos(ofGetElapsedTimeMillis()), 35*avgSound[1]*sin(ofGetElapsedTimeMillis()));
    chromaShader.setUniform2f("offsetVecB", 35*avgSound[2]*sin(ofGetElapsedTimeMillis()+(pi/2)), 354*avgSound[2]*cos(ofGetElapsedTimeMillis())+(pi/2));
    
    ofPushMatrix();
    ofTranslate(0,0);
    glBegin(GL_QUADS);
    
    glTexCoord2f(0,0); glVertex3f(0,0,0);
    glTexCoord2f(1,0); glVertex3f(1280,0,0);
    glTexCoord2f(1,1); glVertex3f(1280,720,0);
    glTexCoord2f(0,1); glVertex3f(0,720,0);
    
    glEnd();
    ofPopMatrix();
    chromaShader.end();
    tex1.unbind();
}

//--------------------------------------------------------------
void testApp::wobbleAbberation()
{
    tex1.bind();
    chromaWobble.begin();
    chromaWobble.setUniformTexture("image", tex1, 0);
    chromaWobble.setUniform2f("amp", 6,6); //These settings are finicky. Be careful with your ranges
    chromaWobble.setUniform1f("radius", 150*avgSound[0]);
    chromaWobble.setUniform2f("freqR", .0-(0.125*avgSound[0]),.0-(0.125*avgSound[0]));
    chromaWobble.setUniform2f("freqG", .0-(0.05*avgSound[1]),.0-(0.05*avgSound[1]));
    chromaWobble.setUniform2f("freqB", .0+(0.035*avgSound[2]),.0+(0.035*avgSound[2]));
    ofPushMatrix();
    ofTranslate(0, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(0,0,0);
    glTexCoord2f(1,0); glVertex3f(1280,0,0);
    glTexCoord2f(1,1); glVertex3f(1280,720,0);
    glTexCoord2f(0,1); glVertex3f(0,720,0);
    glEnd();
    ofPopMatrix();
    chromaWobble.end();
    tex1.unbind();
}


//--------------------------------------------------------------
void testApp::keyReleased(int key){
    if(key == '1')
    {
        switchInt = 1;
    }
    if(key == '2')
    {
        switchInt = 2;
    }
    if(key == '3')
    {
        switchInt = 3;
    }
    if(key == '4')
    {
        switchInt = 4;
    }
    if(key == '5')
    {
        switchInt = 5;
    }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
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

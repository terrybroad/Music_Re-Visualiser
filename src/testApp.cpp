#include "testApp.h"

//PRESS 1 - 2 - 3 - 4 - 5 to change the effects
//--------------------------------------------------------------
void testApp::setup()
{
    //LOAD YOUR SONG INTO HERE
    //PUT MUSIC IN THE BIN/DATA/SOUNDS DIRECTORY
    chune.loadSound("sounds/Jack.mp3");
    
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
    liquify.load("shaders/ripple");
    liquifyS.load("shaders/rippleSingleChannel");
    perlinLAB.load("shaders/perlinLAB");
    LABFluctuate.load("shaders/LABFluctuate");
    perlinXYZ.load("shaders/perlinXYZ");
    perlinHSV.load("shaders/perlinHSV");
    brightness.load("shaders/brightness");
    switchInt = 0;
    
    
    //MAXIMILLIAN TEST STUFF
    
	sampleRate 			= 44100; /* Sampling Rate */
	initialBufferSize	= 512;	/* Buffer Size. you have to fill this buffer with sound*/
	lAudioOut			= new float[initialBufferSize];/* outputs */
	rAudioOut			= new float[initialBufferSize];
	lAudioIn			= new float[initialBufferSize];/* inputs */
	rAudioIn			= new float[initialBufferSize];
    
    
	/* This is a nice safe piece of code */
	memset(lAudioOut, 0, initialBufferSize * sizeof(float));
	memset(rAudioOut, 0, initialBufferSize * sizeof(float));
    
	memset(lAudioIn, 0, initialBufferSize * sizeof(float));
	memset(rAudioIn, 0, initialBufferSize * sizeof(float));
    
	/* Now you can put anything you would normally put in maximilian's 'setup' method in here. */
    
    //	samp.load(ofToDataPath("sinetest_stepping2.wav"));
    //		samp.load(ofToDataPath("whitenoise2.wav"));
    //	samp.load(ofToDataPath("additive22.wav"));
	//samp.load(ofToDataPath("pinknoise2.wav"));
	/*
    samp.load(ofToDataPath("sounds/B.wav"));
	samp.getLength();
    
    
	fftSize = 1024;
	mfft.setup(fftSize, 512, 256);
	ifft.setup(fftSize, 512, 256);
    
    
    
	nAverages = 12;
	oct.setup(sampleRate, fftSize/2, nAverages);
    
	mfccs = (double*) malloc(sizeof(double) * 13);
	mfcc.setup(512, 42, 13, 20, 20000, sampleRate);
    
	ofxMaxiSettings::setup(sampleRate, 2, initialBufferSize);
	ofSoundStreamSetup(2,2, this, sampleRate, initialBufferSize, 4);/* Call this last ! */
    
	ofSetVerticalSync(true);


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
    
    float centroidTop = 0;
    float centroidBottom = 0;
    for (int i = 0; i < nBandsToGet/2; i++)
    {
        centroidBottom += fftSmoothed[i];
        centroidTop += (fftSmoothed[i]*i);
    }
    centroid = centroidTop/centroidBottom;
    centroid = centroid/(nBandsToGet/2);
   // cout << centroid;
   // cout << " --- ";
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
            perlinLab();
            break;
        case 6:
            wobbleAbberation();
            break;
        case 7:
            liquifyRender();
            break;
        case 8:
            perlinXyz();
            break;
        default:
            liquifyRenderS();
            break;
    }
    
    /*
    	cout << "\nMFCCS: ";
	ofSetColor(255, 0, 0,100);
	float xinc = 900.0 / 13;
	for(int i=0; i < 13; i++) {
		float height = mfccs[i] * 250.0;
		ofRect(100 + (i*xinc),600 - height,40, height);
        		cout << mfccs[i] << ",";
	}
     */
    
}

//--------------------------------------------------------------
void testApp::audioRequested 	(float * output, int bufferSize, int nChannels){
    //	static double tm;
	for (int i = 0; i < bufferSize; i++){
        //		wave = osc.saw(maxiMap::linexp(mouseY + ofGetWindowPositionY(), 0, ofGetScreenHeight(), 200, 8000));
        //		wave = lAudioIn[i];
		wave = samp.play(1.);
		//get fft
		if (mfft.process(wave)) {
            //			int bins   = fftSize / 2.0;
			//do some manipulation
            //			int hpCutoff = floor(((mouseX + ofGetWindowPositionX()) / (float) ofGetScreenWidth()) * fftSize / 2.0);
            //highpass
            //			memset(mfft.magnitudes, 0, sizeof(float) * hpCutoff);
            //			memset(mfft.phases, 0, sizeof(float) * hpCutoff);
            //lowpass
            //			memset(mfft.magnitudes + hpCutoff, 0, sizeof(float) * (bins - hpCutoff));
            //			memset(mfft.phases + hpCutoff, 0, sizeof(float) * (bins - hpCutoff));
			mfft.magsToDB();
            //			for(int z=0; z < 512; z++) cout << mfft.magnitudesDB[z] << ",";
            //			cout << "---------\n";
			oct.calculate(mfft.magnitudesDB);
			mfcc.mfcc(mfft.magnitudes, mfccs);
			//cout << mfft.spectralFlatness() << ", " << mfft.spectralCentroid() << endl;
		}
		//inverse fft
		gettimeofday(&callTS,NULL);
        //		ifftVal = ifft.process(mfft.magnitudes, mfft.phases);
		gettimeofday(&callEndTS,NULL);
		callTime = (float)(callEndTS.tv_usec - callTS.tv_usec) / 1000000.0;
		callTime += (float)(callEndTS.tv_sec - callTS.tv_sec);
		//play result
		mymix.stereo(wave, outputs, 0.5);
        //		float mix = ((mouseX + ofGetWindowPositionX()) / (float) ofGetScreenWidth());
        //		mymix.stereo((wave * mix) + ((1.0-mix) * ifftVal), outputs, 0.5);
		lAudioOut[i] = output[i*nChannels    ] = outputs[0]; /* You may end up with lots of outputs. add them here */
		rAudioOut[i] = output[i*nChannels + 1] = outputs[1];
	}
    
    
    
}

//--------------------------------------------------------------
void testApp::audioReceived 	(float * input, int bufferSize, int nChannels){
    
    
	/* You can just grab this input and stick it in a double, then use it above to create output*/
    
	for (int i = 0; i < bufferSize; i++){
        
		/* you can also grab the data out of the arrays*/
        
		lAudioIn[i] = input[i*2];
		rAudioIn[i] = input[i*2+1];
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
void testApp::liquifyRender()
{
    tex1.bind();
    liquify.begin();
    liquify.setUniformTexture("tex", tex1, 0);
    liquify.setUniform1f("changingR", avgSound[0]*2);
    liquify.setUniform1f("changingG", avgSound[1]/2);
    liquify.setUniform1f("changingB", avgSound[2]/2);
    liquify.setUniform1f("time", ofGetElapsedTimeMillis());
    
    ofPushMatrix();
    ofTranslate(0, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(0,0,0);
    glTexCoord2f(1,0); glVertex3f(1280,0,0);
    glTexCoord2f(1,1); glVertex3f(1280,720,0);
    glTexCoord2f(0,1); glVertex3f(0,720,0);
    glEnd();
    ofPopMatrix();
    liquify.end();
    tex1.unbind();
}
//--------------------------------------------------------------
void testApp::liquifyRenderS()
{
    tex1.bind();
    liquifyS.begin();
    liquifyS.setUniformTexture("tex", tex1, 0);
    liquifyS.setUniform1f("changing", avgSound[0]*2);
    liquifyS.setUniform1f("time", ofGetElapsedTimeMillis());
    
    ofPushMatrix();
    ofTranslate(0, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(0,0,0);
    glTexCoord2f(1,0); glVertex3f(1280,0,0);
    glTexCoord2f(1,1); glVertex3f(1280,720,0);
    glTexCoord2f(0,1); glVertex3f(0,720,0);
    glEnd();
    ofPopMatrix();
    liquifyS.end();
    tex1.unbind();
}
//--------------------------------------------------------------
void testApp::perlinLab()
{
    tex1.bind();
    perlinLAB.begin();
    perlinLAB.setUniformTexture("tex", tex1, 0);
    perlinLAB.setUniform1f("changingL", avgSound[0]*3);
    perlinLAB.setUniform1f("changingA", avgSound[1]*0.3);
    perlinLAB.setUniform1f("changingB", avgSound[2]*0.3);
    perlinLAB.setUniform1f("time", ofGetElapsedTimeMillis());
    
    ofPushMatrix();
    ofTranslate(0, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(0,0,0);
    glTexCoord2f(1,0); glVertex3f(1280,0,0);
    glTexCoord2f(1,1); glVertex3f(1280,720,0);
    glTexCoord2f(0,1); glVertex3f(0,720,0);
    glEnd();
    ofPopMatrix();
    perlinLAB.end();
    tex1.unbind();
}

//--------------------------------------------------------------
void testApp::perlinXyz()
{
    tex1.bind();
    perlinXYZ.begin();
    perlinXYZ.setUniformTexture("tex", tex1, 0);
    perlinXYZ.setUniform1f("changingA", avgSound[0]*3);
    perlinXYZ.setUniform1f("changingL", avgSound[1]*0.3);
    perlinXYZ.setUniform1f("changingB", avgSound[2]*0.3);
    perlinXYZ.setUniform1f("time", ofGetElapsedTimeMillis());
    
    ofPushMatrix();
    ofTranslate(0, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(0,0,0);
    glTexCoord2f(1,0); glVertex3f(1280,0,0);
    glTexCoord2f(1,1); glVertex3f(1280,720,0);
    glTexCoord2f(0,1); glVertex3f(0,720,0);
    glEnd();
    ofPopMatrix();
    perlinXYZ.end();
    tex1.unbind();
}

//--------------------------------------------------------------
void testApp::perlinHsv()
{
    tex1.bind();
    perlinHSV.begin();
    perlinHSV.setUniformTexture("tex", tex1, 0);
    perlinHSV.setUniform1f("changingA", avgSound[0]*3);
    perlinHSV.setUniform1f("changingL", avgSound[1]*0.3);
    perlinHSV.setUniform1f("changingB", avgSound[2]*0.3);
    perlinHSV.setUniform1f("time", ofGetElapsedTimeMillis());
    
    ofPushMatrix();
    ofTranslate(0, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(0,0,0);
    glTexCoord2f(1,0); glVertex3f(1280,0,0);
    glTexCoord2f(1,1); glVertex3f(1280,720,0);
    glTexCoord2f(0,1); glVertex3f(0,720,0);
    glEnd();
    ofPopMatrix();
    perlinHSV.end();
    tex1.unbind();
}

//--------------------------------------------------------------
void testApp::labFluctuate()
{
    tex1.bind();
    LABFluctuate.begin();
    LABFluctuate.setUniformTexture("tex", tex1, 0);
    LABFluctuate.setUniform1f("changingL", 0.5 + avgSound[0]*5);
    LABFluctuate.setUniform1f("changingA", avgSound[1]*5);
    LABFluctuate.setUniform1f("changingB", avgSound[2]*5);
    LABFluctuate.setUniform1f("time", ofGetElapsedTimeMillis());
    
    ofPushMatrix();
    ofTranslate(0, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(0,0,0);
    glTexCoord2f(1,0); glVertex3f(1280,0,0);
    glTexCoord2f(1,1); glVertex3f(1280,720,0);
    glTexCoord2f(0,1); glVertex3f(0,720,0);
    glEnd();
    ofPopMatrix();
    LABFluctuate.end();
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
    if(key == '6')
    {
        switchInt = 6;
    }
    if(key == '7')
    {
        switchInt = 7;
    }
    if(key == '8')
    {
        switchInt = 8;
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

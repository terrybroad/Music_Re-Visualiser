#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include <sys/time.h>

#include "maxiMFCC.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void posterize();
    void wobble();
    void rgbPosterize();
    void abberation();
    void wobbleAbberation();
    void liquifyRender();
    void liquifyRenderS();
    void perlinLab();
    void perlinXyz();
    void brightnessAlt();
    void perlinHsv();
    void labFluctuate();
    
    
        ofSoundPlayer chune;
        int nBandsToGet;
        float 				* fftSmoothed;
        float avgSound[3];
        float centroid;
    
        int camWidth, camHeight;
        ofVideoGrabber vidGrabber;
        ofTexture tex1;
        ofShader wobbleShader, testShader, empty,chromaShader,RGBPoster,chromaWobble, liquify, liquifyS, brightness,perlinLAB, perlinXYZ, perlinHSV, LABFluctuate;
        int switchInt;
   
    
	/* stick you maximilian stuff below */
    
    void audioRequested 	(float * input, int bufferSize, int nChannels); /* output method */
	void audioReceived 	(float * input, int bufferSize, int nChannels); /* input method */
    
	float 	* lAudioOut; /* outputs */
	float   * rAudioOut;
    
	float * lAudioIn; /* inputs */
	float * rAudioIn;
    
	int		initialBufferSize; /* buffer size */
	int		sampleRate;
        
	double wave,sample,outputs[2], ifftVal;
	maxiMix mymix;
	maxiOsc osc;
    
	ofxMaxiFFTOctaveAnalyzer oct;
	int nAverages;
	float *ifftOutput;
	int ifftSize;
    
	ofxMaxiIFFT ifft;
	ofxMaxiFFT mfft;
	int fftSize;
	int bins, dataSize;
    
	float callTime;
	timeval callTS, callEndTS;
    
	maxiMFCC mfcc;
	double *mfccs;
    
	maxiSample samp;
};

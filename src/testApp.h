#pragma once

#include "ofMain.h"

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
    
        ofSoundPlayer chune;
        int nBandsToGet;
        float 				* fftSmoothed;
        float avgSound[3];
    
        int camWidth, camHeight;
        ofVideoGrabber vidGrabber;
        ofTexture tex1;
        ofShader wobbleShader, testShader, empty,chromaShader,RGBPoster,chromaWobble, ripple;
        int switchInt;
};

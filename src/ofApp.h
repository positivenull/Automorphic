#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxSpout.h"
#include "ofxOsc.h"
#include "ofxFft.h"

#define PORT_CV 2200

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);

		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void initSound();
		void updateVol();
		void audioIn(ofSoundBuffer & input);

		void getOsc();
		void renderShaders();
		
		// base parameters
		int texWid = 1024;	int texHei = 1024;
		int fboWid = 1280;	int fboHei = 720;
		bool bSwitch = false;
		bool bTest = false;

		// sound parameters
		vector <float> left, right, volHistory;
		int 	bufferCounter, drawCounter;
		float	smoothedVol, scaledVol;
		float	usedVol; //tuba: adding a float to store the mapped out volume data
		ofSoundStream soundStream;

		// shaders parameters -- input
		vector< glm::vec2 > audience;
		vector< glm::vec2 > frequencies;
		glm::vec3 harmonies;
		float amplitude;
		// shaders parameters -- output
		int baseShader = 0;	int objsShader = 0;
		float gridSize, colorShift, blend, moveSpeed;

		// rendering objects
		ofFbo		baseFbo, maskFbo, blendFbo;
		ofShader	texShaders[7], blendShader;
		
		ofLight		dirLight;
		ofxAssimpModelLoader mLand, mObjs;

		// input & analysis
		ofxFft* fft;
		int bufferSize;
		ofxOscReceiver	cvOsc;

		// cameras and Spout output
		ofFbo		fbo1, fbo2;
		ofEasyCam   cam1, cam2;
		ofxSpout::Sender spOut1, spOut2;
};

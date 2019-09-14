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

		void getOsc();
		void renderShaders();
		
		// base parameters
		int texWid = 400;	int texHei = 400;
		int fboWid = 1280;	int fboHei = 720;

		// shaders parameters -- input
		vector< glm::vec2 > audience;
		vector< glm::vec2 > frequencies;
		glm::vec3 harmonies;
		float amplitude;
		// shaders parameters -- output
		int baseShader = 0;	int objsShader = 0;
		float gridSize, colorShift;
		glm::vec2 speed;

		// rendering objects
		ofFbo		baseFbo, objsFbo;
		ofShader	texShaders[5];
		
		ofLight		dirLight;
		ofxAssimpModelLoader model;

		// input & analysis
		ofxFft* fft;
		int bufferSize;
		ofxOscReceiver	cvOsc;

		// cameras and Spout output
		ofFbo		fbo1, fbo2;
		ofEasyCam   cam1, cam2;
		ofxSpout::Sender spOut1, spOut2;
};

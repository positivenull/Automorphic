#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxSpout.h"

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

		void renderShaders();
		ofFbo renderCamera(ofEasyCam cam);

		int texWid = 400;	int texHei = 400;
		int fboWid = 1280;	int fboHei = 720;

		ofFbo		matFbo;
		ofShader	colorShader;
		ofLight		dirLight;
		ofxAssimpModelLoader model;
		
		ofFbo		fbo1, fbo2;
		ofEasyCam   cam1, cam2;
		ofxSpout::Sender spOut1, spOut2;
};

#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

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

		ofMesh createSurface(int wid, int hei, int sep);
		int surfWid = 100;
		int surfHei = 100;
		int imgWid = 400;
		int imgHei = 400;

		ofFbo		matFbo;
		ofShader	colorShader;
		ofMesh		boxMesh, surfMesh;
		ofxAssimpModelLoader model;

		ofEasyCam    camera1;
};

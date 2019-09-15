#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofGLFWWindowSettings settings;
	settings.setSize(1280, 720);

	settings.title = "Automorphic";

	settings.setGLVersion(4, 1);
	//settings.setGLVersion(3, 3);
	ofCreateWindow(settings);

	ofRunApp( new ofApp() );
}

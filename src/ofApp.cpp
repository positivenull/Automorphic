#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofBackground(0);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
	ofSetSmoothLighting(true);
    ofDisableArbTex();
    ofEnableDepthTest();

    // init Fbo
	matFbo.allocate(texWid, texHei);
	fbo1.allocate(fboWid, fboHei);
	fbo2.allocate(fboWid, fboHei);

    // init shader
    colorShader.load("shaders/shader.vert", "shaders/basicColor.frag");

    // load model
	model.loadModel("shape.obj");

	// initialize spout sender
	spOut1.init("AutomorphicOfx_1");
	spOut2.init("AutomorphicOfx_2");

    // init camera
    cam1.setPosition(900, 900, 500);
    cam1.lookAt(ofVec3f(0, 0, 0));
	cam2.setPosition(-900, 900, 500);
	cam2.lookAt(ofVec3f(0, 0, 0));

	// init light
	dirLight.setDirectional();
	dirLight.setOrientation(ofVec3f(-1, -1, -1));
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	renderShaders();

	fbo1.begin();
	ofClear(0, 0);
    cam1.begin();
		dirLight.enable();
        matFbo.getTexture().bind();
        model.drawFaces();
        matFbo.getTexture().unbind();
		dirLight.disable();
    cam1.end();
	fbo1.end();

	fbo2.begin();
	ofClear(0, 0);
	cam2.begin();
		dirLight.enable();
		matFbo.getTexture().bind();
		model.drawFaces();
		matFbo.getTexture().unbind();
		dirLight.disable();
	cam2.end();
	fbo2.end();


    // matFbo.draw(0, 0);
	fbo1.draw(0, 0);

	// send picture via Spout
	spOut1.send(fbo1.getTexture());
	spOut2.send(fbo2.getTexture());
}


//--------------------------------------------------------------
void ofApp::renderShaders() {
	matFbo.begin();
	ofClear(0, 0);
		colorShader.begin();
		colorShader.setUniform1f("u_time", ofGetElapsedTimef());
		colorShader.setUniform2f("u_resolution", texWid, texHei);
		ofDrawRectangle(0, 0, texWid, texHei);
		colorShader.end();
	matFbo.end();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){}
void ofApp::mouseMoved(int x, int y){}
void ofApp::mouseDragged(int x, int y, int button){}
void ofApp::mousePressed(int x, int y, int button){}
void ofApp::mouseReleased(int x, int y, int button){}
void ofApp::mouseEntered(int x, int y){}
void ofApp::mouseExited(int x, int y){}
void ofApp::windowResized(int w, int h){}
void ofApp::gotMessage(ofMessage msg){}
void ofApp::dragEvent(ofDragInfo dragInfo){ }

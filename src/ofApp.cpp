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
	baseFbo.allocate(texWid, texHei);
	objsFbo.allocate(texWid, texHei);
	fbo1.allocate(fboWid, fboHei);
	fbo2.allocate(fboWid, fboHei);

    // init shader
	string vertex = "shaders/shader.vert";
    texShaders[0].load(vertex, "shaders/basicColor.frag");
    texShaders[1].load(vertex, "shaders/raymarch_alien.frag");
    texShaders[2].load(vertex, "shaders/colorWall.frag");
	texShaders[3].load(vertex, "shaders/clouds.frag");
	texShaders[4].load(vertex, "shaders/basicColor.frag");

    // load models
	model.loadModel("models/amph_landscape.obj");

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

	// init FFT
	bufferSize = 2048;
	fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING);
	ofSoundStreamSetup(0, 1, this, 44100, bufferSize, 4);

	// init OSC
	cvOsc.setup(PORT_CV);
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
        baseFbo.getTexture().bind();
        model.drawFaces();
        baseFbo.getTexture().unbind();
		dirLight.disable();
    cam1.end();
	fbo1.end();

	fbo2.begin();
	ofClear(0, 0);
	cam2.begin();
		dirLight.enable();
		baseFbo.getTexture().bind();
		model.drawFaces();
		baseFbo.getTexture().unbind();
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
	baseFbo.begin();
	ofClear(0, 0);
		texShaders[baseShader].begin();
		texShaders[baseShader].setUniform1f("u_time", ofGetElapsedTimef());
		texShaders[baseShader].setUniform2f("u_resolution", texWid, texHei);
		texShaders[baseShader].setUniform1f("u_gridSize", gridSize);
		texShaders[baseShader].setUniform2f("u_moveSpeed", speed.x, speed.y);
		texShaders[baseShader].setUniform1f("u_colorShift", colorShift);
		if(baseShader == 2){
			texShaders[baseShader].setUniform1f("u_moveSpeed", 2.);
			texShaders[baseShader].setUniform1f("u_colorSpeed", 1.);
			texShaders[baseShader].setUniform1f("u_colorWave", 0.);
			texShaders[baseShader].setUniform1f("u_intensity", 1.);
		}
		ofDrawRectangle(0, 0, texWid, texHei);
		texShaders[baseShader].end();
	baseFbo.end();

	objsFbo.begin();
	ofClear(0, 0);
		texShaders[baseShader].begin();
		texShaders[baseShader].setUniform1f("u_time", ofGetElapsedTimef());
		texShaders[baseShader].setUniform2f("u_resolution", texWid, texHei);
		if (baseShader == 2) {
			texShaders[baseShader].setUniform1f("u_moveSpeed", 2.);
			texShaders[baseShader].setUniform1f("u_colorSpeed", 1.);
			texShaders[baseShader].setUniform1f("u_colorWave", 0.);
			texShaders[baseShader].setUniform1f("u_intensity", 1.);
		}
		ofDrawRectangle(0, 0, texWid, texHei);
		texShaders[baseShader].end();
	objsFbo.end();
}


void ofApp::getOsc() {
	while (cvOsc.hasWaitingMessages()) {
		ofxOscMessage m;
		cvOsc.getNextMessage(m);

		if (m.getAddress() == "") {
			float a = m.getArgAsFloat(0);
		}
	}
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case '0':
			baseShader = 0;
			break;
		case '1':
			baseShader = 1;
			break;
		case '2':
			baseShader = 2;
			break;
		case '3':
			baseShader = 3;
			break;
		case '4':
			baseShader = 4;
			break;
	}
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

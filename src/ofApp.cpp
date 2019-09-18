#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(30);
    ofBackground(0);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
	ofSetSmoothLighting(true);
    ofEnableDepthTest();
    ofDisableArbTex();

    // init Fbo
	baseFbo.allocate (texWid, texHei, GL_RGBA);
	maskFbo.allocate (texWid, texHei, GL_RGBA);
	blendFbo.allocate(texWid, texHei, GL_RGBA);
	fbo1.allocate(fboWid, fboHei, GL_RGBA);
	fbo2.allocate(fboWid, fboHei, GL_RGBA);

    // init shader
	string vertex = "shaders/shader.vert";
    texShaders[0].load(vertex, "shaders/basicColor.frag");
    texShaders[1].load(vertex, "shaders/raymarch_alien.frag");
    texShaders[2].load(vertex, "shaders/colorWall.frag");
	texShaders[3].load(vertex, "shaders/clouds.frag");
	texShaders[4].load(vertex, "shaders/voronoi.frag");
	texShaders[5].load(vertex, "shaders/px_quascrystal.frag");
	texShaders[6].load(vertex, "shaders/px_smokeCryst.frag");
	blendShader.load  (vertex, "shaders/blend.frag");

    
    // init camera
    cam1.setPosition(900, 900, 500);
    cam1.lookAt(ofVec3f(0, 0, 0));
	cam2.setPosition(-900, 900, 500);
	cam2.lookAt(ofVec3f(0, 0, 0));

	// init light
	dirLight.setDirectional();
	dirLight.setOrientation(ofVec3f(-1, -1, -1));

	// load models
	mLand.loadModel("models/amph_landscape.obj");
	mObjs.loadModel("models/amph_objects.obj");
	mObjs.setScale(0.9, 0.9, 0.9);

	// init parameters
	gridSize = 4;
	colorShift = 0;
	moveSpeed  = 1;
	blend = 0;

	// init FFT
	bufferSize = 2048;
	fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING);
	ofSoundStreamSetup(0, 1, this, 44100, bufferSize, 4);

	// initialize spout sender
	spOut1.init("AutomorphicOfx_1");
	spOut2.init("AutomorphicOfx_2");

	// init OSC
	cvOsc.setup(PORT_CV);
}

//--------------------------------------------------------------
void ofApp::update(){
	updateVol();
	getOsc();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofDisableDepthTest();
	renderShaders();

	ofEnableDepthTest();
	fbo1.begin();
	ofClear(0, 0);
    cam1.begin();
		dirLight.enable();
		// -- draw landscape
		baseFbo.getTexture().bind();
		mLand.drawFaces();
		baseFbo.getTexture().unbind();
		// -- draw objects
		blendFbo.getTexture().bind();
		mObjs.drawFaces();
		blendFbo.getTexture().unbind();
		dirLight.disable();
	cam1.end();
	fbo1.end();

	fbo2.begin();
	ofClear(0, 0);
	cam2.begin();
		dirLight.enable();
		// -- draw landscape
		baseFbo.getTexture().bind();
		mLand.drawFaces();
		baseFbo.getTexture().unbind();
		// -- draw objects
		blendFbo.getTexture().bind();
		mObjs.drawFaces();
		blendFbo.getTexture().unbind();
		dirLight.disable();
	cam2.end();
	fbo2.end();


    // matFbo.draw(0, 0);
	fbo1.draw(0, 0);

	if (bTest) { blendFbo.draw(0, 0); }
	else {}

	// send picture via Spout
	spOut1.send(fbo1.getTexture());
	spOut2.send(fbo2.getTexture());

	// show settings
	//spOut1.send(blendFbo.getTexture());
	//spOut2.send(baseFbo.getTexture());

}



//--------------------------------------------------------------
void ofApp::initSound() {
	soundStream.printDeviceList();

	int bufferSize = 256;

	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	volHistory.assign(400, 0.0);

	bufferCounter = 0;
	drawCounter = 0;
	smoothedVol = 0.0;
	scaledVol = 0.0;

	ofSoundStreamSettings soundSettings;

	auto devices = soundStream.getMatchingDevices("Default");
	if (!devices.empty()) {
		soundSettings.setInDevice(devices[0]);
	}

	soundSettings.setInListener(this);
	soundSettings.sampleRate = 44100;
	soundSettings.numOutputChannels = 0;
	soundSettings.numInputChannels = 2;
	soundSettings.bufferSize = bufferSize;
	soundStream.setup(soundSettings);
}
void ofApp::updateVol() {
	//lets scale the vol up to a 0-1 range 
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

	//lets record the volume into an array
	volHistory.push_back(scaledVol);

	//if we are bigger the the size we want to record - lets drop the oldest value
	if (volHistory.size() >= 400) {
		volHistory.erase(volHistory.begin(), volHistory.begin() + 1);
	}
}
void ofApp::audioIn(ofSoundBuffer & input) {
	float curVol = 0.0;

	// samples are "interleaved"
	int numCounted = 0;

	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
	for (size_t i = 0; i < input.getNumFrames(); i++) {
		left[i] = input[i * 2] * 0.5;
		right[i] = input[i * 2 + 1] * 0.5;

		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted += 2;
	}

	//this is how we get the mean of rms :) 
	curVol /= (float)numCounted;

	// this is how we get the root of rms :) 
	curVol = sqrt(curVol);

	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;

	bufferCounter++;
}

//--------------------------------------------------------------
void ofApp::renderShaders() {
	// temporary FBOs
	ofFbo	base1, base2, land1, land2;
	ofFbo	blend_o1, blend_o2, blend_l1, blend_l2;
	base1.allocate(texWid, texHei, GL_RGBA);
	base2.allocate(texWid, texHei, GL_RGBA);
	land1.allocate(texWid, texHei, GL_RGBA);
	land2.allocate(texWid, texHei, GL_RGBA);
	blend_o1.allocate(texWid, texHei, GL_RGBA);
	blend_o2.allocate(texWid, texHei, GL_RGBA);
	blend_l1.allocate(texWid, texHei, GL_RGBA);
	blend_l2.allocate(texWid, texHei, GL_RGBA);

	base1.begin();
	ofClear(0, 0);
		texShaders[2].begin();
		texShaders[2].setUniform1f("u_time", ofGetElapsedTimef());
		texShaders[2].setUniform2f("u_resolution", texWid, texHei);
		texShaders[2].setUniform1f("u_gridSize", gridSize);
		texShaders[2].setUniform1f("u_moveSpeed", moveSpeed);
		texShaders[2].setUniform1f("u_colorSpeed", 1.);
		texShaders[2].setUniform1f("u_colorWave", 0.);
		texShaders[2].setUniform1f("u_intensity", 1.);
		ofDrawRectangle(0, 0, texWid, texHei);
		texShaders[2].end();
	base1.end();

	base2.begin();
	ofClear(0, 0);
		texShaders[4].begin();
		texShaders[4].setUniform1f("u_time", ofGetElapsedTimef());
		texShaders[4].setUniform2f("u_resolution", texWid, texHei);
		texShaders[4].setUniform1f("u_gridSize", gridSize);
		texShaders[4].setUniform1f("u_moveSpeed", moveSpeed);
		texShaders[4].setUniform1f("u_colorSpeed", 1.);
		texShaders[4].setUniform1f("u_colorWave", 0.);
		texShaders[4].setUniform1f("u_intensity", 1.);
		ofDrawRectangle(0, 0, texWid, texHei);
		texShaders[4].end();
	base2.end();

	land1.begin();
	ofClear(0, 0);
		texShaders[3].begin();
		texShaders[3].setUniform1f("u_time", ofGetElapsedTimef());
		texShaders[3].setUniform2f("u_resolution", texWid, texHei);
		texShaders[3].setUniform1f("u_gridSize", gridSize);
		texShaders[3].setUniform1f("u_moveSpeed", moveSpeed);
		texShaders[3].setUniform1f("u_colorShift", colorShift);
		ofDrawRectangle(0, 0, texWid, texHei);
		texShaders[3].end();
	land1.end();

	land2.begin();
	ofClear(0, 0);
		texShaders[1].begin();
		texShaders[1].setUniform1f("u_time", ofGetElapsedTimef());
		texShaders[1].setUniform2f("u_resolution", texWid, texHei);
		texShaders[1].setUniform1f("u_gridSize", gridSize);
		texShaders[1].setUniform1f("u_moveSpeed", moveSpeed);
		texShaders[1].setUniform1f("u_colorShift", colorShift);
		ofDrawRectangle(0, 0, texWid, texHei);
		texShaders[1].end();
	land2.end();

	// -- mask shader
	maskFbo.begin();
	ofClear(0, 0);
		texShaders[5].begin();
		texShaders[5].setUniform1f("u_time", ofGetElapsedTimef());
		texShaders[5].setUniform2f("u_resolution", texWid, texHei);
		texShaders[5].setUniform1f("u_gridSize", gridSize);
		ofDrawRectangle(0, 0, texWid, texHei);
		texShaders[5].end();
	maskFbo.end();

	// -- blend shaders
	float mixer = abs(blend);
	// ----- texture, objs part 1
	blend_o1.begin();
	ofClear(0, 0);
		blendShader.begin();
		blendShader.setUniformTexture("inTex", base1.getTexture(), 0);
		blendShader.setUniformTexture("maskTex", maskFbo.getTextureReference(0), 1);
		blendShader.setUniform1f("u_time", ofGetElapsedTimef());
		blendShader.setUniform2f("u_resolution", texWid, texHei);
		blendShader.setUniform1f("u_blend", mixer);
		blendShader.setUniform1i("u_invert", 0);
		ofDrawRectangle(0, 0, texWid, texHei);
		blendShader.end();
	blend_o1.end();
	// ----- texture, objs part 2
	blend_o2.begin();
	ofClear(0, 0);
		blendShader.begin();
		blendShader.setUniformTexture("inTex", base2.getTexture(), 0);
		blendShader.setUniformTexture("maskTex", maskFbo.getTextureReference(0), 1);
		blendShader.setUniform1f("u_time", ofGetElapsedTimef());
		blendShader.setUniform2f("u_resolution", texWid, texHei);
		blendShader.setUniform1f("u_blend", mixer);
		blendShader.setUniform1i("u_invert", 1);
		ofDrawRectangle(0, 0, texWid, texHei);
		blendShader.end();
	blend_o2.end();
	// ----- texture part 1
	blend_l1.begin();
	ofClear(0, 0);
		blendShader.begin();
		blendShader.setUniformTexture("inTex", land1.getTexture(), 0);
		blendShader.setUniformTexture("maskTex", maskFbo.getTextureReference(0), 1);
		blendShader.setUniform1f("u_time", ofGetElapsedTimef());
		blendShader.setUniform2f("u_resolution", texWid, texHei);
		blendShader.setUniform1f("u_blend", mixer);
		blendShader.setUniform1i("u_invert", 0);
		ofDrawRectangle(0, 0, texWid, texHei);
		blendShader.end();
	blend_l1.end();
	// ----- texture part 2
	blend_l2.begin();
	ofClear(0, 0);
		blendShader.begin();
		blendShader.setUniformTexture("inTex", land2.getTexture(), 0);
		blendShader.setUniformTexture("maskTex", maskFbo.getTextureReference(0), 1);
		blendShader.setUniform1f("u_time", ofGetElapsedTimef());
		blendShader.setUniform2f("u_resolution", texWid, texHei);
		blendShader.setUniform1f("u_blend", mixer);
		blendShader.setUniform1i("u_invert", 1);
		ofDrawRectangle(0, 0, texWid, texHei);
		blendShader.end();
	blend_l2.end();

	// ----- final mix
	blendFbo.begin();
	ofClear(0, 0);
		blend_o1.draw(0, 0);
		blend_o2.draw(0, 0);
	blendFbo.end();

	baseFbo.begin();
	ofClear(0, 0);
		blend_l1.draw(0, 0);
		blend_l2.draw(0, 0);
	baseFbo.end();
}


//--------------------------------------------------------------
void ofApp::getOsc() {
	while (cvOsc.hasWaitingMessages()) {
		// get OSC message
		ofxOscMessage m;
		cvOsc.getNextMessage(m);

		if (m.getAddress() == "/cv/audience") {
			// get how many people were captured on frame
			float amount = m.getArgAsFloat(0);
			// if we have any captured audience
			if (amount > 0) {
				// flush the current audience 
				audience.clear();
				for (size_t i = 0; i < amount; i++) {
					// get x, y position and create vector
					float x = m.getArgAsFloat(2 * i + 1);
					float y = m.getArgAsFloat(2 * i + 2);
					glm::vec2 pos = glm::vec2(x, y);
					// and add to the vector
					audience.push_back(pos);
				}
			}
			// update grid size
			gridSize = 1.5 + amount / 20 * 5;
		}
		else if (m.getAddress() == "/control/blend") {
			//blend = m.getArgAsFloat(0)*2 - 1;
			blend = m.getArgAsFloat(0);
			// keep blend in range [-1,1]
			if (blend < -1) { blend = -1; }
			else if (blend > 1) { blend = 1; }
		}
		else if (m.getAddress() == "/control/gridSize") {
			gridSize = m.getArgAsFloat(0);
		}
		else if (m.getAddress() == "/control/moveSpeed") {
			moveSpeed = m.getArgAsFloat(0);
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
		case '5':
			baseShader = 5;
			break;
		case '6':
			baseShader = 6;
			break;
		case 'z':
			bSwitch = !bSwitch;
			break;
		case 'x':
			bTest = !bTest;
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

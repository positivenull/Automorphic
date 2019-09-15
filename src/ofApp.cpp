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
	base1.allocate   (texWid, texHei, GL_RGBA);
	base2.allocate   (texWid, texHei, GL_RGBA);
	objsFbo.allocate (texWid, texHei, GL_RGBA);
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
	model.loadModel("models/amph_landscape.obj");


	// init parameters
	gridSize = 4;
	colorShift = 0;
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
	getOsc();
}

//--------------------------------------------------------------
void ofApp::draw(){
	renderShaders();
	//shadersBlend();

	fbo1.begin();
	ofClear(0, 0);
    cam1.begin();
		dirLight.enable();
		blendFbo.getTexture().bind();
        model.drawFaces();
		blendFbo.getTexture().unbind();
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
		if(baseShader == 2 || baseShader == 4){
			texShaders[baseShader].setUniform1f("u_moveSpeed", 2.);
			texShaders[baseShader].setUniform1f("u_colorSpeed", 1.);
			texShaders[baseShader].setUniform1f("u_colorWave", 0.);
			texShaders[baseShader].setUniform1f("u_intensity", 1.);
		}
		ofDrawRectangle(0, 0, texWid, texHei);
		texShaders[baseShader].end();
	baseFbo.end();

	base1.begin();
	ofClear(0, 0);
		texShaders[2].begin();
		texShaders[2].setUniform1f("u_time", ofGetElapsedTimef());
		texShaders[2].setUniform2f("u_resolution", texWid, texHei);
		texShaders[2].setUniform1f("u_gridSize", gridSize);
		texShaders[2].setUniform2f("u_moveSpeed", speed.x, speed.y);
		texShaders[2].setUniform1f("u_colorShift", colorShift);
		texShaders[2].setUniform1f("u_moveSpeed", 2.);
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
		texShaders[4].setUniform2f("u_moveSpeed", speed.x, speed.y);
		texShaders[4].setUniform1f("u_colorShift", colorShift);
		texShaders[4].setUniform1f("u_moveSpeed", 2.);
		texShaders[4].setUniform1f("u_colorSpeed", 1.);
		texShaders[4].setUniform1f("u_colorWave", 0.);
		texShaders[4].setUniform1f("u_intensity", 1.);
		ofDrawRectangle(0, 0, texWid, texHei);
		texShaders[4].end();
	base2.end();

	maskFbo.begin();
	ofClear(0, 0);
		texShaders[5].begin();
		texShaders[5].setUniform1f("u_time", ofGetElapsedTimef());
		texShaders[5].setUniform2f("u_resolution", texWid, texHei);
		texShaders[5].setUniform1f("u_gridSize", gridSize);
		ofDrawRectangle(0, 0, texWid, texHei);
		texShaders[5].end();
	maskFbo.end();

	objsFbo.begin();
	ofClear(0, 0);
		texShaders[baseShader].begin();
		texShaders[baseShader].setUniform1f("u_time", ofGetElapsedTimef());
		texShaders[baseShader].setUniform2f("u_resolution", texWid, texHei);
		if (baseShader == 2 || baseShader == 4) {
			texShaders[baseShader].setUniform1f("u_moveSpeed", 2.);
			texShaders[baseShader].setUniform1f("u_colorSpeed", 1.);
			texShaders[baseShader].setUniform1f("u_colorWave", 0.);
			texShaders[baseShader].setUniform1f("u_intensity", 1.);
		}
		ofDrawRectangle(0, 0, texWid, texHei);
		texShaders[baseShader].end();
	objsFbo.end();

	blendFbo.begin();
	ofClear(0, 0);
		float mixer = abs(blend);

		// texture part 1
		blendShader.begin();
		blendShader.setUniformTexture("inTex",   base1.getTexture(), 0);
		blendShader.setUniformTexture("maskTex", maskFbo.getTextureReference(0), 1);
		blendShader.setUniform1f("u_time", ofGetElapsedTimef());
		blendShader.setUniform2f("u_resolution", texWid, texHei);
		blendShader.setUniform1f("u_blend", mixer);
		blendShader.setUniform1i("u_invert", 0);
		ofDrawRectangle(0, 0, texWid, texHei);
		blendShader.end();

		// texture part 2
		blendShader.begin();
		blendShader.setUniformTexture("inTex", base2.getTexture(), 0);
		blendShader.setUniformTexture("maskTex", maskFbo.getTextureReference(0), 1);
		blendShader.setUniform1f("u_time", ofGetElapsedTimef());
		blendShader.setUniform2f("u_resolution", texWid, texHei);
		blendShader.setUniform1f("u_blend", mixer);
		blendShader.setUniform1i("u_invert", 1);
		ofDrawRectangle(0, 0, texWid, texHei);
		blendShader.end();
	blendFbo.end();
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
			blend = m.getArgAsFloat(0)*2 - 1;
			// keep blend in range [-1,1]
			if (blend < -1) { blend = -1; }
			else if (blend > 1) { blend = 1; }
		}
		else if (m.getAddress() == "/control/gridSize") {
			gridSize = m.getArgAsFloat(0);
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

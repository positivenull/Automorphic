#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofBackground(0);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofDisableArbTex();
    ofEnableDepthTest();

    // init Fbo
    matFbo.allocate(imgWid, imgHei);

    // init shader
    colorShader.load("shaders/shader.vert", "shaders/basicColor.frag");

    // init Mesh
    boxMesh = ofMesh::box(200, 200, 200);
    surfMesh = createSurface(surfWid, surfHei, 1);
    surfMesh.flatNormals();

    // load model
	model.loadModel("shape.obj");


    // init camera
    // camera1 = ofCamera();
    camera1.setPosition(500, 500, -300);
    camera1.lookAt(ofVec3f(0, 0, 0));
}

//--------------------------------------------------------------
void ofApp::update(){
    float t = ofGetElapsedTimef();

    // surface distortion
    // go through vertices
    // for (size_t x = 0; x < surfWid; x++) {
    //     for (size_t y = 0; y < surfHei; y++) {
    //         // get vertex
    //         int ind = x + y * surfWid;
    //         ofPoint p = surfMesh.getVertex(ind);
    //         // calculate perlin noise
    //         float noise = ofNoise(x*0.05, y*0.05, t*0.1);
    //         // replace the vertex
    //         surfMesh.setVertex(ind, ofPoint(p.x, noise * 50, p.z));
    //     }
    // }
    // surfMesh.flatNormals();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0, 0, 0, 1.);

    matFbo.begin();
    ofClear(0, 0);
    colorShader.begin();
    colorShader.setUniform1f("u_time", ofGetElapsedTimef());
    colorShader.setUniform2f("u_resolution", imgWid, imgHei);
    ofDrawRectangle(0, 0, imgWid, imgHei);
    colorShader.end();
    matFbo.end();

    camera1.begin();
    ofPushMatrix();
        matFbo.getTexture().bind();
        // ofRotate(ofGetElapsedTimef()*4., 0.3, 0.5, 0.2);
        // boxMesh.draw();
        // surfMesh.draw();
        model.setPosition(ofGetWidth()/2, (float)ofGetHeight() * 0.75 , 0);
        model.drawFaces();
        matFbo.getTexture().unbind();
    ofPopMatrix();
    camera1.end();

    matFbo.draw(0, 0);
}


//--------------------------------------------------------------
ofMesh ofApp::createSurface(int wid, int hei, int step){
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    // add vertices
    for (size_t i = 0; i < wid; i++) {
        for (size_t j = 0; j < hei; j++) {
            float x = (i - wid/2.) * step;
            float y = (j - hei/2.) * step;
            float noise = ofNoise(x*0.02, y*0.02, 0.);
            mesh.addVertex( ofPoint(x, noise*40., y) );
            // texCoords
            float tX = -1 + 2*i/wid;
            float tY = -1 + 2*j/hei;
            mesh.addTexCoord( ofVec2f(tX, tY) );
            // mesh.addTexCoord( ofVec2f(x/wid, y/hei) );
            // mesh.addTexCoord( ofVec2f(i/wid, j/hei) );

            // mesh.addColor( ofColor( noise*200. ) );
        }
    }

    // mesh.addTexCoord( ofVec2f(-1, -1) );
    // mesh.addTexCoord( ofVec2f( 1, -1) );
    // mesh.addTexCoord( ofVec2f( 1,  1) );
    // mesh.addTexCoord( ofVec2f(-1,  1) );
    // mesh.addTexCoord( ofVec2f(-1, -1) );

    // // assign triangle faces
    // for (size_t x = 0; x < wid-1; x++) {
    //     for (size_t y = 0; y < hei-1; y++) {
    //         // get the vertices
    //         int v1 = x   + (y)   * wid;
    //         int v2 = x+1 + (y)   * wid;
    //         int v3 = x   + (y+1) * wid;
    //         int v4 = x+1 + (y+1) * wid;
    //         mesh.addTriangle( v1, v2, v3 );
    //         mesh.addTriangle( v2, v4, v3 );
    //     }
    // }

    // assign indices
    for (size_t x = 0; x < wid-1; x++) {
        for (size_t y = 0; y < hei-1; y++) {
            mesh.addIndex( x   + (y)   * wid );
            mesh.addIndex( x+1 + (y)   * wid );
            mesh.addIndex( x   + (y+1) * wid );

            mesh.addIndex( x+1 + (y)   * wid );
            mesh.addIndex( x+1 + (y+1) * wid );
            mesh.addIndex( x   + (y+1) * wid );
        }
    }

    return mesh;
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

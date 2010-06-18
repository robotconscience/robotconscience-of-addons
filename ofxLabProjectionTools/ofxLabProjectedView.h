/*
 *  ofxLabProjectedView.h
 *
 *  Created by Brett Renfer on 3/16/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */
/*
 
 Based in part off of ofxTouchGraphicsWarp:
 
 * Copyright 2008 NOR_/D <http://nortd.com>
 *
 * This file is part of ofxTouch.
 * ofxTouch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as 
 * published by the Free Software Foundation, either version 3 of 
 * the License, or (at your option) any later version. For details
 * see <http://www.gnu.org/licenses/>.
 *
 * * *
 * This class handles perspective warping of the entire OpenGL world.
 *
 */

#pragma once
#include "ofxFBOTexture.h"
#include "ofxXmlSettings.h"

class OverlayImage : public ofRectangle
{
public:
	ofImage image;
	float rotation;
	ofxFBOTexture texture;
	int imageWidth, imageHeight;
	float baseX, baseY;
	
	OverlayImage(){ 
		rotation = 0; 
		baseX = 0;
		baseY = 0;
		//width = height = baseX = baseY = x = y = 0;
	};
	void loadImage( string img, int _imageWidth, int _imageHeight ){
		imageWidth = _imageWidth;
		imageHeight = _imageHeight;
		image.loadImage(img);
		texture.allocate(imageWidth, imageHeight, true);
	}
	
	void render(){
		texture.clear(0,0,0,0);
		texture.begin();
		ofPushMatrix();
		ofTranslate(baseX, baseY);
		ofRotateZ(rotation);
		//ofTranslate(-width/2,0);
		image.draw(x,y,width, height);
		ofPopMatrix();
		texture.end();
	}
};

class ofxLabProjectedView
{
public:
	
	int index;
	ofPoint corners[4];
	bool selected[4];
    int whichCorner;
	//int selected;
	bool bDrawDebug;
	float x, y, width, height;
	ofxFBOTexture texture;
	int selectedPoint;
	
	ofxXmlSettings settings;
	OverlayImage overlays[4];
	
//----------------------------------------------------------------------------
	
	ofxLabProjectedView( int _index, float _x, float _y, float _width, float _height):
	index(_index),
	x(_x), 
	y(_y),
	width(_width), 
	height(_height)
	{
		corners[0].x = x;
		corners[0].y = y;
		corners[1].x = x + width;
		corners[1].y = y;
		corners[2].x = x + width;
		corners[2].y = y + height;
		corners[3].x = x;
		corners[3].y = y + height;
		
		//loadSettings();
		texture.allocate(width, height, true);
		
		bDrawDebug = false;
		selectedPoint  = -1;
		for (int j=0; j<4; j++){
			selected[j] = false;
		}
		
		for (int i=0; i<4; i++){
			overlays[i].loadImage("overlay/overlay.png", width, height);
			overlays[i].width = width;		
			overlays[i].height = 10;
		}
		
		overlays[0].baseX = width;
		overlays[0].baseY = height;
		overlays[0].x = width;
		overlays[0].y = height;
		overlays[0].width = width;		
		overlays[0].height = 10;
		overlays[0].rotation = 180;
		
		overlays[1].baseX = width;
		overlays[1].baseY = 0;
		overlays[1].x = width;
		overlays[1].y = 0;
		overlays[1].width = height;	
		overlays[1].rotation = 90;
		
		overlays[2].baseX = 0;
		overlays[2].baseY = 0;
		overlays[2].x = 0;
		overlays[2].y = 0;
		overlays[2].rotation = 0;
		
		overlays[3].baseX = 0;
		overlays[3].baseY = height;
		overlays[3].x = 0;
		overlays[3].y = height;
		overlays[3].width = height;
		overlays[3].rotation = 270;
		
		loadSettings();
		
	};
	
//----------------------------------------------------------------------------
	
	void draw(){		
		texture.bind();
		glBegin(GL_QUADS);		
		glTexCoord2f(0,0);
		glVertex2f(corners[0].x, corners[0].y);		
		glTexCoord2f(width,0);
		glVertex2f(corners[1].x, corners[1].y);		
		glTexCoord2f(width,height);
		glVertex2f(corners[2].x, corners[2].y);		
		glTexCoord2f(0,height);
		glVertex2f(corners[3].x, corners[3].y);
		glEnd(); 
		texture.unbind();
		
		ofEnableAlphaBlending();
		
		//render overlay fbo
		for (int i=0; i<4; i++){
			overlays[i].render();			
			overlays[i].texture.bind();
			glBegin(GL_QUADS);		
			glTexCoord2f(0,0);
			glVertex2f(corners[0].x, corners[0].y);		
			glTexCoord2f(width,0);
			glVertex2f(corners[1].x, corners[1].y);		
			glTexCoord2f(width,height);
			glVertex2f(corners[2].x, corners[2].y);		
			glTexCoord2f(0,height);
			glVertex2f(corners[3].x, corners[3].y);
			glEnd(); 
			overlays[i].texture.unbind();
		}		
		
		//texture.draw(x, y);
		
		if (bDrawDebug){
			ofPushStyle();
			ofSetColor(255,0,0);
			ofRect(corners[0].x, corners[0].y, 5, 5);
			ofSetColor(0,255,0);
			ofRect(corners[1].x, corners[1].y, 5, 5);
			ofSetColor(0,0,255);
			ofRect(corners[2].x, corners[2].y, 5, 5);
			ofSetColor(255,255,0);
			ofRect(corners[3].x, corners[3].y, 5, 5);
			ofPopStyle();
		}
	};
	
	
//----------------------------------------------------------------------------
	
	void beginDraw(){
		texture.clear();
		texture.begin();
	}
	
//----------------------------------------------------------------------------
	
	void endDraw(){
		texture.end();
	}
	
	
//----------------------------------------------------------------------------
	
	void drawDebug( int debug=true ){
		bDrawDebug = debug;
	}
	
//----------------------------------------------------------------------------

	void mousePressed( int mX, int mY ){
		int hitThreshold = 10;
		selectedPoint  = -1;	
		
		float storeDist = 9999999.0;
		
		for (int j=0; j<4; j++){
			selected[j] = false;
		}
		
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT){				
//			float verticesX[4];
//			verticesX[0] = quads[i].p1.x;
//			verticesX[1] = quads[i].p2.x;
//			verticesX[2] = quads[i].p3.x;
//			verticesX[3] = quads[i].p4.x;
//			
//			float verticesY[4];
//			verticesY[0] = quads[i].p1.y;
//			verticesY[1] = quads[i].p2.y;
//			verticesY[2] = quads[i].p3.y;
//			verticesY[3] = quads[i].p4.y;
			
			//quads[i].pressed = pnpoly(4, verticesX, verticesY, mX, mY);
		} else {			
			float dx1 = fabs(mX -  corners[0].x);
			float dy1 = fabs(mY -  corners[0].y);
			float dx2 = fabs(mX -  corners[1].x);
			float dy2 = fabs(mY -  corners[1].y);
			float dx3 = fabs(mX -  corners[2].x);
			float dy3 = fabs(mY -  corners[2].y);
			float dx4 = fabs(mX -  corners[3].x);
			float dy4 = fabs(mY -  corners[3].y);
			
			float dist1 = sqrt(dx1*dx1 + dy1*dy1);
			float dist2 = sqrt(dx2*dx2 + dy2*dy2);
			float dist3 = sqrt(dx3*dx3 + dy3*dy3);
			float dist4 = sqrt(dx4*dx4 + dy4*dy4);
			
			if(dist1 < storeDist && dist1 < hitThreshold){
				selectedPoint = 0;
				storeDist = dist1;
			}
			
			if(dist2 < storeDist && dist2 < hitThreshold){
				selectedPoint = 1;
				storeDist = dist2;
			}
			
			if(dist3 < storeDist && dist3 < hitThreshold){
				selectedPoint = 2;
				storeDist = dist3;
			}
			
			if(dist4 < storeDist && dist4 < hitThreshold){
				selectedPoint = 3;
				storeDist = dist4;
			}      
			
		}
		
		if (selectedPoint != -1){
			selected[selectedPoint] = true;
		}
	}
	
	
//----------------------------------------------------------------------------
	
	void mouseDragged( int mx, int my ){
		if (selectedPoint == -1) return;
		
		for (int i=0; i<4; i++){
			if (selected[i]){
				corners[i].x = mx;
				corners[i].y = my;
			}
		}
	}
	
//----------------------------------------------------------------------------
	
	void mouseReleased(){
		for (int j=0; j<4; j++){
			selected[j] = false;
		}
		saveSettings();
	}	
	
//----------------------------------------------------------------------------
	
	void loadSettings(){
		settings.loadFile(ofToDataPath("settings.xml"));		
		bool found = settings.pushTag("settings",0);		
		if (!found) return;
		
		for (int i=0; i<settings.getNumTags("quad"); i++){
			settings.pushTag("quad", i);
			
			stringstream intStream;
			intStream<<index;
			
			if (settings.getValue("name", "", 0) == intStream.str()){			
				cout << "found" << endl;
				corners[0].x = settings.getValue("x", 0, 0);
				corners[0].y = settings.getValue("y", 0, 0);
				corners[1].x = settings.getValue("x", 0, 1);
				corners[1].y = settings.getValue("y", 0, 1);
				corners[2].x = settings.getValue("x", 0, 2);
				corners[2].y = settings.getValue("y", 0, 2);
				corners[3].x = settings.getValue("x", 0, 3);
				corners[3].y = settings.getValue("y", 0, 3);
			}
			settings.popTag();
		}
		settings.popTag();
	};
	
//----------------------------------------------------------------------------

	void saveSettings(){
		settings.loadFile(ofToDataPath("settings.xml", true));
		
		if (!settings.tagExists("settings", 0)){
			settings.addTag("settings");
		}
		
		settings.pushTag("settings",0);
		
		bool found = false;
		stringstream intStream;
		intStream<<index;
		
		for (int i=0; i<settings.getNumTags("quad"); i++){
			settings.pushTag("quad", i);
						
			if (settings.getValue("name", "", 0) == intStream.str()){
				found = true;
			} else {
				settings.popTag();
			}
		}
		
		
		if (!found){
			settings.addTag("quad");
			settings.pushTag("quad", settings.getNumTags("quad")-1);
		}
		
		settings.setValue("name", intStream.str(),0);
		settings.setValue("x", corners[0].x,0);
		settings.setValue("y", corners[0].y,0);		
		settings.setValue("x", corners[1].x,1);
		settings.setValue("y", corners[1].y,1);		
		settings.setValue("x", corners[2].x,2);
		settings.setValue("y", corners[2].y,2);		
		settings.setValue("x", corners[3].x,3);
		settings.setValue("y", corners[3].y,3);		
		settings.popTag();
		
		settings.popTag();
		settings.saveFile(ofToDataPath("settings.xml", true));
	};
	
};


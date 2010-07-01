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
	
	bool bHasImage;
	
	OverlayImage(){ 
		rotation = 0; 
		baseX = 0;
		baseY = 0;
		bHasImage = false;
		//width = height = baseX = baseY = x = y = 0;
	};
	
	void loadImage( string img, int _imageWidth, int _imageHeight ){
		imageWidth = _imageWidth;
		imageHeight = _imageHeight;
		bHasImage = image.loadImage(img);
		if (bHasImage) texture.allocate(imageWidth, imageHeight, true);
	};
	
	void render(){
		texture.clear(0,0,0,0);
		texture.begin();
		ofPushMatrix();{
			ofTranslate(baseX, baseY);
			ofRotateZ(rotation);
			//ofTranslate(-width/2,0);
			if (bHasImage){
				image.draw(x,y,width, height);
			} else {
				ofSetColor(0,0,0);
				ofRect(x,y,width,height);
			}
		}ofPopMatrix();
		texture.end();
	};
};

class ofxLabProjectedView
{
public:
	
	int index;
	ofPoint skewCorners[4];
	bool selected[4];
	int selectedPoint;
	
	ofPoint cropCorners[4];
	bool bCropSelected[4];
	int selectedCropPoint;
	
	//int selected;
	bool bDrawSkew;
	bool bDrawCrop;
	
	ofRectangle resolution;
	
	float x, y, width, height;
	ofxFBOTexture texture;
	
	ofxXmlSettings settings;
	OverlayImage edges[4];
	
//----------------------------------------------------------------------------
	
	ofxLabProjectedView( int _index, float _x, float _y, float _width, float _height):
	index(_index),
	x(_x), 
	y(_y),
	width(_width), 
	height(_height)
	{		
		skewCorners[0].x = cropCorners[0].x = x;
		skewCorners[0].y = cropCorners[0].x = y;
		skewCorners[1].x = cropCorners[1].x = x + width;
		skewCorners[1].y = cropCorners[1].x = y;
		skewCorners[2].x = cropCorners[2].x = x + width;
		skewCorners[2].y = cropCorners[2].x = y + height;
		skewCorners[3].x = cropCorners[3].x = x;
		skewCorners[3].y = cropCorners[3].x = y + height;
		
		//loadSettings();
		texture.allocate(width, height, true);
		
		bDrawSkew = false;
		bDrawCrop = false;
		
		//skew
		selectedPoint  = -1;
		for (int j=0; j<4; j++){
			selected[j] = false;
		}
		
		//crop
		selectedCropPoint = -1;
		for (int j=0; j<4; j++){
			bCropSelected[j] = false;
		}
		
		//edge overlays
		for (int i=0; i<4; i++){
			edges[i].loadImage("overlay/overlay.png", width, height);
			edges[i].width = width;		
			edges[i].height = 10;
		}
		
		edges[0].baseX = width;
		edges[0].baseY = height;
		edges[0].x = width;
		edges[0].y = height;
		edges[0].width = width;		
		edges[0].height = 10;
		edges[0].rotation = 180;
		
		edges[1].baseX = width;
		edges[1].baseY = 0;
		edges[1].x = width;
		edges[1].y = 0;
		edges[1].width = height;	
		edges[1].rotation = 90;
		
		edges[2].baseX = 0;
		edges[2].baseY = 0;
		edges[2].x = 0;
		edges[2].y = 0;
		edges[2].rotation = 0;
		
		edges[3].baseX = 0;
		edges[3].baseY = height;
		edges[3].x = 0;
		edges[3].y = height;
		edges[3].width = height;
		edges[3].rotation = 270;
		
		loadSettings();
		
	};
	
//----------------------------------------------------------------------------
	
	void draw(){		
		texture.bind();
		glBegin(GL_QUADS);{		
			glTexCoord2f(0,0);
			glVertex2f(skewCorners[0].x, skewCorners[0].y);		
			glTexCoord2f(width,0);
			glVertex2f(skewCorners[1].x, skewCorners[1].y);		
			glTexCoord2f(width,height);
			glVertex2f(skewCorners[2].x, skewCorners[2].y);		
			glTexCoord2f(0,height);
			glVertex2f(skewCorners[3].x, skewCorners[3].y);
		}glEnd(); 
		texture.unbind();
		
		ofEnableAlphaBlending();
		
		//render overlay fbos
		
		for (int i=0; i<4; i++){
			edges[i].render();			
			edges[i].texture.bind();
			glBegin(GL_QUADS);{
				glTexCoord2f(0,0);
				glVertex2f(skewCorners[0].x, skewCorners[0].y);		
				glTexCoord2f(width,0);
				glVertex2f(skewCorners[1].x, skewCorners[1].y);		
				glTexCoord2f(width,height);
				glVertex2f(skewCorners[2].x, skewCorners[2].y);		
				glTexCoord2f(0,height);
				glVertex2f(skewCorners[3].x, skewCorners[3].y);
			}glEnd(); 
			edges[i].texture.unbind();
		}
		
		//texture.draw(x, y);
		
		if (bDrawSkew){
			ofPushStyle();{
				ofSetColor(255,0,0);
				ofRect(skewCorners[0].x, skewCorners[0].y, 5, 5);
				ofSetColor(0,255,0);
				ofRect(skewCorners[1].x, skewCorners[1].y, 5, 5);
				ofSetColor(0,0,255);
				ofRect(skewCorners[2].x, skewCorners[2].y, 5, 5);
				ofSetColor(255,255,0);
				ofRect(skewCorners[3].x, skewCorners[3].y, 5, 5);
			}ofPopStyle();
		} else if (bDrawCrop){
			ofPushStyle();{
				ofSetColor(255,0,0);
				ofRect(skewCorners[0].x, skewCorners[0].y, 5, 5);
				ofSetColor(0,255,0);
				ofRect(skewCorners[1].x, skewCorners[1].y, 5, 5);
				ofSetColor(0,0,255);
				ofRect(skewCorners[2].x, skewCorners[2].y, 5, 5);
				ofSetColor(255,255,0);
				ofRect(skewCorners[3].x, skewCorners[3].y, 5, 5);
			}ofPopStyle();			
		};
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
	ofPoint getResolution(){
		return ofPoint(width,height);
	} 
	
//----------------------------------------------------------------------------
	ofPoint getRenderPosition(){
		return ofPoint(x,y);
	}
					   
					   
//----------------------------------------------------------------------------
	
	void drawSkew( int skew=true ){
		bDrawSkew = skew;
		if (bDrawSkew) bDrawCrop =false;
	}
	
//----------------------------------------------------------------------------
	
	void drawCrop( bool crop=true ){
		bDrawCrop = crop;
		if (bDrawCrop) bDrawSkew =false;
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
			float dx1 = fabs(mX -  skewCorners[0].x);
			float dy1 = fabs(mY -  skewCorners[0].y);
			float dx2 = fabs(mX -  skewCorners[1].x);
			float dy2 = fabs(mY -  skewCorners[1].y);
			float dx3 = fabs(mX -  skewCorners[2].x);
			float dy3 = fabs(mY -  skewCorners[2].y);
			float dx4 = fabs(mX -  skewCorners[3].x);
			float dy4 = fabs(mY -  skewCorners[3].y);
			
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
				skewCorners[i].x = mx;
				skewCorners[i].y = my;
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
		
		//skew
		for (int i=0; i<settings.getNumTags("quad"); i++){
			settings.pushTag("quad", i);
			
			stringstream intStream;
			intStream<<index;
			
			if (settings.getValue("name", "", 0) == intStream.str()){			
				cout << "found" << endl;
				skewCorners[0].x = settings.getValue("x", 0, 0);
				skewCorners[0].y = settings.getValue("y", 0, 0);
				skewCorners[1].x = settings.getValue("x", 0, 1);
				skewCorners[1].y = settings.getValue("y", 0, 1);
				skewCorners[2].x = settings.getValue("x", 0, 2);
				skewCorners[2].y = settings.getValue("y", 0, 2);
				skewCorners[3].x = settings.getValue("x", 0, 3);
				skewCorners[3].y = settings.getValue("y", 0, 3);
			}
			settings.popTag();
		}
		
		//crop
		for (int i=0; i<settings.getNumTags("crop"); i++){
			settings.pushTag("crop", i);
			
			stringstream intStream;
			intStream<<index;
			
			if (settings.getValue("name", "", 0) == intStream.str()){			
				cout << "found" << endl;
				cropCorners[0].x = settings.getValue("x", 0, 0);
				cropCorners[0].y = settings.getValue("y", 0, 0);
				cropCorners[1].x = settings.getValue("x", 0, 1);
				cropCorners[1].y = settings.getValue("y", 0, 1);
				cropCorners[2].x = settings.getValue("x", 0, 2);
				cropCorners[2].y = settings.getValue("y", 0, 2);
				cropCorners[3].x = settings.getValue("x", 0, 3);
				cropCorners[3].y = settings.getValue("y", 0, 3);
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
		
		//skew
		
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
		settings.setValue("x", skewCorners[0].x,0);
		settings.setValue("y", skewCorners[0].y,0);		
		settings.setValue("x", skewCorners[1].x,1);
		settings.setValue("y", skewCorners[1].y,1);		
		settings.setValue("x", skewCorners[2].x,2);
		settings.setValue("y", skewCorners[2].y,2);		
		settings.setValue("x", skewCorners[3].x,3);
		settings.setValue("y", skewCorners[3].y,3);		
		settings.popTag();
		
		//crop
		
		found = false;
		
		for (int i=0; i<settings.getNumTags("crop"); i++){
			settings.pushTag("crop", i);			
			if (settings.getValue("name", "", 0) == intStream.str()){
				found = true;
			} else {
				settings.popTag();
			}
		}
		
		if (!found){
			settings.addTag("crop");
			settings.pushTag("crop", settings.getNumTags("crop")-1);
		}
		
		settings.setValue("name", intStream.str(),0);
		settings.setValue("x", skewCorners[0].x,0);
		settings.setValue("y", skewCorners[0].y,0);		
		settings.setValue("x", skewCorners[1].x,1);
		settings.setValue("y", skewCorners[1].y,1);		
		settings.setValue("x", skewCorners[2].x,2);
		settings.setValue("y", skewCorners[2].y,2);		
		settings.setValue("x", skewCorners[3].x,3);
		settings.setValue("y", skewCorners[3].y,3);		
		settings.popTag();
		
		//pop settings tag + save
		
		settings.popTag();
		settings.saveFile(ofToDataPath("settings.xml", true));
	};
	
};


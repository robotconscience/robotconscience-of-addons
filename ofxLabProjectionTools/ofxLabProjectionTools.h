/*
 *  ofxLabProjectionTools.h
 *  kaleidoscope
 *
 *  Created by Brett Renfer on 3/16/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once

#include "ofxLabProjectedView.h"
#include "ofxLabGui.h"

/**************************************************************************

 this is a set of tools for creating masked projections
 right now, it works like this:
 - add views in setup()
 - in draw(), push and pop views to draw to them
 - when you're done drawing to each view, call draw() on 
	your ofxLabProectionTools object
 
 other utils:
 - gui
	- an ofxLabGui instance is rolled in
	- to add to it, call getGui() in your testApp, which returns a pointer
		to the gui
	- you can add default groups + panels as well to match look + feel
 
**************************************************************************/


class ofxLabProjectionTools
{
public:
	
	vector<ofxLabProjectedView *> views;
	int currentView;
	
	ofxLabGui gui;
	bool bDrawGui;
	bool bDrawSkew;
	bool bDrawCropping;
	
	ofxLabProjectionTools(){
		currentView = -1;
		bDrawGui = false;
		bDrawSkew = false;
		
		setupGui();
		
		ofAddListener(ofEvents.mousePressed, this, &ofxLabProjectionTools::mousePressed);
		ofAddListener(ofEvents.mouseReleased, this, &ofxLabProjectionTools::mouseReleased);
		ofAddListener(ofEvents.mouseDragged, this, &ofxLabProjectionTools::mouseDragged);
		
		//views.push_back(ofxLabProjectedView(0,0,ofGetWidth(),ofGetHeight()));
		//loadViews();
	};
	
	
	//----------------------------------------------------------------------------
	void setupGui(){
		gui.setup("settings", 0, 0, 330, 1000);
		gui.setShowText(false);
		gui.setCollapsible(false);
		gui.setDraggable(true);
		gui.setBackgroundColor(255,255,255);
		gui.setOutlineColor(255,255,255,0);
		gui.setTextColor(148,129,85);
		gui.setSaveColor(58, 187, 147);
		gui.setLoadColor(180, 87, 128);
		gui.setSaveAsColor(238, 53, 35);
		gui.setSaveSelectedColor(80, 253, 190);
		gui.setRestoreColor(34, 151, 210);
		gui.setRestoreSelectedColor(116, 191, 228);
		gui.setDrawBarOnTop(false);		
		gui.update();
	}
	
	//----------------------------------------------------------------------------
	void addView( int x, int y, int width, int height ){
		ofxLabProjectedView * view = new ofxLabProjectedView(views.size(), x,y,width,height);
		views.push_back(view);
		
		stringstream name;
		name<<"view "<<views.size();
		
		stringstream index;
		index<<views.size()-1;
		
		guiTypePanel * panel = gui.addPanel(name.str(), 1, false);
		panel->setDrawLock( false );	
		panel->setBackgroundColor(174,139,138);
		panel->setBackgroundSelectColor(174,139,138);
		
		// video settings
		gui.setWhichPanel(name.str());
		guiTypeGroup * maskGroup = gui.addGroup("mask");
		maskGroup->setActive(true);
		maskGroup->setBackgroundColor(148,129,85);
		maskGroup->setBackgroundSelectColor(148,129,85);
		maskGroup->seBaseColor(244,136,136);
		maskGroup->setShowText(false);
		
		gui.addSlider("top mask: x", "TOP_MASK_X_"+index.str(), view->edges[0].x,-view->width,view->width,false);
		gui.addSlider("top mask: y", "TOP_MASK_Y_"+index.str(), view->edges[0].y,-view->height,view->height,false);
		gui.addSlider("top mask: width", "TOP_MASK_WIDTH_"+index.str(), view->edges[0].width,0,view->width,false);
		gui.addSlider("top mask: height", "TOP_MASK_HEIGHT_"+index.str(), view->edges[0].height,0,view->height,false);
		gui.addSlider("top mask: rotation", "TOP_MASK_ROTATE_"+index.str(), view->edges[0].rotation,-180,180,false);
		
		gui.addSlider("right mask: x", "RIGHT_MASK_X_"+index.str(), view->edges[1].x,-view->width,view->width,false);
		gui.addSlider("right mask: y", "RIGHT_MASK_Y_"+index.str(), view->edges[1].y,-view->height,view->height,false);
		gui.addSlider("right mask: width", "RIGHT_MASK_WIDTH_"+index.str(), view->edges[1].width,0,view->width,false);
		gui.addSlider("right mask: height", "RIGHT_MASK_HEIGHT_"+index.str(), view->edges[1].height,0,view->height,false);
		gui.addSlider("top mask: rotation", "TOP_MASK_ROTATE_"+index.str(), view->edges[1].rotation,-180,180,false);
		
		gui.addSlider("bottom mask: x", "BOTTOM_MASK_X_"+index.str(), view->edges[2].x,-view->width,view->width,false);
		gui.addSlider("bottom mask: y", "BOTTOM_MASK_Y_"+index.str(), view->edges[2].y,-view->height,view->height,false);
		gui.addSlider("bottom mask: width", "BOTTOM_MASK_WIDTH_"+index.str(), view->edges[2].width,0,view->width,false);
		gui.addSlider("bottom mask: height", "BOTTOM_MASK_HEIGHT_"+index.str(), view->edges[2].height,0,view->height,false);
		gui.addSlider("top mask: rotation", "TOP_MASK_ROTATE_"+index.str(), view->edges[2].rotation,-180,180,false);
		
		gui.addSlider("left mask: x", "LEFT_MASK_X_"+index.str(), view->edges[3].x,-view->width,view->width,false);
		gui.addSlider("left mask: y", "LEFT_MASK_Y_"+index.str(), view->edges[3].y,-view->height,view->height,false);
		gui.addSlider("left mask: width", "LEFT_MASK_WIDTH_"+index.str(), view->edges[3].width,0,view->width,false);
		gui.addSlider("left mask: height", "LEFT_MASK_HEIGHT_"+index.str(), view->edges[3].height,0,view->height,false);
		gui.addSlider("top mask: rotation", "TOP_MASK_ROTATE_"+index.str(), view->edges[3].rotation,-180,180,false);
		gui.loadSettings("settings/guisettings.xml");
		gui.update();
		update();
	};
	
	//----------------------------------------------------------------------------
	void pushView(){
		if (views.size() <= 0){
			ofLog(OF_LOG_ERROR, "there are no views!");
		}
		pushView(0);
	};
	
	//----------------------------------------------------------------------------
	void pushView( int which ){
		if (views.size() <= 0){
			ofLog(OF_LOG_ERROR, "there are no views!");
			return;
		}
		if (which > views.size() - 1) return;
		views[which]->beginDraw();
		currentView = which;
	};
	
	//----------------------------------------------------------------------------
	void popView(){
		if (currentView != -1) views[currentView]->endDraw();
		currentView = -1;
	};
	
	
	//----------------------------------------------------------------------------
	void update(){
		gui.update();
		
		for (int i=0; i<views.size(); i++){
			stringstream index;
			index<<i;
			
			views[i]->edges[0].x = gui.getValueF("TOP_MASK_X_"+index.str());
			views[i]->edges[0].y = gui.getValueF("TOP_MASK_Y_"+index.str());
			views[i]->edges[0].width = gui.getValueF("TOP_MASK_WIDTH_"+index.str());
			views[i]->edges[0].height = gui.getValueF("TOP_MASK_HEIGHT_"+index.str());
			views[i]->edges[0].rotation = gui.getValueF("TOP_MASK_ROTATE_"+index.str());
			
			views[i]->edges[1].x = gui.getValueF("RIGHT_MASK_X_"+index.str());
			views[i]->edges[1].y = gui.getValueF("RIGHT_MASK_Y_"+index.str());
			views[i]->edges[1].width = gui.getValueF("RIGHT_MASK_WIDTH_"+index.str());
			views[i]->edges[1].height = gui.getValueF("RIGHT_MASK_HEIGHT_"+index.str());
			views[i]->edges[1].rotation = gui.getValueF("TOP_MASK_ROTATE_"+index.str());
			
			views[i]->edges[2].x = gui.getValueF("BOTTOM_MASK_X_"+index.str());
			views[i]->edges[2].y = gui.getValueF("BOTTOM_MASK_Y_"+index.str());
			views[i]->edges[2].width = gui.getValueF("BOTTOM_MASK_WIDTH_"+index.str());
			views[i]->edges[2].height = gui.getValueF("BOTTOM_MASK_HEIGHT_"+index.str());
			views[i]->edges[2].rotation = gui.getValueF("TOP_MASK_ROTATE_"+index.str());
			
			views[i]->edges[3].x = gui.getValueF("LEFT_MASK_X_"+index.str());
			views[i]->edges[3].y = gui.getValueF("LEFT_MASK_Y_"+index.str());
			views[i]->edges[3].width = gui.getValueF("LEFT_MASK_WIDTH_"+index.str());
			views[i]->edges[3].height = gui.getValueF("LEFT_MASK_HEIGHT_"+index.str());
			views[i]->edges[3].rotation = gui.getValueF("TOP_MASK_ROTATE_"+index.str());
		}
	}
	
	//----------------------------------------------------------------------------
	void draw(){
		for (int i=0; i<views.size(); i++){
			views[i]->draw();
		}
		
		if (bDrawGui) gui.draw();
	};
	
	//----------------------------------------------------------------------------
	void draw( int which ){
		if (which > views.size() -1) return;
		views[which]->draw();
		if (bDrawGui) gui.draw();
	};
	
/**************************************************************************
	 VIEW INFORMATION
**************************************************************************/
	
	ofPoint getViewResolution( int which ){
		if (which > views.size() -1) return ofPoint();		
		return views[which]->getResolution();
	}
	
	ofPoint getRenderPosition( int which ){
		if (which > views.size() -1) return ofPoint();		
		return views[which]->getRenderPosition();
	}
	
/**************************************************************************
	DRAW SKEWABLE CORNERS
**************************************************************************/
	
	// deprecated
	//----------------------------------------------------------------------------
	void drawDebug( bool debug=true){
		bDrawSkew = debug;
		for (int i=0; i<views.size(); i++){
			views[i]->drawSkew(bDrawSkew);
		}
	};
	// deprecated
	//----------------------------------------------------------------------------
	void toggleDrawDebug(){
		bDrawSkew = !bDrawSkew;
		for (int i=0; i<views.size(); i++){
			views[i]->drawSkew(bDrawSkew);
		}
	}
	
	
	//----------------------------------------------------------------------------
	void drawSkew( bool skew=true){
		bDrawSkew = skew;
		for (int i=0; i<views.size(); i++){
			views[i]->drawSkew(bDrawSkew);
		}
	};
	
	//----------------------------------------------------------------------------
	void toggleDrawSkew](){
		bDrawSkew = !bDrawSkew;
		for (int i=0; i<views.size(); i++){
			views[i]->drawSkew(bDrawSkew);
		}
	}
	
/**************************************************************************
	DRAW CROP CORNERS
**************************************************************************/
	
	//----------------------------------------------------------------------------
	void drawCrop( bool crop=true){
		bDrawCropping = crop;
		for (int i=0; i<views.size(); i++){
			views[i]->drawCrop(bDrawCropping);
		}
	};
	
	//----------------------------------------------------------------------------
	void toggleDrawCrop(){
		bDrawCropping = !bDrawCropping;
		for (int i=0; i<views.size(); i++){
			views[i]->drawCrop(bDrawCropping);
		}
	}

/**************************************************************************
	GUI CONTROLS
**************************************************************************/
	
	//----------------------------------------------------------------------------
	void drawGui( bool gui=true){
		bDrawGui = gui;
	};
	
	//----------------------------------------------------------------------------
	void toggleDrawGui(){
		bDrawGui = !bDrawGui;
	}
	
	//----------------------------------------------------------------------------
	ofxLabGui * getGui(){
		return &gui;
	};
	
	//----------------------------------------------------------------------------
	guiTypePanel * addDefaultPanel( string name ){
		guiTypePanel * panel = gui.addPanel(name, 1, false);		
		panel->setDrawLock( false );	
		panel->setBackgroundColor(174,139,138);
		panel->setBackgroundSelectColor(174,139,138);
		return panel;
	}
	
	//----------------------------------------------------------------------------
	guiTypeGroup * addDefaultGroup ( string name, bool showText=false){
		guiTypeGroup * group = gui.addGroup(name);		
		group->setActive(true);
		group->setBackgroundColor(148,129,85);
		group->setBackgroundSelectColor(148,129,85);
		group->seBaseColor(244,136,136);
		group->setShowText(showText);
		return group;
	}
	

/**************************************************************************
	UTILS
**************************************************************************/
	
	//----------------------------------------------------------------------------
	ofxLabProjectedView * getView( int which ){
		return views[which];
	};
	
	//----------------------------------------------------------------------------
	int getNumViews(  ){
		return views.size();
	};
	
	//----------------------------------------------------------------------------
	void mousePressed( ofMouseEventArgs & e ){
		int x = e.x;
		int y = e.y;
		if(bDrawGui) gui.mousePressed(x, y, 0);
		
		if (bDrawSkew){
			for (int i=views.size()-1; i>=0; i--){
				views[i]->mousePressed(x,y); 
				if (views[i]->selectedPoint != -1) break;
			}
		}
	};
	
	//----------------------------------------------------------------------------
	void mouseReleased( ofMouseEventArgs & e ){
		int x = e.x;
		int y = e.y;
		if(bDrawGui){
			gui.mouseReleased();
			update();
		}
		if (bDrawSkew){
			for (int i=0; i<views.size(); i++){
				views[i]->mouseReleased();
			}
		}
	};
	
	//----------------------------------------------------------------------------
	void mouseDragged( ofMouseEventArgs & e ){
		int x = e.x;
		int y = e.y;
		if(bDrawGui){
			gui.mouseDragged(x, y, 0);
			update();
		}
		
		if (bDrawSkew){
			for (int i=0; i<views.size(); i++){
				views[i]->mouseDragged(x,y);
			}
		}
	};
	
};


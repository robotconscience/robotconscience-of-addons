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

class ofxLabProjectionTools
{
public:
	
	vector<ofxLabProjectedView *> views;
	int currentView;
	
	ofxLabGui gui;
	bool bDrawGui;
	bool bDrawDebug;
	
	ofxLabProjectionTools(){
		currentView = -1;
		bDrawGui = false;
		bDrawDebug = false;
		
		setupGui();
		
		ofAddListener(ofEvents.mousePressed, this, &ofxLabProjectionTools::mousePressed);
		ofAddListener(ofEvents.mouseReleased, this, &ofxLabProjectionTools::mouseReleased);
		ofAddListener(ofEvents.mouseDragged, this, &ofxLabProjectionTools::mouseDragged);
		
		//views.push_back(ofxLabProjectedView(0,0,ofGetWidth(),ofGetHeight()));
		//loadViews();
	};
	
	
	//----------------------------------------------------------------------------
	void setupGui(){
		gui.setup("settings", 0, 0, 330, 768);
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
		
		gui.addSlider("top mask: x", "TOP_MASK_X_"+index.str(), view->overlays[0].x,-view->width,view->width,false);
		gui.addSlider("top mask: y", "TOP_MASK_Y_"+index.str(), view->overlays[0].y,-view->height,view->height,false);
		gui.addSlider("top mask: width", "TOP_MASK_WIDTH_"+index.str(), view->overlays[0].width,0,view->width,false);
		gui.addSlider("top mask: height", "TOP_MASK_HEIGHT_"+index.str(), view->overlays[0].height,0,view->height,false);
		
		gui.addSlider("right mask: x", "RIGHT_MASK_X_"+index.str(), view->overlays[1].x,-view->width,view->width,false);
		gui.addSlider("right mask: y", "RIGHT_MASK_Y_"+index.str(), view->overlays[1].y,-view->height,view->height,false);
		gui.addSlider("right mask: width", "RIGHT_MASK_WIDTH_"+index.str(), view->overlays[1].width,0,view->width,false);
		gui.addSlider("right mask: height", "RIGHT_MASK_HEIGHT_"+index.str(), view->overlays[1].height,0,view->height,false);
		
		gui.addSlider("bottom mask: x", "BOTTOM_MASK_X_"+index.str(), view->overlays[2].x,-view->width,view->width,false);
		gui.addSlider("bottom mask: y", "BOTTOM_MASK_Y_"+index.str(), view->overlays[2].y,-view->height,view->height,false);
		gui.addSlider("bottom mask: width", "BOTTOM_MASK_WIDTH_"+index.str(), view->overlays[2].width,0,view->width,false);
		gui.addSlider("bottom mask: height", "BOTTOM_MASK_HEIGHT_"+index.str(), view->overlays[2].height,0,view->height,false);
		
		gui.addSlider("left mask: x", "LEFT_MASK_X_"+index.str(), view->overlays[3].x,-view->width,view->width,false);
		gui.addSlider("left mask: y", "LEFT_MASK_Y_"+index.str(), view->overlays[3].y,-view->height,view->height,false);
		gui.addSlider("left mask: width", "LEFT_MASK_WIDTH_"+index.str(), view->overlays[3].width,0,view->width,false);
		gui.addSlider("left mask: height", "LEFT_MASK_HEIGHT_"+index.str(), view->overlays[3].height,0,view->height,false);
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
			
			views[i]->overlays[0].x = gui.getValueF("TOP_MASK_X_"+index.str());
			views[i]->overlays[0].y = gui.getValueF("TOP_MASK_Y_"+index.str());
			views[i]->overlays[0].width = gui.getValueF("TOP_MASK_WIDTH_"+index.str());
			views[i]->overlays[0].height = gui.getValueF("TOP_MASK_HEIGHT_"+index.str());
			
			views[i]->overlays[1].x = gui.getValueF("RIGHT_MASK_X_"+index.str());
			views[i]->overlays[1].y = gui.getValueF("RIGHT_MASK_Y_"+index.str());
			views[i]->overlays[1].width = gui.getValueF("RIGHT_MASK_WIDTH_"+index.str());
			views[i]->overlays[1].height = gui.getValueF("RIGHT_MASK_HEIGHT_"+index.str());
			
			views[i]->overlays[2].x = gui.getValueF("BOTTOM_MASK_X_"+index.str());
			views[i]->overlays[2].y = gui.getValueF("BOTTOM_MASK_Y_"+index.str());
			views[i]->overlays[2].width = gui.getValueF("BOTTOM_MASK_WIDTH_"+index.str());
			views[i]->overlays[2].height = gui.getValueF("BOTTOM_MASK_HEIGHT_"+index.str());
			
			views[i]->overlays[3].x = gui.getValueF("LEFT_MASK_X_"+index.str());
			views[i]->overlays[3].y = gui.getValueF("LEFT_MASK_Y_"+index.str());
			views[i]->overlays[3].width = gui.getValueF("LEFT_MASK_WIDTH_"+index.str());
			views[i]->overlays[3].height = gui.getValueF("LEFT_MASK_HEIGHT_"+index.str());
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
		views[which]->draw();
		if (bDrawGui) gui.draw();
	};
	
	//----------------------------------------------------------------------------
	void drawDebug( bool debug=true){
		bDrawDebug = debug;
		for (int i=0; i<views.size(); i++){
			views[i]->drawDebug(bDrawDebug);
		}
	};
	
	//----------------------------------------------------------------------------
	void toggleDrawDebug(){
		bDrawDebug = !bDrawDebug;
		for (int i=0; i<views.size(); i++){
			views[i]->drawDebug(bDrawDebug);
		}
	}
		
	//----------------------------------------------------------------------------
	void toggleDrawGui(){
		bDrawGui = !bDrawGui;
	}
	
	//----------------------------------------------------------------------------
	ofxLabProjectedView * getView( int which ){
		return views[which];
	};
	
	//----------------------------------------------------------------------------
	void mousePressed( ofMouseEventArgs & e ){
		int x = e.x;
		int y = e.y;
		if(bDrawGui) gui.mousePressed(x, y, 0);
		
		if (bDrawDebug){
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
		if (bDrawDebug){
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
		
		if (bDrawDebug){
			for (int i=0; i<views.size(); i++){
				views[i]->mouseDragged(x,y);
			}
		}
	};
	
};


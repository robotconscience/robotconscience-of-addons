/*
 *  ofxTSPSReceiver.h
 *  Hopscotch Confetti
 *
 *  Created by Brett Renfer on 3/8/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#ifndef OFX_TSPS_RECEIVER
#define OFX_TSPS_RECEIVER

//include dependent addons
#include "ofxOsc.h"

//include src
#include "ofxTSPSPerson.h"
#include "ofxTSPSScene.h"

//interface for listener of people events
class ofxPersonListener {
public:
	//called when the person enters the system
    virtual void personEntered( ofxTSPSPerson* person, ofxTSPSScene* scene ) = 0;
	//called each time the centroid moves (a lot)
	virtual void personMoved( ofxTSPSPerson* person, ofxTSPSScene* scene ) = 0;
	//called one frame before the person is removed from the list to let you clean up
    virtual void personWillLeave( ofxTSPSPerson* person, ofxTSPSScene* scene ) = 0;
	//called every frame no matter what.
	virtual void personUpdated(ofxTSPSPerson* person, ofxTSPSScene* scene) = 0;
};

class ofxTSPSReceiver : public ofxOscReceiver {
public:
	ofxTSPSReceiver();
	
	void update( ofEventArgs &e );
	void draw( int width, int height );
	void connect( int port );
	
	//function to attach testApp
	void setListener( ofxPersonListener* delegate );
	
	ofxPersonListener* eventListener;	
	ofxTSPSPerson* personAtIndex(int i);
	ofxTSPSPerson* getTrackedPerson(int pid);
	int totalPeople();
	
	void personEntered( ofxTSPSPerson * person );
	void personMoved( ofxTSPSPerson * person );
	void personWillLeave( ofxTSPSPerson * person );
	void personUpdated( ofxTSPSPerson * person );
	
protected:	
	vector<ofxTSPSPerson*> trackedPeople;
	ofxTSPSScene scene;
	bool bSetup;
};

#endif


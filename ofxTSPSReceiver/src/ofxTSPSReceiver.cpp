/*
 *  ofxTSPSReceiver.cpp
 *  Hopscotch Confetti
 *
 *  Created by Brett Renfer on 3/8/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#include "ofxTSPSReceiver.h"

ofxTSPSReceiver::ofxTSPSReceiver(){
	bSetup = false;
};

void ofxTSPSReceiver::connect( int port ){
	setup(port);
	bSetup = true;
}

void ofxTSPSReceiver::setListener(ofxPersonListener* listener)
{
	eventListener = listener;
	ofAddListener(ofEvents.update, this, &ofxTSPSReceiver::update);
}

void ofxTSPSReceiver::draw(int width, int height){
	ofPushStyle();
	ofNoFill();
	ofSetLineWidth(.5);
	for(int i = 0; i < totalPeople(); i++){
		ofxTSPSPerson* p = personAtIndex(i);
		ofSetColor(0xffffff);
		ofRect(p->boundingRect.x*width, p->boundingRect.y*height, p->boundingRect.width*width, p->boundingRect.height*height);
		ofSetColor(0xff0000);
		ofCircle((p->centroid.x*width)-2, (p->centroid.y*height)-2, 4);
		ofSetColor(0xffff00);
		ofBeginShape();
		for (int j=0; j<p->contour.size(); j++){
			ofVertex(p->contour[j].x*width, p->contour[j].y*height);
		}
		ofEndShape();
	}
	ofPopStyle();
}

void ofxTSPSReceiver::update( ofEventArgs &e ){
	// check for waiting messages
	while( hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		getNextMessage( &m );
				
		if (m.getAddress() == "TSPS/personEntered/" || 
			m.getAddress() == "TSPS/personMoved/" || 
			m.getAddress() == "TSPS/personUpdated/" || 
			m.getAddress() == "TSPS/personWillLeave/"){
			int id = m.getArgAsInt32(0);
			ofxTSPSPerson * person = new ofxTSPSPerson( id, trackedPeople.size() );
			person->age = m.getArgAsInt32(1);
			person->centroid.x = m.getArgAsFloat(2);
			person->centroid.y = m.getArgAsFloat(3);
			person->velocity.x = m.getArgAsFloat(4);
			person->velocity.y = m.getArgAsFloat(5);
			person->boundingRect.x = m.getArgAsFloat(6);
			person->boundingRect.y = m.getArgAsFloat(7);
			person->boundingRect.width= m.getArgAsFloat(8);
			person->boundingRect.height = m.getArgAsFloat(9);
			person->opticalFlowVelocity.x = m.getArgAsFloat(10);
			person->opticalFlowVelocity.y = m.getArgAsFloat(11);
						
			if (m.getNumArgs() > 12){
				for (int i=12; i<m.getNumArgs(); i+=2){
					person->contour.push_back(ofPoint(m.getArgAsFloat(i),m.getArgAsFloat(i+1)));
				}
			}
						
			if (m.getAddress() == "TSPS/personEntered/"){
				personEntered(person);
			}
			else if (m.getAddress() == "TSPS/personMoved/"){
				personMoved(person);			
			}
			else if (m.getAddress() == "TSPS/personUpdated/"){
				personUpdated(person);			
			}
			else if (m.getAddress() == "TSPS/personWillLeave/"){
				personWillLeave(person);			
			}
		}
	};
};

//unpack osc message + send to appropriate event

void ofxTSPSReceiver::personEntered( ofxTSPSPerson * person ){
	trackedPeople.push_back(person);	
	eventListener->personEntered(person, &scene);
};

void ofxTSPSReceiver::personMoved( ofxTSPSPerson * person ){
	bool found = false;
	
	//look for person... if we're not tracking them yet, call
	// person entered
	for (int i=0; i<trackedPeople.size(); i++){
		if (trackedPeople[i]->pid == person->pid){
			found = true;
			trackedPeople[i]->update(person);
		}
	}
	
	if (found) eventListener->personMoved(person, &scene);
	else personEntered(person);
};

void ofxTSPSReceiver::personUpdated( ofxTSPSPerson * person ){
	bool found = false;
	
	//look for person... if we're not tracking them yet, call
	// person entered
	for (int i=0; i<trackedPeople.size(); i++){
		if (trackedPeople[i]->pid == person->pid){
			found = true;
			trackedPeople[i]->update(person);
			cout<<"UPDATING "<<person->contour.size()<<endl;
		}
	}
	
	if (found) eventListener->personUpdated(person, &scene);
	else personEntered(person);
};


void ofxTSPSReceiver::personWillLeave( ofxTSPSPerson * person ){
	for (int i=trackedPeople.size()-1; i>=0; i--){
		if (trackedPeople[i]->pid == person->pid){
			trackedPeople.erase(trackedPeople.begin() + i);
			break;
		}
	}
	eventListener->personWillLeave(person, &scene);
};

//callbacks to get people
ofxTSPSPerson* ofxTSPSReceiver::personAtIndex(int i)
{
	return trackedPeople[i];
}

int ofxTSPSReceiver::totalPeople()
{
	return trackedPeople.size();
}


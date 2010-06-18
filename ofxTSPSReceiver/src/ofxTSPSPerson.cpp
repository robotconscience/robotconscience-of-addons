/*
 *  ofxTSPSPerson.cpp
 *  peopleTracker_000
 *
 *  Adapted by James George on 11/21/09.
 *  Copyright 2009 (http://www.jamesgeorge.org) All rights reserved.
 *
 */
#include "ofxTSPSPerson.h"

#define MAX_HAAR_GHOSTFRAMES 15 //how long before we say it's really gone

ofxTSPSPerson::ofxTSPSPerson(int pid, int oid, ofxCvBlob blob)
: pid(pid),
  oid(oid),
  age(0),
  hasHaar(false),
  haarRect(ofRectangle(0,0,0,0)),
  opticalFlowVectorAccumulation(ofPoint(0,0)),
  centroid(blob.centroid),
  framesWithoutHaar(0),
  customAttributes(NULL)
{
	update(blob, false);
}

ofxTSPSPerson::ofxTSPSPerson(int pid, int oid, ofxTSPSPerson * person)
: pid(pid),
oid(oid),
age(0),
hasHaar(false),
haarRect(ofRectangle(0,0,0,0)),
opticalFlowVectorAccumulation(ofPoint(0,0)),
centroid(person->centroid),
framesWithoutHaar(0),
customAttributes(NULL)
{
	update(person);
}

ofxTSPSPerson::ofxTSPSPerson(int pid, int oid)
: pid(pid),
oid(oid),
age(0),
hasHaar(false),
haarRect(ofRectangle(0,0,0,0)),
opticalFlowVectorAccumulation(ofPoint(0,0)),
centroid(ofPoint(0,0)),
framesWithoutHaar(0),
customAttributes(NULL)
{};

ofxTSPSPerson::~ofxTSPSPerson()
{
	if(customAttributes != NULL){
		free(customAttributes);
	}
}

void ofxTSPSPerson::update(ofxCvBlob blob, bool dampen)
{
	if(dampen){
		centroid = (centroid * .7) + (blob.centroid * .3);
	}
	else{
		centroid = blob.centroid;
	}
	
	velocity	 = blob.centroid - centroid;
	area		 = blob.area;
	boundingRect = blob.boundingRect;
	contour		 = blob.pts; 
	age++;
}


void ofxTSPSPerson::update(ofxTSPSPerson * person){
	centroid = person->centroid;

	velocity	 = person->centroid - centroid;
	//what should we do for area?
	//area		 = person->area;
	boundingRect = person->boundingRect;
	if (person->contour.size() > 0) contour = person->contour; 
	age			 = person->age;
};

void ofxTSPSPerson::setHaarRect(ofRectangle _haarRect)
{
	haarRect = _haarRect;
	hasHaar = true;
	framesWithoutHaar = 0;
}

ofRectangle ofxTSPSPerson::getHaarRect()
{
	if(!hasHaar){
		printf("ofxTSPSPerson: accessing Haar rect without Haar!\n");
		return ofRectangle(0,0,0,0);
	}
	return haarRect;
}

bool ofxTSPSPerson::hasHaarRect()
{
	return hasHaar;
}

void ofxTSPSPerson::noHaarThisFrame()
{
	//temp remove keep haar
	hasHaar = false;
	return;
	
	//if we had a haar but it vanished move it by the movement of the blob
	if(framesWithoutHaar++ > MAX_HAAR_GHOSTFRAMES){
		hasHaar = false;
	}
	if(hasHaar){
		haarRect.x += velocity.x;
		haarRect.y += velocity.y;
	}
}

ofRectangle ofxTSPSPerson::getBoundingRectNormalized(float videoWidth, float videoHeight)
{
		return ofRectangle(boundingRect.x/videoWidth,
						   boundingRect.y/videoHeight,
						   boundingRect.width/videoWidth,
						   boundingRect.height/videoHeight);
}

ofPoint ofxTSPSPerson::getCentroidNormalized(float videoWidth, float videoHeight)
{
	return ofPoint(centroid.x / videoWidth, centroid.y / videoHeight);
}

ofPoint ofxTSPSPerson::getHaarCentroidNormalized(float videoWidth, float videoHeight)
{
	ofRectangle haarRect = getHaarRect();
	return ofPoint( (haarRect.x + haarRect.width/2) / videoWidth, (haarRect.y + haarRect.height/2) / videoHeight );
}


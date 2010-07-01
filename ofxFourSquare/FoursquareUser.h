/*
 *  FoursquareUser.h
 *  SJ Foursquare Sensor
 *
 *  Created by brenfer on 6/29/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once
#include "ofxXmlSettings.h"

class FoursquareUser
{
public:
	FoursquareUser(){
		bActive = true;
		bNewUser = true;
	}
	
	void loadXMLString( string data ){
				
		ofxXmlSettings xmlData;
		
		//load XML		
		xmlData.loadFromBuffer(data);
		
		xmlData.pushTag("user");{
			uId = xmlData.getValue("id", 0);
			firstname = xmlData.getValue("firstname", "");
			lastname = xmlData.getValue("lastname", "");
			photo = xmlData.getValue("photo", "");
			gender = xmlData.getValue("gender", "");
			shout = xmlData.getValue("shout", "");
		} xmlData.popTag();
	}
		
	string	photo;
	string	gender;
	string	shout;
	
	int		uId;
	string	firstname;
	string	lastname;
	
	string getName(){
		return firstname+" "+lastname;
	}
	
	int getId(){
		return uId;
	}
	
	bool bActive;
	bool bNewUser;
	
protected:
};
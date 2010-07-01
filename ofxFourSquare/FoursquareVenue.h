/*
 *  FoursquareVenue.h
 *  SJ Foursquare Sensor
 *
 *  Created by brenfer on 6/29/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once

#include "ofxXmlSettings.h"
#include "FoursquareUser.h"

class FoursquareVenue
{
public:
	
	FoursquareVenue(){
		name = "";
		bCacheXML = false;
	}
		
	void loadXMLString( string data ){
		
		cout << "load xml" << endl;
		
		//reset users
		
		users.clear();
		
		//load XML
		xmlData.clear();
		xmlData.loadFromBuffer(data);
		
		bool bNew = false;
		
		xmlData.pushTag("venue");{
			
			name = xmlData.getValue("name", "");
			if (bCacheXML) xmlData.saveFile(name+".xml");
			
			if (bNew){
				oldNumCheckins = 0;
			}
			int oldCheckins = totalCheckins;
			geoLat = xmlData.getValue("geolat", "");
			geoLong = xmlData.getValue("geolong", "");
			xmlData.pushTag("stats");{
				totalCheckins = xmlData.getValue("checkins", 0);
				newCheckins = max(0, totalCheckins - oldCheckins);
				herenow = xmlData.getValue("herenow", 0);
			}; xmlData.popTag();
			
			xmlData.pushTag("checkins");{
				numCheckins = xmlData.getNumTags("checkin");
				
				//check users
				for (int i=0; i<numCheckins; i++){
					
					xmlData.pushTag("checkin",i);{
						FoursquareUser* user = new FoursquareUser();
						
						xmlData.pushTag("user");{
							user->uId = xmlData.getValue("id", 0);
							user->firstname = xmlData.getValue("firstname", "");
							user->lastname = xmlData.getValue("lastname", "");
							user->photo = xmlData.getValue("photo", "");
							user->gender = xmlData.getValue("gender", "");
							user->shout = xmlData.getValue("shout", "");
						} xmlData.popTag();
						
						users.push_back(user);
												
					} xmlData.popTag();
				};
				
			} xmlData.popTag();
						
		}; xmlData.popTag();
	}
	
	//get + reset number of new checkins!
	int getNewCheckins(){
		int numNewCheckins = newCheckins;
		newCheckins = 0;
		return numNewCheckins;
	}
	
	//get number of current checkins (different than all time checkins)
	int getCheckins(){
		return numCheckins;
	}
	
	vector <FoursquareUser * > * getUsers(){
		return &users;
	};
	
	string getName(){
		return name;
	};
		
	int getHereNow(){
		return herenow;
	};
	
	
	int userExists( FoursquareUser * checkUser ){
		for (int i=0; i<users.size(); i++){
			if (checkUser->getId() == users[i]->getId()){
				users[i]->bActive = true;
				return i;
			}
		};
		return -1;
	};
	
	//there are a lot more fields we could be storing...
	
	int id;
	string geoLat, geoLong;
	int totalCheckins;
	int numCheckins, oldNumCheckins;
	int newCheckins;
	string name;
	int herenow;
	
protected:
	ofxXmlSettings xmlData;
	
	bool bCacheXML;
	
	vector <FoursquareUser * > users;
};
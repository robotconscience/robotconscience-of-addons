/*
 *  ofxFourSquare.h
 *
 *  Created by brenfer on 6/29/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once

// This still needs a level of error checking... right now if the 
// response is missing a tag it just continues on
// Also, there should be a better specification than google bot

#include "curl.h"
#include "ofxThread.h"
#include "FoursquareVenue.h"

enum{
	FOURSQUARE_QUERY_USER,
	FOURSQUARE_QUERY_VENUE,
	FOURSQUARE_QUERY_VENUES
};

class ofxFourSquare : public ofxThread
{
public:
	
	int queryType;
	bool bReady;
	
	ofxFourSquare(){
		queryType = 0;
		bReady = false;
		queryURL = "";
	};
	
	//set user and password you are auth-ing with
	
	void setAuth( string user, string pw){
		userName = user;
		password = pw;
		bDoAuth = true;
	}
		
	//get details from a specific venue via ID
	
	void getVenueDetails( string _venueId){
		venueId = _venueId;
		queryType = FOURSQUARE_QUERY_VENUE;
		if (isThreadRunning()) stopThread();
		
		queryURL = "http://api.foursquare.com/v1/venue?vid="+venueId;
		
		startThread();
	};
	
	//get a list of nearby venues via a lat and long
	
	void getVenues( string geolat, string geolong, int numResults=50, string searchWord = "" ){
		queryType = FOURSQUARE_QUERY_VENUES;
		if (isThreadRunning()) stopThread();
		queryURL = "http://api.foursquare.com/v1/venues?geolat="+(geolat)+"&geolong="+(geolong)+"&l="+ofToString(numResults)+"&q="+searchWord;
		startThread();
	};		
		
	//CURL STUFF
	char errorBuffer[CURL_ERROR_SIZE];
	string buffer;
	
	static int stringWriter(char *data, size_t size, size_t nmemb, std::string *buffer){
		int result = 0;
		if (buffer != NULL){
			buffer->append(data, size * nmemb);
			result = size * nmemb;
		}
		return result;
	}
	
	//do we have results yet?
	bool isReady(){
		return bReady;
	}
	
	//do query in thread
	
	void threadedFunction(){
		//while( isThreadRunning() == true ){
			CURL * curl;
			curl = curl_easy_init();
			if (curl){
				
				buffer = "";
								
				if (bDoAuth){
					string userPw = userName+":"+password;
					curl_easy_setopt(curl, CURLOPT_USERPWD, userPw.c_str());
				}
				
				char * userAgent = (char *) "Googlebot/2.1 (http://www.googlebot.com/bot.html)";
				curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent);
				curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
				curl_easy_setopt(curl, CURLOPT_URL, queryURL.c_str());
				curl_easy_setopt(curl, CURLOPT_HEADER, 0);
				curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, stringWriter);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
				
				CURLcode result = curl_easy_perform(curl);
				
				if (result == CURLE_OK){
					cout << "auth success!"<<endl;
				}
				curl_easy_cleanup(curl);
			}
		cout << "errors?"<< errorBuffer<<endl;
			//stopThread();
		//}
		if (queryType == FOURSQUARE_QUERY_VENUE){
			currentVenue.loadXMLString(buffer);
		} else if ( queryType == FOURSQUARE_QUERY_USER){
			//do something
		} else if ( queryType == FOURSQUARE_QUERY_VENUES ){
			loadVenues(buffer);
		}
		
		bReady = true;
	}
	
	//load venues
	void loadVenues( string xmlBuffer, bool bGetNearby=true ){
		cout <<"loading venues! "<<endl;
		//currentVenues.clear();
		ofxXmlSettings xmlData;
		xmlData.loadFromBuffer(xmlBuffer);
		xmlData.saveFile("venues.xml");
		
		xmlData.pushTag("venues");{
			
			//get either nearby (default) or "trending now"
			if (bGetNearby) xmlData.pushTag("group",1);
			else xmlData.pushTag("group",0);
			
			cout << "FOUND "<< xmlData.getNumTags("venue")<<endl;
			for (int i=0; i<xmlData.getNumTags("venue"); i++){
				xmlData.pushTag("venue", i);{
					FoursquareVenue * newVenue = new FoursquareVenue();
					newVenue->id = xmlData.getValue("id", 0);
					
					int index = venueExists(newVenue);
					
					if (index < 0){				
						newVenue->name = xmlData.getValue("name", "");
						newVenue->geoLat = xmlData.getValue("geolat", "");
						newVenue->geoLong = xmlData.getValue("geolong","");
						xmlData.pushTag("stats");{
							newVenue->herenow = xmlData.getValue("herenow", 0);
							newVenue->totalCheckins = xmlData.getValue("checkins", 0);
						} xmlData.popTag();
						currentVenues.push_back(newVenue);
					} else {
						int totalCheckins = 0;
						xmlData.pushTag("stats");{
							currentVenues[index]->herenow = xmlData.getValue("herenow", 0);
							totalCheckins = xmlData.getValue("checkins", 0);
						} xmlData.popTag();
						
						currentVenues[index]->newCheckins = totalCheckins - currentVenues[index]->totalCheckins;
						currentVenues[index]->totalCheckins = totalCheckins;
					}
				} xmlData.popTag();
			}
		} xmlData.popTag();		
	};
	
	//check if already in vector
	
	int venueExists( FoursquareVenue * checkVenue ){
		for (int i=0; i<currentVenues.size(); i++){
			if ( checkVenue->id == currentVenues[i]->id ){
				return i;
			}
		};
		return -1;
	};
	
	//get venues
	
	FoursquareVenue * getCurrentVenue(){
		if (currentVenues.size() > 0) return currentVenues[currentVenues.size()-1];
		return &currentVenue;
	};	
	
	vector <FoursquareVenue *> * getCurrentVenues(){
		return &currentVenues;
	};	
	
protected:
	bool bDoAuth;
	
	string queryURL;
	
	string userName;
	string password;
	string venueId;
					 
	FoursquareVenue currentVenue;
	vector <FoursquareVenue *> currentVenues;
	
};
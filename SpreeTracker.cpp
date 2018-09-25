// SpreeTracker.cpp : A BZFS plugin to track and report player killing sprees.
//

#include "bzfsAPI.h"
#include "plugin_utils.h"
#include <map>
#include <string>
#include <time.h>

using std::map;
using std::string;

class SpreeTracker: public bz_EventHandler
{
private:
	map<int, int> spreeLength;
	map<int, double> lastKill;
	map<int, int> multiLength;
	
	void addPlayer(int playerID);
	void removePlayer(int playerID);
	
	void reportDeath(bz_PlayerDieEventData* event);
	void reportKill(bz_PlayerDieEventData* event);
	void reportSuicide(bz_PlayerDieEventData* event);
	
	void checkSpree(int playerID);
	void checkMulti(int playerID);
public:
	virtual void process(bz_EventData* event);
};

string getCallsign(int playerID)
{
	// Get the player record
	bz_PlayerRecord* player = bz_getPlayerByIndex(playerID);
	
	// Retrieve the callsign
	string callsign(player->callsign.c_str());
	
	// Free the player record
	bz_freePlayerRecord(player);
	
	// Return the callsign
	return callsign;
}

void SpreeTracker::addPlayer(int playerID)
{
	spreeLength[playerID] = 0;
	lastKill[playerID] = 0.0;
	multiLength[playerID] = 0;
}

void SpreeTracker::removePlayer(int playerID)
{
	spreeLength.erase(playerID);
	lastKill.erase(playerID);
	multiLength.erase(playerID);
}

void SpreeTracker::reportDeath(bz_PlayerDieEventData* event)
{
	// Get the player IDs of the killer and victim
	int killerID = event->killerID;
	int victimID = event->playerID;
	
	// Check: was this an actual kill (not a suicide or world event)?
	if ((killerID >= 0) && (killerID != victimID))
	{
		this->reportKill(event);
	}
	else
	{
		this->reportSuicide(event);
	}
}



const time_t MULTI_KILL_TIME = 5.0; // Seconds

const string SPREE_ENDED_MESSAGE = "'s killing spree was ended by ";

void SpreeTracker::reportKill(bz_PlayerDieEventData* event)
{
	// Get the player IDs
	int killerID = event->killerID;
	int victimID = event->playerID;
	
	// Increment the killer's spree length
	spreeLength[killerID]++;
	
	// Check if the killer has reached a new spree level
	checkSpree(killerID);
	
	// If the time since the last kill is short enough...
	double eventTime = event->time;
	if ((eventTime - lastKill[killerID]) <= MULTI_KILL_TIME)
	{
		// Increment the killer's multi-kill counter
		multiLength[killerID]++;
		
		// Print the message for the multi-kill streak
		checkMulti(killerID);
	}
	// Otherwise, if too much time has passed since the last kill
	else
	{
		// Reset the multi-kill streak
		multiLength[killerID] = 0;
	}
	
	// Set the killer's last kill time to this kill
	lastKill[killerID] = eventTime;
	
	// Check if the killed player was on a spree
	string message;
	if (spreeLength[victimID] >= 5)
	{
		message = getCallsign(victimID);
		message += SPREE_ENDED_MESSAGE;
		message += getCallsign(killerID);
		message += "!";
		bz_sendTextMessage(BZ_SERVER, BZ_ALLUSERS, message.c_str());
	}
	
	// Reset all spree data for the killed player
	spreeLength[victimID] = 0;
	lastKill[victimID] = 0.0;
	multiLength[victimID] = 0;
}

const string SUICIDE_MESSAGE = "'s killing spree comes to an unfortunate end.";

void SpreeTracker::reportSuicide(bz_PlayerDieEventData* event)
{
	// Get the player ID
	int playerID = event->playerID;
	
	// Check if the player was on a spree
	if (spreeLength[playerID] >= 5)
	{
		string message = getCallsign(playerID);
		message += SUICIDE_MESSAGE;
		bz_sendTextMessage(BZ_SERVER, BZ_ALLUSERS, message.c_str());
	}
	
	// Reset all spree data for the killed player
	spreeLength[playerID] = 0;
	lastKill[playerID] = 0.0;
	multiLength[playerID] = 0;
}

const string KILLING_SPREE_MESSAGE =	" is on a killing spree!";
const string RAMPAGE_MESSAGE =		" is on a rampage!";
const string DOMINATING_MESSAGE =	" is Dominating!";
const string UNSTOPPABLE_MESSAGE =	" is UNSTOPPABLE!";
const string GODLIKE_MESSAGE =		" is one with the matrix...";

void SpreeTracker::checkSpree(int playerID)
{
	// Have we reached a new milestone?
	bool printMessage = false;
	string message = getCallsign(playerID);
	switch(spreeLength[playerID])
	{
		case 5:
			message += KILLING_SPREE_MESSAGE;
			printMessage = true;
			break;
		case 10:
			message += RAMPAGE_MESSAGE;
			printMessage = true;
			break;
		case 15:
			message += DOMINATING_MESSAGE;
			printMessage = true;
			break;
		case 20:
			message += UNSTOPPABLE_MESSAGE;
			printMessage = true;
			break;
		case 50:
			message += GODLIKE_MESSAGE;
			printMessage = true;
			break;
		
	}
	
	// If there is a new message to print, send it to the server
	if (printMessage)
		bz_sendTextMessage(BZ_SERVER, BZ_ALLUSERS, message.c_str());
}

const string DOUBLE_MESSAGE =	"Double kill!";
const string TRIPLE_MESSAGE =	"Triple kill!";
const string QUAD_MESSAGE =	"QUAD kill!";
const string FIVE_MESSAGE =	"SUPER KILL!";
const string MORE_MESSAGE =	"AMAZING!";

void SpreeTracker::checkMulti(int playerID)
{
	// Find and print the appropriate message to the player
	string message;
	switch (multiLength[playerID])
	{
		case 1:
			message = DOUBLE_MESSAGE;
			break;
		case 2:
			message = TRIPLE_MESSAGE;
			break;
		case 3:
			message = QUAD_MESSAGE;
			break;
		case 4:
			message = FIVE_MESSAGE;
			break;
		default:
			message = MORE_MESSAGE;
			break;
	}
	bz_sendTextMessage(BZ_SERVER, playerID, message.c_str());
}

/* ===================================================================== */
/* ============== SpreeTracker class definitions end here ============== */
/* ===================================================================== */

// Global singleton tracker
SpreeTracker tracker;

BZ_GET_PLUGIN_VERSION

BZF_PLUGIN_CALL int bz_Load ( const char* /*commandLine*/ )
{
	
	bz_registerEvent(bz_ePlayerJoinEvent, &tracker);
	bz_registerEvent(bz_ePlayerPartEvent, &tracker);
	bz_registerEvent(bz_ePlayerDieEvent, &tracker);
	bz_debugMessage(4,"spreeTracker plugin loaded");
	return 0;
}

BZF_PLUGIN_CALL int bz_Unload ( void )
{
	bz_removeEvent(bz_ePlayerDieEvent, &tracker);
	bz_removeEvent(bz_ePlayerPartEvent, &tracker);
	bz_removeEvent(bz_ePlayerJoinEvent, &tracker);
	bz_debugMessage(4,"spreeTracker plugin unloaded");
	return 0;
}

void SpreeTracker::process(bz_EventData *event)
{
	int playerID;
	switch (event->eventType)
	{
		case bz_ePlayerJoinEvent:
			playerID = (dynamic_cast<bz_PlayerJoinPartEventData*>(event))->playerID;
			tracker.addPlayer(playerID);
			break;
		case bz_ePlayerPartEvent:
			playerID = (dynamic_cast<bz_PlayerJoinPartEventData*>(event))->playerID;
			tracker.removePlayer(playerID);
			break;
		case bz_ePlayerDieEvent:
			tracker.reportDeath(dynamic_cast<bz_PlayerDieEventData*>(event));
			break;
		default:
			break;
	}
}

// Local Variables: ***
// mode:C++ ***
// tab-width: 8 ***
// c-basic-offset: 2 ***
// indent-tabs-mode: t ***
// End: ***
// ex: shiftwidth=2 tabstop=8

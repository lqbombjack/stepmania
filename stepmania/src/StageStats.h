#ifndef StageStats_H
#define StageStats_H
/*
-----------------------------------------------------------------------------
 Class: StageStats

 Desc: Contains statistics for one stage of play - either one song, or a whole course.

 Copyright (c) 2001-2002 by the person(s) listed below.  All rights reserved.
	Chris Danford
-----------------------------------------------------------------------------
*/


#include "PlayerNumber.h"
#include "GameConstantsAndTypes.h"
#include "Grade.h"
class Song;


struct StageStats
{
	StageStats();
	void AddStats( const StageStats& other );		// accumulate
	Grade GetGrade( PlayerNumber pn );
	bool OnePassed() const;

	Song*	pSong;
	enum { STAGE_INVALID, STAGE_NORMAL, STAGE_EXTRA, STAGE_EXTRA2 } StageType;
	int		iMeter[NUM_PLAYERS];
	float	fAliveSeconds[NUM_PLAYERS];				// how far into the music did they last before failing?  Updated by Gameplay.
	bool	bFailed[NUM_PLAYERS];					// true if they have failed at any point during the song
	int		iPossibleDancePoints[NUM_PLAYERS];
	int		iActualDancePoints[NUM_PLAYERS];
	int		iTapNoteScores[NUM_PLAYERS][NUM_TAP_NOTE_SCORES];
	int		iHoldNoteScores[NUM_PLAYERS][NUM_HOLD_NOTE_SCORES];
	int		iCurCombo[NUM_PLAYERS];
	int		iMaxCombo[NUM_PLAYERS];
	int		iCurMissCombo[NUM_PLAYERS];
	int		iScore[NUM_PLAYERS];
	int		iBonus[NUM_PLAYERS];  // bonus to be added on screeneval
	float	fRadarPossible[NUM_PLAYERS][NUM_RADAR_CATEGORIES];	// filled in by ScreenGameplay on start of notes
	float	fRadarActual[NUM_PLAYERS][NUM_RADAR_CATEGORIES];	// filled in by ScreenGameplay on start of notes
	float	fSecondsBeforeFail[NUM_PLAYERS];				// -1 means didn't/hasn't failed
	/* The number of songs played and passed, respectively. */
	int		iSongsPassed[NUM_PLAYERS];
	int		iSongsPlayed[NUM_PLAYERS];
	int		iTotalError[NUM_PLAYERS];
};


#endif

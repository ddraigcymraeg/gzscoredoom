// Emacs style mode select	 -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//	 All the global variables that store the internal state.
//	 Theoretically speaking, the internal state of the engine
//	  should be found by looking at the variables collected
//	  here, and every relevant module will have to include
//	  this header file.
//	 In practice, things are a bit messy.
//
//-----------------------------------------------------------------------------


#ifndef __D_STATE__
#define __D_STATE__

// We need globally shared data structures,
//	for defining the global state variables.
// We need the player data structure as well.
//#include "d_player.h"

#include "doomdata.h"
#include "d_net.h"
#include "g_level.h"

// We also need the definition of a cvar
#include "c_cvars.h"

#include <string> //ghk

//GHK: anti-cheat boolean for sd-remote hi scoring allow C_DoCommand to be
//called on startup
extern bool sd_c_docmd_ok;


//GHK used to disallow hi scores, when console is used etc...

extern bool sd_remote_hiscores_ok;

extern bool sd_do_default_monsters;

//GHK: used to check whether a custom SD config is loaded (like sdcustom.ini)
extern bool sd_sdcustom_ini;

//determines whether offline, online or online-hardcore. (0,1,2+)
extern int sd_online_level;

//ghk  determines whether infighting and infighting damage is disabled
extern int sd_notarget;

//ghk  determines whether infighting and infighting damage is disabled
extern int sd_piststart;

//GHK used for enforcing dmflags values
//with online hi scoring
extern int sd_dmflags;

//GHK anti-cheat: Used for the 'global' md5 value (the .exe & gzscoredoom.pk3)
extern const char *sd_global_md5;

//GHK anti-cheat:  used for patch (deh, bex) patch md5 value as well as wads
extern const char *sd_wads_md5; //called in args.considerpatches

extern const char *sd_addonpack_md5;

extern const char *sd_gldefs_md5;

extern const char *sd_configvalues_md5;

//ghk
extern const char *sd_global_sdmini_path;

extern std::string sdConfigValues;

//GHK get files used for patch (deh, bex) from loadup, for 'public' wad saving info
//extern const char *sd_patches;
extern char sd_patches2[2600]; //need to use this instead 2600 chars should be enough for patches

//GHK used for wistuff display

extern bool sd_wistuff_didonce;

extern bool sd_startannounce_didonce; //ghk

//GHK used for finale display

extern bool sd_finale_didonce;

//GHK
extern char wad_hiscore_name[16];

//GHK
extern char lvl_hiscore_name[16];



//GHK:
extern const char *sdBaronChainReps[1000];
extern int sdBaronChainChances[1000];
extern int sdBaronChainRepCount;
extern int sdBaronChainRange;

extern const char *sdCyberChainReps[1000];
extern int sdCyberChainChances[1000];
extern int sdCyberChainRepCount;
extern int sdCyberChainRange;

extern const char *sdSpiderChainReps[1000];
extern int sdSpiderChainChances[1000];
extern int sdSpiderChainRepCount;
extern int sdSpiderChainRange;

extern const char *sdExtraHealthReps[1000];
extern int sdExtraHealthRepChances[1000];
extern int sdExtraHealthRepCount;
extern int sdExtraHealthRange;

extern const char *sdArtiReps[1000];
extern int sdArtiRepChances[1000];
extern int sdArtiRepCount;
extern int sdArtiRange;

extern const char *sdExplosiveBarrelReps[1000];
extern int sdExplosiveBarrelRepChances[1000];
extern int sdExplosiveBarrelRepCount;
extern int sdExplosiveBarrelRange;
extern const char *sdBurningBarrelReps[1000];
extern int sdBurningBarrelRepChances[1000];
extern int sdBurningBarrelRepCount;
extern int sdBurningBarrelRange;
extern const char *sdArchvileReps[1000];
extern int sdArchvileRepChances[1000];
extern int sdArchvileRepCount;
extern int sdArchvileRange;
extern const char *sdArachnotronReps[1000];
extern int sdArachnotronRepChances[1000];
extern int sdArachnotronRepCount;
extern int sdArachnotronRange;
extern const char *sdBaronOfHellReps[1000];
extern int sdBaronOfHellRepChances[1000];
extern int sdBaronOfHellRepCount;
extern int sdBaronOfHellRange;
extern const char *sdHellKnightReps[1000];
extern int sdHellKnightRepChances[1000];
extern int sdHellKnightRepCount;
extern int sdHellKnightRange;
extern const char *sdCacodemonReps[1000];
extern int sdCacodemonRepChances[1000];
extern int sdCacodemonRepCount;
extern int sdCacodemonRange;
extern const char *sdCyberdemonReps[1000]; //array of std:strings instead?
extern int sdCyberdemonRepChances[1000];
extern int sdCyberdemonRepCount;
extern int sdCyberdemonRange;
extern const char *sdDemonReps[1000]; //array of std:strings instead?
extern int sdDemonRepChances[1000];
extern int sdDemonRepCount;
extern int sdDemonRange;
extern const char *sdSpectreReps[1000]; //array of std:strings instead?
extern int sdSpectreRepChances[1000];
extern int sdSpectreRepCount;
extern int sdSpectreRange;
extern const char *sdChaingunGuyReps[1000]; //array of std:strings instead?
extern int sdChaingunGuyRepChances[1000];
extern int sdChaingunGuyRepCount;
extern int sdChaingunGuyRange;
extern const char *sdDoomImpReps[1000]; //array of std:strings instead?
extern int sdDoomImpRepChances[1000];
extern int sdDoomImpRepCount;
extern int sdDoomImpRange;
extern const char *sdFatsoReps[1000]; //array of std:strings instead?
extern int sdFatsoRepChances[1000];
extern int sdFatsoRepCount;
extern int sdFatsoRange;
extern const char *sdLostSoulReps[1000]; //array of std:strings instead?
extern int sdLostSoulRepChances[1000];
extern int sdLostSoulRepCount;
extern int sdLostSoulRange;
extern const char *sdPainElementalReps[1000]; //array of std:strings instead?
extern int sdPainElementalRepChances[1000];
extern int sdPainElementalRepCount;
extern int sdPainElementalRange;
extern const char *sdRevenantReps[1000]; //array of std:strings instead?
extern int sdRevenantRepChances[1000];
extern int sdRevenantRepCount;
extern int sdRevenantRange;
extern const char *sdShotgunGuyReps[1000]; //array of std:strings instead?
extern int sdShotgunGuyRepChances[1000];
extern int sdShotgunGuyRepCount;
extern int sdShotgunGuyRange;
extern const char *sdSpiderMastermindReps[1000]; //array of std:strings instead?
extern int sdSpiderMastermindRepChances[1000];
extern int sdSpiderMastermindRepCount;
extern int sdSpiderMastermindRange;
extern const char *sdWolfensteinSSReps[1000]; //array of std:strings instead?
extern int sdWolfensteinSSRepChances[1000];
extern int sdWolfensteinSSRepCount;
extern int sdWolfensteinSSRange;
extern const char *sdZombieManReps[1000]; //array of std:strings instead?
extern int sdZombieManRepChances[1000];
extern int sdZombieManRepCount;
extern int sdZombieManRange;

//



// -----------------------
// Game speed.
//
enum EGameSpeed
{
	SPEED_Normal,
	SPEED_Fast,
};
extern EGameSpeed GameSpeed;


// ------------------------
// Command line parameters.
//
extern	bool			devparm;		// DEBUG: launched with -devparm



// -----------------------------------------------------
// Game Mode - identify IWAD as shareware, retail etc.
//
extern GameMode_t		gamemode;
extern GameMission_t	gamemission;

// -------------------------------------------
// Selected skill type, map etc.
//

extern	char			startmap[8];		// [RH] Actual map name now

extern	bool 			autostart;

// Selected by user.
EXTERN_CVAR (Int, gameskill);
extern	int				NextSkill;			// [RH] Skill to use at next level load

// Netgame? Only true if >1 player.
extern	bool			netgame;

// Bot game? Like netgame, but doesn't involve network communication.
extern	bool			multiplayer;

// Flag: true only if started as net deathmatch.
EXTERN_CVAR (Int, deathmatch)

// [RH] Pretend as deathmatch for purposes of dmflags
EXTERN_CVAR (Bool, alwaysapplydmflags)

// [RH] Teamplay mode
EXTERN_CVAR (Bool, teamplay)

// [RH] Friendly fire amount
EXTERN_CVAR (Float, teamdamage)

// [RH] The class the player will spawn as in single player,
// in case using a random class with Hexen.
extern int SinglePlayerClass[MAXPLAYERS];

// -------------------------
// Internal parameters for sound rendering.

EXTERN_CVAR (Float, snd_sfxvolume)		// maximum volume for sound
EXTERN_CVAR (Float, snd_musicvolume)	// maximum volume for music


// -------------------------
// Status flags for refresh.
//

enum EMenuState
{
	MENU_Off,			// Menu is closed
	MENU_On,			// Menu is opened
	MENU_WaitKey,		// Menu is opened and waiting for a key in the controls menu
	MENU_OnNoPause,		// Menu is opened but does not pause the game
};

extern	bool			automapactive;	// In AutoMap mode?
extern	EMenuState		menuactive; 	// Menu overlayed?
extern	int				paused; 		// Game Pause?


extern	bool			viewactive;

extern	int				lastleadernum; //ghk

extern	bool	 		nodrawers;
extern	bool	 		noblit;

extern	int 			viewwindowx;
extern	int 			viewwindowy;
extern	"C" int 		viewheight;
extern	"C" int 		viewwidth;
extern	"C"	int			halfviewwidth;		// [RH] Half view width, for plane drawing
extern	"C" int			realviewwidth;		// [RH] Physical width of view window
extern	"C" int			realviewheight;		// [RH] Physical height of view window
extern	"C" int			detailxshift;		// [RH] X shift for horizontal detail level
extern	"C" int			detailyshift;		// [RH] Y shift for vertical detail level





// This one is related to the 3-screen display mode.
// ANG90 = left side, ANG270 = right
extern	int				viewangleoffset;

// Player taking events. i.e. The local player.
extern	int				consoleplayer;


extern level_locals_t level;


// --------------------------------------
// DEMO playback/recording related stuff.
// No demo, there is a human player in charge?
// Disable save/end game?
extern	bool			usergame;

extern	bool			demoplayback;
extern	bool			demorecording;
extern	int				demover;

// Quit after playing a demo from cmdline.
extern	bool			singledemo;




extern	gamestate_t 	gamestate;

extern	int				SaveVersion;




//-----------------------------
// Internal parameters, fixed.
// These are set by the engine, and not changed
//	according to user inputs. Partly load from
//	WAD, partly set at startup time.



extern	int 			gametic;


// Alive? Disconnected?
extern	bool	 		playeringame[MAXPLAYERS];


// Player spawn spots for deathmatch.
extern TArray<mapthing2_t> deathmatchstarts;

// Player spawn spots.
extern	mapthing2_t		playerstarts[MAXPLAYERS];

// Intermission stats.
// Parameters for world map / intermission.
extern	struct wbstartstruct_s wminfo;







//-----------------------------------------
// Internal parameters, used for engine.
//

// File handling stuff.
extern	FILE*			debugfile;

// if true, load all graphics at level load
extern	bool	 		precache;


//-------
//REFRESH
//-------

// wipegamestate can be set to -1
//	to force a wipe on the next draw
extern gamestate_t wipegamestate;
extern bool setsizeneeded;
extern bool setmodeneeded;

extern int BorderNeedRefresh;
extern int BorderTopRefresh;


EXTERN_CVAR (Float, mouse_sensitivity)
//?
// debug flag to cancel adaptiveness
extern	bool	 		singletics;

extern	int 			bodyqueslot;



// Needed to store the number of the dummy sky flat.
// Used for rendering,
//	as well as tracking projectiles etc.
extern int				skyflatnum;



// Netgame stuff (buffers and pointers, i.e. indices).

// This is the interface to the packet driver, a separate program
// in DOS, but just an abstraction here.
extern	doomcom_t		doomcom;

extern	struct ticcmd_t	localcmds[LOCALCMDTICS];

extern	int 			maketic;
extern	int 			nettics[MAXNETNODES];

extern	ticcmd_t		netcmds[MAXPLAYERS][BACKUPTICS];
extern	int 			ticdup;


// ---- [RH] ----
EXTERN_CVAR (Bool, developer)

extern bool ToggleFullscreen;

extern float JoyAxes[6];

extern int Net_Arbitrator;

EXTERN_CVAR (Bool, var_friction)
EXTERN_CVAR (Bool, var_pushers)


// [RH] Miscellaneous info for DeHackEd support
struct DehInfo
{
	int StartHealth;
	int StartBullets;
	int MaxHealth;
	int MaxArmor;
	int GreenAC;
	int BlueAC;
	int MaxSoulsphere;
	int SoulsphereHealth;
	int MegasphereHealth;
	int GodHealth;
	int FAArmor;
	int FAAC;
	int KFAArmor;
	int KFAAC;
	char PlayerSprite[5];
	BYTE ExplosionStyle;
	fixed_t ExplosionAlpha;
	int NoAutofreeze;
};
extern DehInfo deh;
EXTERN_CVAR (Int, infighting)

// [RH] Deathmatch flags

EXTERN_CVAR (Int, dmflags);
EXTERN_CVAR (Int, dmflags2);	// [BC]

EXTERN_CVAR (Int, compatflags);
extern int i_compatflags;

#endif

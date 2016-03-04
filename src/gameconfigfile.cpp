/*
** gameconfigfile.cpp
** An .ini parser specifically for zdoom.ini
**
**---------------------------------------------------------------------------
** Copyright 1998-2006 Randy Heit
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/

#include <stdio.h>
#include <time.h>


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <lmcons.h>
#include <shlobj.h>
extern HWND Window;
#define USE_WINDOWS_DWORD
#endif

#include "doomdef.h"
#include "gameconfigfile.h"
#include "c_cvars.h"
#include "c_dispatch.h"
#include "c_bind.h"
#include "gstrings.h"
#include "m_argv.h"
#include "cmdlib.h"
#include "version.h"
#include "m_misc.h"
#include "v_font.h"
#include "a_pickups.h"
#include "doomstat.h"
#include "i_system.h"
#include "gi.h" //GHK
#include "d_player.h" //GHK
#include "w_wad.h" //GHK added to get at the Wads collection
#include "Request.h" //ghk

#include "md5wrapper.h" //ghk


CVAR (String,sd_hiscoreserver_pwd,"changeme",CVAR_ARCHIVE) //ghk
CVAR (String,sd_hiscoreserver_url,"http://www.scoredoom.com",CVAR_ARCHIVE) //ghk
EXTERN_CVAR (Bool, sd_first_run) //ghk
EXTERN_CVAR (Int, sd_noinfight) //ghk

EXTERN_CVAR (Int, sp_random_custom_monsters2) //ghk
EXTERN_CVAR (Bool, sd_bossrush2) //GHK
EXTERN_CVAR (Bool, sd_defaultmonstermaps_replace_all2) //GHK

EXTERN_CVAR (Int, sd_rep_zombieman_chance) //ghk
EXTERN_CVAR (Int, sd_rep_wolfensteinss_chance) //ghk
EXTERN_CVAR (Int, sd_rep_spidermastermind_chance) //ghk
EXTERN_CVAR (Int, sd_rep_shotgunguy_chance) //ghk
EXTERN_CVAR (Int, sd_rep_revenant_chance) //ghk
EXTERN_CVAR (Int, sd_rep_painelemental_chance) //ghk
EXTERN_CVAR (Int, sd_rep_lostsoul_chance) //ghk
EXTERN_CVAR (Int, sd_rep_fatso_chance) //ghk
EXTERN_CVAR (Int, sd_rep_doomimp_chance) //ghk
EXTERN_CVAR (Int, sd_rep_chaingunguy_chance) //ghk
EXTERN_CVAR (Int, sd_rep_spectre_chance) //ghk
EXTERN_CVAR (Int, sd_rep_demon_chance) //ghk
EXTERN_CVAR (Int, sd_rep_cyberdemon_chance) //ghk
EXTERN_CVAR (Int, sd_rep_cacodemon_chance) //ghk
EXTERN_CVAR (Int, sd_rep_hellknight_chance) //ghk
EXTERN_CVAR (Int, sd_rep_baronofhell_chance) //ghk
EXTERN_CVAR (Int, sd_rep_archvile_chance) //ghk
EXTERN_CVAR (Int, sd_rep_burningbarrel_chance) //ghk
EXTERN_CVAR (Int, sd_rep_explosivebarrel_chance) //ghk
EXTERN_CVAR (Int, sd_rep_arachnotron_chance) //ghk
EXTERN_CVAR (Int, sd_artifact_chance) //ghk
EXTERN_CVAR (Int, sd_extrahealth_chance) //ghk
EXTERN_CVAR (Int, sd_kill_artifact_chance) //ghk
EXTERN_CVAR (Int, sd_kill_extrahealth_chance) //ghk

EXTERN_CVAR (Int, sd_rep_zombieman_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_wolfensteinss_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_spidermastermind_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_shotgunguy_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_revenant_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_painelemental_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_lostsoul_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_fatso_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_doomimp_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_chaingunguy_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_spectre_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_demon_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_cyberdemon_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_cacodemon_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_hellknight_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_baronofhell_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_archvile_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_burningbarrel_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_explosivebarrel_chance2) //ghk
EXTERN_CVAR (Int, sd_rep_arachnotron_chance2) //ghk
EXTERN_CVAR (Int, sd_artifact_chance2) //ghk
EXTERN_CVAR (Int, sd_extrahealth_chance2) //ghk
EXTERN_CVAR (Int, sd_kill_artifact_chance2) //ghk
EXTERN_CVAR (Int, sd_kill_extrahealth_chance2) //ghk


EXTERN_CVAR (Bool, con_centernotify)
EXTERN_CVAR (Int, msg0color)
EXTERN_CVAR (Color, dimcolor)
EXTERN_CVAR (Color, color)
EXTERN_CVAR (Float, dimamount)
EXTERN_CVAR (Int, msgmidcolor)
EXTERN_CVAR (Int, msgmidcolor2)
EXTERN_CVAR (Bool, snd_pitched)
EXTERN_CVAR (Color, am_wallcolor)
EXTERN_CVAR (Color, am_fdwallcolor)
EXTERN_CVAR (Color, am_cdwallcolor)


char wad_hiscore_name[16]; //ghk

char lvl_hiscore_name[16]; //ghk


//GHK:
const char *sdBaronChainReps[1000];
int sdBaronChainChances[1000];
int sdBaronChainRepCount=0;
int sdBaronChainRange=0;

const char *sdCyberChainReps[1000];
int sdCyberChainChances[1000];
int sdCyberChainRepCount=0;
int sdCyberChainRange=0;

const char *sdSpiderChainReps[1000];
int sdSpiderChainChances[1000];
int sdSpiderChainRepCount=0;
int sdSpiderChainRange=0;

const char *sdExtraHealthReps[1000];
int sdExtraHealthRepChances[1000];
int sdExtraHealthRepCount=0;
int sdExtraHealthRange=0;

const char *sdArtiReps[1000];
int sdArtiRepChances[1000];
int sdArtiRepCount=0;
int sdArtiRange=0;

const char *sdExplosiveBarrelReps[1000];
int sdExplosiveBarrelRepChances[1000];
int sdExplosiveBarrelRepCount=0;
int sdExplosiveBarrelRange=0;

const char *sdBurningBarrelReps[1000];
int sdBurningBarrelRepChances[1000];
int sdBurningBarrelRepCount=0;
int sdBurningBarrelRange=0;

const char *sdArchvileReps[1000];
int sdArchvileRepChances[1000];
int sdArchvileRepCount=0;
int sdArchvileRange=0;
const char *sdArachnotronReps[1000];
int sdArachnotronRepChances[1000];
int sdArachnotronRepCount=0;
int sdArachnotronRange=0;
const char *sdBaronOfHellReps[1000];
int sdBaronOfHellRepChances[1000];
int sdBaronOfHellRepCount=0;
int sdBaronOfHellRange=0;
const char *sdHellKnightReps[1000];
int sdHellKnightRepChances[1000];
int sdHellKnightRepCount=0;
int sdHellKnightRange=0;
const char *sdCacodemonReps[1000];
int sdCacodemonRepChances[1000];
int sdCacodemonRepCount=0;
int sdCacodemonRange=0;
const char *sdCyberdemonReps[1000]; //array of std:strings instead?
int sdCyberdemonRepChances[1000];
int sdCyberdemonRepCount=0;
int sdCyberdemonRange=0;
const char *sdDemonReps[1000]; //array of std:strings instead?
int sdDemonRepChances[1000];
int sdDemonRepCount=0;
int sdDemonRange=0;
const char *sdSpectreReps[1000]; //array of std:strings instead?
int sdSpectreRepChances[1000];
int sdSpectreRepCount=0;
int sdSpectreRange=0;
const char *sdChaingunGuyReps[1000]; //array of std:strings instead?
int sdChaingunGuyRepChances[1000];
int sdChaingunGuyRepCount=0;
int sdChaingunGuyRange=0;
const char *sdDoomImpReps[1000]; //array of std:strings instead?
int sdDoomImpRepChances[1000];
int sdDoomImpRepCount=0;
int sdDoomImpRange=0;
const char *sdFatsoReps[1000]; //array of std:strings instead?
int sdFatsoRepChances[1000];
int sdFatsoRepCount=0;
int sdFatsoRange=0;
const char *sdLostSoulReps[1000]; //array of std:strings instead?
int sdLostSoulRepChances[1000];
int sdLostSoulRepCount=0;
int sdLostSoulRange=0;
const char *sdPainElementalReps[1000]; //array of std:strings instead?
int sdPainElementalRepChances[1000];
int sdPainElementalRepCount=0;
int sdPainElementalRange=0;
const char *sdRevenantReps[1000]; //array of std:strings instead?
int sdRevenantRepChances[1000];
int sdRevenantRepCount=0;
int sdRevenantRange=0;
const char *sdShotgunGuyReps[1000]; //array of std:strings instead?
int sdShotgunGuyRepChances[1000];
int sdShotgunGuyRepCount=0;
int sdShotgunGuyRange=0;
const char *sdSpiderMastermindReps[1000]; //array of std:strings instead?
int sdSpiderMastermindRepChances[1000];
int sdSpiderMastermindRepCount=0;
int sdSpiderMastermindRange=0;
const char *sdWolfensteinSSReps[1000]; //array of std:strings instead?
int sdWolfensteinSSRepChances[1000];
int sdWolfensteinSSRepCount=0;
int sdWolfensteinSSRange=0;
const char *sdZombieManReps[1000]; //array of std:strings instead?
int sdZombieManRepChances[1000];
int sdZombieManRepCount=0;
int sdZombieManRange=0;

//

FString WeaponSection;

FGameConfigFile::FGameConfigFile ()
{
	FString pathname;

	bMigrating = false;
	pathname = GetConfigPath (true);
	ChangePathName (pathname);
	LoadConfigFile (MigrateStub, NULL);

	if (!HaveSections ())
	{ // Config file not found; try the old one
		MigrateOldConfig ();
	}

	// If zdoom.ini was read from the program directory, switch
	// to the user directory now. If it was read from the user
	// directory, this effectively does nothing.
	pathname = GetConfigPath (false);
	ChangePathName (pathname);

	// Set default IWAD search paths if none present
	if (!SetSection ("IWADSearch.Directories"))
	{
		SetSection ("IWADSearch.Directories", true);
		SetValueForKey ("Path", ".", true);
		SetValueForKey ("Path", "$DOOMWADDIR", true);
#ifndef unix
		SetValueForKey ("Path", "$HOME", true);
		SetValueForKey ("Path", "$PROGDIR", true);
#else
		SetValueForKey ("Path", HOME_DIR, true);
		SetValueForKey ("Path", SHARE_DIR, true);
#endif
	}

	// Set default search paths if none present
	if (!SetSection ("FileSearch.Directories"))
	{
		SetSection ("FileSearch.Directories", true);
#ifndef unix
		SetValueForKey ("Path", "$PROGDIR", true);
#else
		SetValueForKey ("Path", SHARE_DIR, true);
#endif
		SetValueForKey ("Path", "$DOOMWADDIR", true);
	}
}

//ghk for loading custom monsters ini
FGameConfigFile::FGameConfigFile ( char * sdmname )
{
	FString pathname;

	bMigrating = false;
	pathname = GetConfigPathSDM (true);
	ChangePathName (pathname);
	LoadConfigFile (MigrateStub, NULL);

	//if (!HaveSections ())
	//{ // Config file not found; try the old one
		//MigrateOldConfig ();
	//}

	// If zdoom.ini was read from the program directory, switch
	// to the user directory now. If it was read from the user
	// directory, this effectively does nothing.
	pathname = GetConfigPathSDM (false);
	ChangePathName (pathname);

	// Set default IWAD search paths if none present
	/*if (!SetSection ("IWADSearch.Directories"))
	{
		SetSection ("IWADSearch.Directories", true);
		SetValueForKey ("Path", ".", true);
		SetValueForKey ("Path", "$DOOMWADDIR", true);
#ifndef unix
		SetValueForKey ("Path", "$HOME", true);
		SetValueForKey ("Path", "$PROGDIR", true);
#else
		SetValueForKey ("Path", HOME_DIR, true);
		SetValueForKey ("Path", SHARE_DIR, true);
#endif
	}

	// Set default search paths if none present
	if (!SetSection ("FileSearch.Directories"))
	{
		SetSection ("FileSearch.Directories", true);
#ifndef unix
		SetValueForKey ("Path", "$PROGDIR", true);
#else
		SetValueForKey ("Path", SHARE_DIR, true);
#endif
		SetValueForKey ("Path", "$DOOMWADDIR", true);
	} */
}

FGameConfigFile::~FGameConfigFile ()
{
}

void FGameConfigFile::WriteCommentHeader (FILE *file) const
{
	fprintf (file, "# This file was generated by " GAMENAME " " DOTVERSIONSTR " on %s"
				   "# It is not really meant to be modified outside of ZDoom, nyo.\n\n", myasctime ());
}

void FGameConfigFile::MigrateStub (const char *pathname, FConfigFile *config, void *userdata)
{
	static_cast<FGameConfigFile *>(config)->bMigrating = true;
}

void FGameConfigFile::MigrateOldConfig ()
{
	// Set default key bindings. These will be overridden
	// by the bindings in the config file if it exists.
	C_SetDefaultBindings ();

#if 0	// Disabled for now, maybe forever.
	int i;
	char *execcommand;

	i = strlen (GetPathName ()) + 8;
	execcommand = new char[i];
	sprintf (execcommand, "exec \"%s\"", GetPathName ());
	execcommand[i-5] = 'c';
	execcommand[i-4] = 'f';
	execcommand[i-3] = 'g';
	cvar_defflags = CVAR_ARCHIVE;
	C_DoCommand (execcommand);
	cvar_defflags = 0;
	delete[] execcommand;

	FBaseCVar *configver = FindCVar ("configver", NULL);
	if (configver != NULL)
	{
		UCVarValue oldver = configver->GetGenericRep (CVAR_Float);

		if (oldver.Float < 118.f)
		{
			C_DoCommand ("alias idclip noclip");
			C_DoCommand ("alias idspispopd noclip");

			if (oldver.Float < 117.2f)
			{
				dimamount = *dimamount * 0.25f;
				if (oldver.Float <= 113.f)
				{
					C_DoCommand ("bind t messagemode; bind \\ +showscores;"
								 "bind f12 spynext; bind sysrq screenshot");
					if (C_GetBinding (KEY_F5) && !stricmp (C_GetBinding (KEY_F5), "menu_video"))
					{
						C_ChangeBinding ("menu_display", KEY_F5);
					}
				}
			}
		}
		delete configver;
	}
	// Change all impulses to slot commands
	for (i = 0; i < NUM_KEYS; i++)
	{
		char slotcmd[8] = "slot ";
		char *bind, *numpart;

		bind = C_GetBinding (i);
		if (bind != NULL && strnicmp (bind, "impulse ", 8) == 0)
		{
			numpart = strchr (bind, ' ');
			if (numpart != NULL && strlen (numpart) < 4)
			{
				strcpy (slotcmd + 5, numpart);
				C_ChangeBinding (slotcmd, i);
			}
		}
	}

	// Migrate and delete some obsolete cvars
	FBaseCVar *oldvar;
	UCVarValue oldval;

	oldvar = FindCVar ("autoexec", NULL);
	if (oldvar != NULL)
	{
		oldval = oldvar->GetGenericRep (CVAR_String);
		if (oldval.String[0])
		{
			SetSection ("Doom.AutoExec", true);
			SetValueForKey ("Path", oldval.String, true);
		}
		delete oldvar;
	}

	oldvar = FindCVar ("def_patch", NULL);
	if (oldvar != NULL)
	{
		oldval = oldvar->GetGenericRep (CVAR_String);
		if (oldval.String[0])
		{
			SetSection ("Doom.DefaultDehacked", true);
			SetValueForKey ("Path", oldval.String, true);
		}
		delete oldvar;
	}

	oldvar = FindCVar ("vid_noptc", NULL);
	if (oldvar != NULL)
	{
		delete oldvar;
	}
#endif
}

//GHK
//Used for setting too, to stop user config overwrite for these sections.
void FGameConfigFile::GetSDUserReplacements ()
{


	//std::string sdConfigValues("0");

	//get 'chance' values first
	if(!sd_first_run){
		SetSection ("Doom.ConsoleVariables");

		cvar_set("sp_random_custom_monsters2",GetValueForKey("sp_random_custom_monsters"));
		cvar_set("sd_bossrush2",GetValueForKey("sd_bossrush"));

		cvar_set("sd_defaultmonstermaps_replace_all2",GetValueForKey("sd_defaultmonstermaps_replace_all"));

		sdConfigValues+="sd_rep_zombieman_chance";
		sdConfigValues+=GetValueForKey("sd_rep_zombieman_chance");

		cvar_set("sd_rep_zombieman_chance2",GetValueForKey("sd_rep_zombieman_chance"));

		sdConfigValues+="sd_rep_wolfensteinss_chance";
		sdConfigValues+=GetValueForKey("sd_rep_wolfensteinss_chance");

		cvar_set("sd_rep_wolfensteinss_chance2",GetValueForKey("sd_rep_wolfensteinss_chance"));


		sdConfigValues+="sd_rep_spidermastermind_chance";
		sdConfigValues+=GetValueForKey("sd_rep_spidermastermind_chance");

		cvar_set("sd_rep_spidermastermind_chance2",GetValueForKey("sd_rep_spidermastermind_chance"));


		sdConfigValues+="sd_rep_shotgunguy_chance";
		sdConfigValues+=GetValueForKey("sd_rep_shotgunguy_chance");

		cvar_set("sd_rep_shotgunguy_chance2",GetValueForKey("sd_rep_shotgunguy_chance"));


		sdConfigValues+="sd_rep_revenant_chance";
		sdConfigValues+=GetValueForKey("sd_rep_revenant_chance");

		cvar_set("sd_rep_revenant_chance2",GetValueForKey("sd_rep_revenant_chance"));


		sdConfigValues+="sd_rep_painelemental_chance";
		sdConfigValues+=GetValueForKey("sd_rep_painelemental_chance");

		cvar_set("sd_rep_painelemental_chance2",GetValueForKey("sd_rep_painelemental_chance"));


		sdConfigValues+="sd_rep_lostsoul_chance";
		sdConfigValues+=GetValueForKey("sd_rep_lostsoul_chance");

		cvar_set("sd_rep_lostsoul_chance2",GetValueForKey("sd_rep_lostsoul_chance"));


		sdConfigValues+="sd_rep_fatso_chance";
		sdConfigValues+=GetValueForKey("sd_rep_fatso_chance");

		cvar_set("sd_rep_fatso_chance2",GetValueForKey("sd_rep_fatso_chance"));


		sdConfigValues+="sd_rep_doomimp_chance";
		sdConfigValues+=GetValueForKey("sd_rep_doomimp_chance");

		cvar_set("sd_rep_doomimp_chance2",GetValueForKey("sd_rep_doomimp_chance"));


		sdConfigValues+="sd_rep_chaingunguy_chance";
		sdConfigValues+=GetValueForKey("sd_rep_chaingunguy_chance");

		cvar_set("sd_rep_chaingunguy_chance2",GetValueForKey("sd_rep_chaingunguy_chance"));


		sdConfigValues+="sd_rep_spectre_chance";
		sdConfigValues+=GetValueForKey("sd_rep_spectre_chance");

		cvar_set("sd_rep_spectre_chance2",GetValueForKey("sd_rep_spectre_chance"));


		sdConfigValues+="sd_rep_demon_chance";
		sdConfigValues+=GetValueForKey("sd_rep_demon_chance");

		cvar_set("sd_rep_demon_chance2",GetValueForKey("sd_rep_demon_chance"));


		sdConfigValues+="sd_rep_cyberdemon_chance";
		sdConfigValues+=GetValueForKey("sd_rep_cyberdemon_chance");

		cvar_set("sd_rep_cyberdemon_chance2",GetValueForKey("sd_rep_cyberdemon_chance"));


		sdConfigValues+="sd_rep_cacodemon_chance";
		sdConfigValues+=GetValueForKey("sd_rep_cacodemon_chance");

		cvar_set("sd_rep_cacodemon_chance2",GetValueForKey("sd_rep_cacodemon_chance"));


		sdConfigValues+="sd_rep_hellknight_chance";
		sdConfigValues+=GetValueForKey("sd_rep_hellknight_chance");

		cvar_set("sd_rep_hellknight_chance2",GetValueForKey("sd_rep_hellknight_chance"));


		sdConfigValues+="sd_rep_baronofhell_chance";
		sdConfigValues+=GetValueForKey("sd_rep_baronofhell_chance");

		cvar_set("sd_rep_baronofhell_chance2",GetValueForKey("sd_rep_baronofhell_chance"));


		sdConfigValues+="sd_rep_archvile_chance";
		sdConfigValues+=GetValueForKey("sd_rep_archvile_chance");

		cvar_set("sd_rep_archvile_chance2",GetValueForKey("sd_rep_archvile_chance"));


		sdConfigValues+="sd_rep_burningbarrel_chance";
		sdConfigValues+=GetValueForKey("sd_rep_burningbarrel_chance");

		cvar_set("sd_rep_burningbarrel_chance2",GetValueForKey("sd_rep_burningbarrel_chance"));


		sdConfigValues+="sd_rep_explosivebarrel_chance";
		sdConfigValues+=GetValueForKey("sd_rep_explosivebarrel_chance");

		cvar_set("sd_rep_explosivebarrel_chance2",GetValueForKey("sd_rep_explosivebarrel_chance"));


		sdConfigValues+="sd_rep_arachnotron_chance";
		sdConfigValues+=GetValueForKey("sd_rep_arachnotron_chance");

		cvar_set("sd_rep_arachnotron_chance2",GetValueForKey("sd_rep_arachnotron_chance"));


		sdConfigValues+="sd_artifact_chance";
		sdConfigValues+=GetValueForKey("sd_artifact_chance");

		cvar_set("sd_artifact_chance2",GetValueForKey("sd_artifact_chance"));


		sdConfigValues+="sd_extrahealth_chance";
		sdConfigValues+=GetValueForKey("sd_extrahealth_chance");

		cvar_set("sd_extrahealth_chance2",GetValueForKey("sd_extrahealth_chance"));

		sdConfigValues+="sd_kill_artifact_chance";
		sdConfigValues+=GetValueForKey("sd_kill_artifact_chance");

		cvar_set("sd_kill_artifact_chance2",GetValueForKey("sd_kill_artifact_chance"));


		sdConfigValues+="sd_kill_extrahealth_chance";
		sdConfigValues+=GetValueForKey("sd_kill_extrahealth_chance");

		cvar_set("sd_kill_extrahealth_chance2",GetValueForKey("sd_kill_extrahealth_chance"));


	}

	if (!SetSection ("BaronChainReplacements")){
		SetSection("BaronChainReplacements",true);
		SetValueForKey("BaronChain1AC","1",false);
		SetValueForKey("BaronChain1BC","1",false);

		const char *key;
		const char *value;

		SetSection ("BaronChainReplacements");
		while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
		}

	}else{
		if(SetSection("BaronChainReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdBaronChainReps[count]=key;
						sdBaronChainChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdBaronChainRepCount=count;
				sdBaronChainRange=chancesum;
		}
	}

	if (!SetSection ("CyberChainReplacements")){
		SetSection("CyberChainReplacements",true);
		SetValueForKey("CyberChain1bSTART","1",false);

		const char *key;
		const char *value;

		SetSection ("CyberChainReplacements");
		while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
		}

	}else{
		if(SetSection("CyberChainReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdCyberChainReps[count]=key;
						sdCyberChainChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdCyberChainRepCount=count;
				sdCyberChainRange=chancesum;
		}
	}

	if (!SetSection ("SpiderChainReplacements")){
		SetSection("SpiderChainReplacements",true);
		SetValueForKey("SpiderChain1bSTART","1",false);

		const char *key;
		const char *value;

		SetSection ("SpiderChainReplacements");
		while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
		}

	}else{
		if(SetSection("SpiderChainReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdSpiderChainReps[count]=key;
						sdSpiderChainChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdSpiderChainRepCount=count;
				sdSpiderChainRange=chancesum;
		}
	}

	if (!SetSection ("ExtraHealthReplacements")){
		SetSection("ExtraHealthReplacements",true);
		//SetValueForKey("GHKCustClass","2",false);
		SetValueForKey("StimPack","3",false);
		SetValueForKey("Medikit","1",false);
		SetValueForKey("GreenArmor","1",false);

		const char *key;
		const char *value;

			SetSection ("ExtraHealthReplacements");
			while (NextInSection (key, value)){
				sdConfigValues+=key;
				sdConfigValues+=value;
			}


	}else{
		if(SetSection("ExtraHealthReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdExtraHealthReps[count]=key;
						sdExtraHealthRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdExtraHealthRepCount=count;
				sdExtraHealthRange=chancesum;
		}

	}

	if (!SetSection ("ArtiReplacements")){
		SetSection("ArtiReplacements",true);
		SetValueForKey("TerrorSphere","6",true);
		//SetValueForKey("ArtiSpeedBoots2","6",true);
		//SetValueForKey("ArtiDarkServant2","6",true);
		SetValueForKey("QuarterDamage","6",true);
		SetValueForKey("QuadDamage","6",true);
		SetValueForKey("DoubleDamage","6",true);
		SetValueForKey("Regen","6",true);
		SetValueForKey("Drain","6",true);
		SetValueForKey("HalfDamage","6",true);
		SetValueForKey("ShieldSphere","6",true);
		SetValueForKey("SpreadRune","6",true);
		SetValueForKey("BigBackPack2","3",true);
		SetValueForKey("BigBackPack3","3",true);
		SetValueForKey("BerserkSD","2",true);
		SetValueForKey("BlurSphereSD","2",true);
		SetValueForKey("BlueArmor","2",true);
		SetValueForKey("SoulSphereSD","2",true);
		SetValueForKey("MegasphereSD","2:2",true);
		SetValueForKey("SoulSphereSD","2:1",true);
		//SetValueForKey("TimeFreezer","6",true);
		SetValueForKey("SDSmartBomb","6",true);
		//SetValueForKey("ArtiSentryServant","6",true);
		//SetValueForKey("PointsBunny","6",true);
		SetValueForKey("RageRune","6",true);
		SetValueForKey("AmmoSphere","6",true);
		//SetValueForKey("HighJumpRuneSD","6",true);

		const char *key;
		const char *value;

			SetSection ("ArtiReplacements");
			while (NextInSection (key, value)){
				sdConfigValues+=key;
				sdConfigValues+=value;
			}

	}else{
		if(SetSection("ArtiReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdArtiReps[count]=key;
						sdArtiRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArtiReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArtiRepChances[count]);

						count++;
					}

				}
				sdArtiRepCount=count;
				sdArtiRange=chancesum;
		}

	}



	if (!SetSection ("ExplosiveBarrelReplacements")){
		SetSection("ExplosiveBarrelReplacements",true);
		//SetValueForKey("GHKCustClass","2",false);
		const char *key;
		const char *value;

			SetSection ("ExplosiveBarrelReplacements");
			while (NextInSection (key, value)){
				sdConfigValues+=key;
				sdConfigValues+=value;
			}
	}else{
		if(SetSection("ExplosiveBarrelReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdExplosiveBarrelReps[count]=key;
						sdExplosiveBarrelRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdExplosiveBarrelRepCount=count;
				sdExplosiveBarrelRange=chancesum;
		}

	}


	if (!SetSection ("BurningBarrelReplacements")){
		SetSection("BurningBarrelReplacements",true);
		//SetValueForKey("Swarm","1",false);
		//SetValueForKey("TeslaCoil","1",false);
		//SetValueForKey("GHKCustClass","2",false);
		const char *key;
		const char *value;

			SetSection ("BurningBarrelReplacements");
			while (NextInSection (key, value)){
				sdConfigValues+=key;
				sdConfigValues+=value;
			}
	}else{
		if(SetSection("BurningBarrelReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdBurningBarrelReps[count]=key;
						sdBurningBarrelRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdBurningBarrelRepCount=count;
				sdBurningBarrelRange=chancesum;
		}
	}

	if (!SetSection ("ArchvileReplacements")){
		SetSection("ArchvileReplacements",true);
		//SetValueForKey("Deathknight","1",false);
		SetValueForKey("Diabloist","1",false);
		SetValueForKey("DarkVile0","1",false);
		SetValueForKey("VileKing","1",false);
		//SetValueForKey("ArchSpawner2","1",false);
		SetValueForKey("Vetis","1",false);
		SetValueForKey("DarkArchvile","1",false);
		//SetValueForKey("TornadoDemon","1",false);

		const char *key;
		const char *value;

		SetSection ("ArchvileReplacements");
		while (NextInSection (key, value)){
				sdConfigValues+=key;
				sdConfigValues+=value;

		}

	}else{
		if(SetSection("ArchvileReplacements",true)){
			const char *key;
			const char *value;
			int count=0;
			int chancesum=0;
			int tmpVal = 0;
			bool skip=false;
			//SetValueForKey("GHKCustClass","2");
			while (NextInSection (key, value)){
				sdConfigValues+=key;
				sdConfigValues+=value;
				skip=false;
				//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

				//if not atoi do a check for n:1, n:2
				//, then substr it and do atoi on that
				//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

				char tmpbuf[1000];
				char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
				char *pch = strtok(tmpbuf,":");
				value=pch;

				pch=strtok(NULL,":");

				if(pch!=NULL){

						//if(pch!=NULL){

							if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
								tmpVal = atoi(pch);
						//}

					if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
						if(tmpVal!=2)
							skip=true;
						else
							skip=false;
					}else{ //doom1
						if(tmpVal!=1)
							skip=true;
						else
							skip=false;
					}

				}

				if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
					chancesum+=atoi(value);
					sdArchvileReps[count]=key;
					sdArchvileRepChances[count]=chancesum;

					//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

					//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

					count++;
				}

			}
			sdArchvileRepCount=count;
			sdArchvileRange=chancesum;


			/*
			const char **p;

			int i=0;
			for (p = sdArchvileReps; p < &sdArchvileReps[countof(sdArchvileReps)]; p++){

				if(i>=count)
					break;

				i++;
				if (debugfile) fprintf (debugfile, "custClass:%s",*p);

				//if(*p==NULL)//No more entries.
					//break;

			}

			for(int i=0;i < countof(sdArchvileRepChances); i++){

				if(i>=count)
					break;

				if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchvileRepChances[i]);

				//if(!sdArchiveRepChances[i])//No more entries.
					//break;
			}
			*/
			}
	}
	if (!SetSection ("ArachnotronReplacements")){
		SetSection("ArachnotronReplacements",true);
		//SetValueForKey("ZombieTank","1",false);
		SetValueForKey("FusionSpider","1",false);
		SetValueForKey("Amachotron","1",false);
		SetValueForKey("SpatterTron","1",false);
		SetValueForKey("ArachnotronOfHell","1",false);
		SetValueForKey("SmallInfernalSpider","1",false);
		//SetValueForKey("Crusaderbot","1",false);
		SetValueForKey("BlackWidow","1",false);
		SetValueForKey("CGunSpider","1",false);
		SetValueForKey("RailArachnotron","1",false);
		//SetValueForKey("ZombiePlasmaTank","1",false);
		//SetValueForKey("ZombieTank","1",false);
		SetValueForKey("Decepticon0","1",false);
		//SetValueForKey("Maephisto","1",false);

		const char *key;
		const char *value;

		SetSection ("ArachnotronReplacements");
		while (NextInSection (key, value)){
			sdConfigValues+=key;
				sdConfigValues+=value;
		}

	}else{
		if(SetSection("ArachnotronReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdArachnotronReps[count]=key;
						sdArachnotronRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdArachnotronRepCount=count;
				sdArachnotronRange=chancesum;
		}
	}

	if (!SetSection ("BaronOfHellReplacements")){
		SetSection("BaronOfHellReplacements",true);
		SetValueForKey("AfritHARD","1",false);
		SetValueForKey("ArchonOfHellHARD","1",false);
		SetValueForKey("BelphegorCloneHARD","1",false);
		SetValueForKey("LordofHeresyHARD","1",false);
		SetValueForKey("HellsFuryHARD","1",false);
		SetValueForKey("BruiserDemonHARD","1",false);
		SetValueForKey("BalorHARD","1",false);
		SetValueForKey("WarlordOfHellHARD","1",false);
		SetValueForKey("ArchonOfHell2HARD","1",false);
		SetValueForKey("CyberbaronHARD","1",false);
		SetValueForKey("CybruiserHARD","1",false);
		SetValueForKey("PyroDemonHARD","1",false);
		SetValueForKey("AzazelHARD","1",false);
		SetValueForKey("ProfaneOneHARD","1",false);
		SetValueForKey("DarknessRiftHARD","1:2",false);
		SetValueForKey("DarknessRift1HARD","1:1",false);
		//SetValueForKey("ThorHARD","1",false);
		//SetValueForKey("BormerethHARD","1",false);
		SetValueForKey("TerminatorHARD","1",false);
		SetValueForKey("Desolator2HARD","1",false);
		SetValueForKey("SuperDemonHARD","1",false);
		//SetValueForKey("ExecutionerHARD","1",false);
		//SetValueForKey("SourceGuardianHARD","1",false);
		SetValueForKey("ChainGunnerTankHARD","1:2",false);
		SetValueForKey("ChainGunnerTank2HARD","1:1",false);
		SetValueForKey("BaronOfHellHARD","5",false);
		SetValueForKey("ErkkiBossSpawn","1",false);
		SetValueForKey("ArmouredBaron","1",false);
		SetValueForKey("Obliterator0","1",false);
		SetValueForKey("IceBaron0","1",false);
		SetValueForKey("LordOfHell","1",false);
		//SetValueForKey("IceBoss","1",false);
		//SetValueForKey("FTchernobog","1",false);

		const char *key;
		const char *value;

		SetSection ("BaronOfHellReplacements");
		while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
		}

	}else{
		if(SetSection("BaronOfHellReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdBaronOfHellReps[count]=key;
						sdBaronOfHellRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdBaronOfHellRepCount=count;
				sdBaronOfHellRange=chancesum;
		}
	}
	if (!SetSection ("HellKnightReplacements")){
		SetSection("HellKnightReplacements",true);
		//SetValueForKey("TornadoDemon","1",false);
		SetValueForKey("HornBeast","1",false);
		SetValueForKey("ObsidianStatueNonDormant","1",false);
		//SetValueForKey("Bormereth","1",false);
		SetValueForKey("Phantom","1",false);
		SetValueForKey("Terminator","1",false);
		//SetValueForKey("TornadoDemon","1",false);
		SetValueForKey("ShadowBeast","1",false);
		SetValueForKey("Golem","1",false);
		//SetValueForKey("Desolator","1",false);
		//SetValueForKey("ZombiePlasmaTank","1",false);
		//SetValueForKey("Vampire","1",false);
		//SetValueForKey("Paladin","1",false);
		SetValueForKey("ErkkiBoss","1:2",false);
	    SetValueForKey("HellChampion0","1:2",false);
		SetValueForKey("Sunhegor","1",false);
		//SetValueForKey("Juggernaut","1",false);
		SetValueForKey("Hierophant","1",false);
		SetValueForKey("Veste","1",false);

		const char *key;
		const char *value;

		SetSection ("HellKnightReplacements");
		while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
		}

	}else{

		if(SetSection("HellKnightReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdHellKnightReps[count]=key;
						sdHellKnightRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdHellKnightRepCount=count;
				sdHellKnightRange=chancesum;
		}
	}

	if (!SetSection ("CacodemonReplacements")){
		SetSection("CacodemonReplacements",true);
		SetValueForKey("PlasmaElemental","1",false);
		SetValueForKey("TorturedSoul","1",false);
		SetValueForKey("Inferno","1",false);
		SetValueForKey("CacolanternClone","1",false);
		SetValueForKey("CacoLich1","1",false);
		SetValueForKey("EnhancedCacodemon","1",false);
		SetValueForKey("Fallen","1",false);
		//SetValueForKey("Watcher","1",false);
		//SetValueForKey("Wyvern1","1",false);
		SetValueForKey("Helemental","1",false);
		SetValueForKey("Agathodemon","1",false);
		SetValueForKey("NightmareCacodemon","1",false);
		SetValueForKey("CacoElemental","1",false);
		SetValueForKey("CrackoDemon","1",false);
		SetValueForKey("SpeedDemon","1",false);
		SetValueForKey("SurrealDemon","1",false);
		//SetValueForKey("ZombieFlyer","1",false);
		//SetValueForKey("Wicked","1",false);
		SetValueForKey("Quasit","1",false);
		SetValueForKey("Defiler","1",false);
		//SetValueForKey("Poe","1",false);
		//SetValueForKey("FlyingImp","1",false);
		//SetValueForKey("Wicked","1",false);
		SetValueForKey("Aracnorb","1",false);
		SetValueForKey("D3Cacodemon","1",false);
		//SetValueForKey("NetherSyst","1",false);
		SetValueForKey("GriefElemental","1",false);
		//SetValueForKey("DeathElemental0","1",false);
		SetValueForKey("TechnoCaco0","1",false);
		//SetValueForKey("ApprenticeofDsparil","1",true);
		//SetValueForKey("Chiller","1",false);
		SetValueForKey("XenoDemon","1",false);
		SetValueForKey("SolarDemon","1",false);
		//SetValueForKey("Chesire","1",false);

		const char *key;
		const char *value;

		SetSection ("CacodemonReplacements");
		while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
		}

	}else{
		if(SetSection("CacodemonReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdCacodemonReps[count]=key;
						sdCacodemonRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdCacodemonRepCount=count;
			sdCacodemonRange=chancesum;
		}
	}

	if (!SetSection ("CyberdemonReplacements")){
		SetSection("CyberdemonReplacements",true);
		//SetValueForKey("ApprenticeHARD","1",false);
		SetValueForKey("AvatarHARD","1",false);
		SetValueForKey("HellSmith1HARD","1",false);
		SetValueForKey("ThamuzHARD","1",false);
		SetValueForKey("InfernuxGrandHARD","1",false);
		SetValueForKey("AnnihilatorHARD","1",false);
		SetValueForKey("AzaniginHARD","1",false);
		SetValueForKey("InfernoDemonHARD","1",false);
		SetValueForKey("CyberDemonHARD","1",false);
		SetValueForKey("CardinalHARD","1",false);
		SetValueForKey("MolochHARD","1",false);
		//SetValueForKey("ScourgeBossHARD","1",false);
		SetValueForKey("SuperCyber","1",false);
		//SetValueForKey("HammerHound","1",false);
		//SetValueForKey("HellsBattery","1",false);
		//SetValueForKey("Baphomet","1",false); //doesnt move, so keep to boss-rush only
		SetValueForKey("Cyberbot","1",false);
		SetValueForKey("DeathBringer","1",false);
		SetValueForKey("GateKeeper","1",false);
		SetValueForKey("DarkInquisitor","1",false);

		const char *key;
		const char *value;

		SetSection ("CyberdemonReplacements");
		while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;

		}

	}else{
		if(SetSection("CyberdemonReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;

					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdCyberdemonReps[count]=key;
						sdCyberdemonRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdCyberdemonRepCount=count;
			sdCyberdemonRange=chancesum;
		}
	}
	if (!SetSection ("DemonReplacements")){
		SetSection("DemonReplacements",true);
		SetValueForKey("HellGuard","1",false);
		//SetValueForKey("HellWarrior","1",false);
		SetValueForKey("BloodDemonClone","1",false);
		SetValueForKey("Bloodfiend","1",false);
		SetValueForKey("HotDog","1",false);
		SetValueForKey("MaulerDemon","1",false);
		SetValueForKey("NightmareDemon","1",false);
		//SetValueForKey("HellRoseSpid","2",false);
		//SetValueForKey("HellRose","1",false);
		SetValueForKey("StoneDemon","1",false);
		//SetValueForKey("KDIZDNightmare","1",false);
		SetValueForKey("PlasmaDemon","1",false);
		SetValueForKey("SlimeWorm","1",false);
		SetValueForKey("Satyr","1",false);
		//SetValueForKey("ZombieFlyer","1",false);
		SetValueForKey("Lurker","1",false);
		SetValueForKey("Watcher","1",false);
		SetValueForKey("Squire","1",false);
		SetValueForKey("ChargeDemon0","1",false);
		SetValueForKey("DarkClink","1",false);
		SetValueForKey("Wicked","1",false);
		//SetValueForKey("HeadMan","1",false);
		//SetValueForKey("Veste","1",false);
		//SetValueForKey("LavaDemon","1",false);


		const char *key;
		const char *value;

		SetSection ("DemonReplacements");
		while (NextInSection (key, value)){
				sdConfigValues+=key;
					sdConfigValues+=value;
		}

	}else{
		if(SetSection("DemonReplacements",true)){
					const char *key;
					const char *value;
					int count=0;
					int chancesum=0;
					int tmpVal = 0;
					bool skip=false;
					//SetValueForKey("GHKCustClass","2");
					while (NextInSection (key, value)){
						sdConfigValues+=key;
						sdConfigValues+=value;
						skip=false;
						//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

						//if not atoi do a check for n:1, n:2
						//, then substr it and do atoi on that
						//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

						char tmpbuf[1000];
						char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
						char *pch = strtok(tmpbuf,":");
						value=pch;

						pch=strtok(NULL,":");

						if(pch!=NULL){

								//if(pch!=NULL){

									if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
										tmpVal = atoi(pch);
								//}

							if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
								if(tmpVal!=2)
									skip=true;
								else
									skip=false;
							}else{ //doom1
								if(tmpVal!=1)
									skip=true;
								else
									skip=false;
							}

						}

						if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
							chancesum+=atoi(value);
							sdDemonReps[count]=key;
							sdDemonRepChances[count]=chancesum;

							//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

							//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

							count++;
						}

					}
				sdDemonRepCount=count;
				sdDemonRange=chancesum;
			}
	}

	if (!SetSection ("SpectreReplacements")){
		SetSection("SpectreReplacements",true);
		SetValueForKey("HellGuard","1",false);
		//SetValueForKey("HellWarrior","1",false);
		SetValueForKey("BloodDemonClone","1",false);
		SetValueForKey("Bloodfiend","1",false);
		SetValueForKey("HotDog","1",false);
		SetValueForKey("MaulerDemon","1",false);
		SetValueForKey("NightmareDemon","1",false);
		//SetValueForKey("HellRoseSpid","2",false);
		//SetValueForKey("HellRose","1",false);
		SetValueForKey("StoneDemon","1",false);
		//SetValueForKey("KDIZDNightmare","1",false);
		SetValueForKey("PlasmaDemon","1",false);
		SetValueForKey("SlimeWorm","1",false);
		SetValueForKey("Satyr","1",false);
		//SetValueForKey("ZombieFlyer","1",false);
		SetValueForKey("Lurker","1",false);
		SetValueForKey("Watcher","1",false);
		SetValueForKey("Squire","1",false);
		SetValueForKey("ChargeDemon0","1",false);
		SetValueForKey("DarkClink","1",false);
		SetValueForKey("Wicked","1",false);
		//SetValueForKey("HeadMan","1",false);
		//SetValueForKey("Veste","1",false);
		//SetValueForKey("LavaDemon","1",false);

		const char *key;
		const char *value;

		SetSection ("SpectreReplacements");
		while (NextInSection (key, value)){
				sdConfigValues+=key;
					sdConfigValues+=value;

		}

	}else{
		if(SetSection("SpectreReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;

					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdSpectreReps[count]=key;
						sdSpectreRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdSpectreRepCount=count;
			sdSpectreRange=chancesum;
		}
	}
	if (!SetSection ("ChaingunGuyReplacements")){
		SetSection("ChaingunGuyReplacements",true);
		SetValueForKey("ChaingunGuy2","16",false);
			//SetValueForKey("FCommander2","4",false);
			SetValueForKey("ChaingunMajor","16",false);
			//SetValueForKey("Disciple","4",false);
			//SetValueForKey("MarineBFG2SD","4",false);
			//SetValueForKey("LaserCannonZombie","4",false);
			//SetValueForKey("PlasmaZombie","4",false);
			//SetValueForKey("DBTPawn","4",false);
			//SetValueForKey("UndeadPriest","4",false);
			//SetValueForKey("UnmakerGuy","4",false);
			//SetValueForKey("XimRocketGuy","4",false);
			SetValueForKey("RepeaterZombie","6",false);
			//SetValueForKey("SonicRailgunZombie","4",false);
			//SetValueForKey("JetpackZombie","4",false);
			SetValueForKey("NailBorgCommando","6",false);
			//SetValueForKey("NailBorg","4",false);
			//SetValueForKey("Aldhivas","2",true);
			SetValueForKey("EvilDoomGuy","1",true);
			SetValueForKey("BFGGuy","4",true); //extra
			//SetValueForKey("QuadShotgunZombie","4",true); //extra
			SetValueForKey("ScientistZombie","1",true);
			SetValueForKey("ScientistZombie2","1",true);
			SetValueForKey("ScientistZombie3","1",true);
			SetValueForKey("DevastatorZombie","4",true);
			SetValueForKey("ZombieTank","4",true);
			SetValueForKey("ZombiePlasmaTank","4",true);
			SetValueForKey("SpreadFireZombie0","8",true);
			SetValueForKey("ZombieFlyer","4",true);

		const char *key;
		const char *value;

		SetSection ("ChaingunGuyReplacements");
		while (NextInSection (key, value)){
				sdConfigValues+=key;
					sdConfigValues+=value;

		}

	}else{
		if(SetSection("ChaingunGuyReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;

					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdChaingunGuyReps[count]=key;
						sdChaingunGuyRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdChaingunGuyRepCount=count;
			sdChaingunGuyRange=chancesum;
		}
	}
	if (!SetSection ("DoomImpReplacements")){
		SetSection("DoomImpReplacements",true);
		SetValueForKey("PhaseImp","2",false);
		SetValueForKey("NamiDarkImp","2",false);
		SetValueForKey("STDarkImp","2",false);
		SetValueForKey("VoidDarkImp","2",false);
		SetValueForKey("Devil","2",false);
		SetValueForKey("Shadow","2",false);
		SetValueForKey("SlimeImp","2",false);
		SetValueForKey("SnakeImp","2",false);
		SetValueForKey("StoneImp","2",false);
		SetValueForKey("SuperImp","2",false);
		SetValueForKey("SoulHarvester","2",false);
		//SetValueForKey("NetherworldDrone","2",false);
		//SetValueForKey("ImpWarlord","2",false);
		//SetValueForKey("Catharsi","2",false);
		//SetValueForKey("Ghoul","2",false);
		SetValueForKey("ArmoredImp","2",false);
		SetValueForKey("Vulgar","2",false);
		SetValueForKey("CycloImp","2",false);
		SetValueForKey("SpiritImp","2",false);
		SetValueForKey("FoxImp","2",false);
		SetValueForKey("Illus","2",false);
		SetValueForKey("Scourge","2",false);
		SetValueForKey("FlyingImp","2",false);
		//SetValueForKey("XWizard","1",false);
		//SetValueForKey("FleshWizard","1",false);
		SetValueForKey("Hellion","2",false);
		SetValueForKey("Roach","2",false);
		SetValueForKey("CyberImp","2",false);
		SetValueForKey("SuperFlyingImp","2",false);
		SetValueForKey("D3Wraith","2",false);
		SetValueForKey("LesserMutant","2",false);
		SetValueForKey("Agaures","2",false);
		SetValueForKey("PyroImp","2",false);
		//SetValueForKey("Loaper","2",false);
		//SetValueForKey("NukageBeast","2",false);
		SetValueForKey("FrozenImp","2",false);
		SetValueForKey("NetherDarkImp","2",false);
		SetValueForKey("Nightshade","2",false);
		SetValueForKey("FleshSpawn","2",false);
		SetValueForKey("SDSpidImpHeadSpawner","2",false);
		SetValueForKey("SDTriteSpawner","2",false);
		SetValueForKey("HereticDarkImp","2",false);
		SetValueForKey("VoidImp","3",false);
		SetValueForKey("STImp","3",false);
		SetValueForKey("ImpAbomination","3",false);
		SetValueForKey("KDIZDNightmare","2",false);
		SetValueForKey("SDPustuleSpawner","2",false);

		const char *key;
		const char *value;

		SetSection ("DoomImpReplacements");
		while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;

		}

	}else{
		if(SetSection("DoomImpReplacements",true)){
			const char *key;
			const char *value;
			int count=0;
			int chancesum=0;
			int tmpVal = 0;
			bool skip=false;
			//SetValueForKey("GHKCustClass","2");
			while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;

				skip=false;
				//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

				//if not atoi do a check for n:1, n:2
				//, then substr it and do atoi on that
				//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

				char tmpbuf[1000];
				char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
				char *pch = strtok(tmpbuf,":");
				value=pch;

				pch=strtok(NULL,":");

				if(pch!=NULL){

						//if(pch!=NULL){

							if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
								tmpVal = atoi(pch);
						//}

					if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
						if(tmpVal!=2)
							skip=true;
						else
							skip=false;
					}else{ //doom1
						if(tmpVal!=1)
							skip=true;
						else
							skip=false;
					}

				}

				if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
					chancesum+=atoi(value);
					sdDoomImpReps[count]=key;
					sdDoomImpRepChances[count]=chancesum;

					//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

					//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

					count++;
				}

			}
			sdDoomImpRepCount=count;
			sdDoomImpRange=chancesum;
		}
	}
	if (!SetSection ("FatsoReplacements")){
		SetSection("FatsoReplacements",true);
		//SetValueForKey("Queen","1",false);
		//SetValueForKey("DESentinel","1",false);
		//SetValueForKey("reaper","1",false);
		//SetValueForKey("inqbot","1",false);
		SetValueForKey("Maxibus","1",false);
		SetValueForKey("Daedabus","1",false);
		SetValueForKey("Hectebus","1",false);
		//SetValueForKey("Incubus","1",false);
		//SetValueForKey("FCerberus","1",false);
		SetValueForKey("Gargantus0","1",false);
		SetValueForKey("UACBot","1",false);
		//SetValueForKey("BloodLich","1",false); //extra
		//SetValueForKey("Cheogh","1",false); //extra
		SetValueForKey("Maephisto","1",false); //extra
		//SetValueForKey("ChaosWyvern","1",false); //extra

		const char *key;
		const char *value;

		SetSection ("FatsoReplacements");
		while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
		}

	}else{
		if(SetSection("FatsoReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdFatsoReps[count]=key;
						sdFatsoRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdFatsoRepCount=count;
			sdFatsoRange=chancesum;
		}
	}
	if (!SetSection ("LostSoulReplacements")){
		SetSection("LostSoulReplacements",true);
		SetValueForKey("ESoul","1",false);
		SetValueForKey("PoisonSoul","1",false);
		SetValueForKey("HS","1",false);
		SetValueForKey("PsychicSoul","1",false);
		SetValueForKey("Terror","1",false);
		SetValueForKey("HellFireSoul","1",false);
		SetValueForKey("EyeEx","1",false);
		SetValueForKey("EyePoison","1",false);
		SetValueForKey("EyeSpider","1",false);
		SetValueForKey("DreamCreeper","1",false);
		SetValueForKey("D3ForgottenOne","1",false);
		SetValueForKey("Rictus","1",false);
		SetValueForKey("Blot","1",false);
		SetValueForKey("BurningSoul0","1",false);
		SetValueForKey("DrownedSoul0","1",false);
		SetValueForKey("SuicideSoul","1",false);
		SetValueForKey("Shade","1",false);
		SetValueForKey("DarkLostSoul","1",false);

		const char *key;
		const char *value;

		SetSection ("LostSoulReplacements");
		while (NextInSection (key, value)){
				sdConfigValues+=key;
				sdConfigValues+=value;

		}

	}else{
		if(SetSection("LostSoulReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdLostSoulReps[count]=key;
						sdLostSoulRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdLostSoulRepCount=count;
			sdLostSoulRange=chancesum;
		}
	}

	if (!SetSection ("PainElementalReplacements")){
		SetSection("PainElementalReplacements",true);
		SetValueForKey("PlasmaElemental","1",false);
		SetValueForKey("TorturedSoul","1",false);
		SetValueForKey("Inferno","1",false);
		SetValueForKey("CacolanternClone","1",false);
		SetValueForKey("CacoLich1","1",false);
		SetValueForKey("EnhancedCacodemon","1",false);
		SetValueForKey("Fallen","1",false);
		//SetValueForKey("Watcher","1",false);
		//SetValueForKey("Wyvern1","1",false);
		SetValueForKey("Helemental","1",false);
		SetValueForKey("Agathodemon","1",false);
		SetValueForKey("NightmareCacodemon","1",false);
		SetValueForKey("CacoElemental","1",false);
		SetValueForKey("CrackoDemon","1",false);
		SetValueForKey("SpeedDemon","1",false);
		SetValueForKey("SurrealDemon","1",false);
		//SetValueForKey("ZombieFlyer","1",false);
		//SetValueForKey("Wicked","1",false);
		SetValueForKey("Quasit","1",false);
		SetValueForKey("Defiler","1",false);
		//SetValueForKey("Poe","1",false);
		//SetValueForKey("FlyingImp","1",false);
		//SetValueForKey("Wicked","1",false);
		SetValueForKey("Aracnorb","1",false);
		SetValueForKey("D3Cacodemon","1",false);
		//SetValueForKey("NetherSyst","1",false);
		SetValueForKey("GriefElemental","1",false);
		//SetValueForKey("DeathElemental0","1",false);
		SetValueForKey("TechnoCaco0","1",false);
		//SetValueForKey("ApprenticeofDsparil","1",true);
		//SetValueForKey("Chiller","1",false);
		SetValueForKey("XenoDemon","1",false);
		SetValueForKey("SolarDemon","1",false);
		//SetValueForKey("Chesire","1",false);


		const char *key;
		const char *value;

		SetSection ("PainElementalReplacements");
		while (NextInSection (key, value)){
			sdConfigValues+=key;
				sdConfigValues+=value;

		}

	}else{
		if(SetSection("PainElementalReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdPainElementalReps[count]=key;
						sdPainElementalRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdPainElementalRepCount=count;
			sdPainElementalRange=chancesum;
		}
	}
	if (!SetSection ("RevenantReplacements")){
		SetSection("RevenantReplacements",true);
		//SetValueForKey("ZombieTank","1",false);
		SetValueForKey("Malevonant","1",false);
		//SetValueForKey("DarkClink","1",false);
		SetValueForKey("NightmareBeast","1",false);
		SetValueForKey("Incarnate","1",false);
		//SetValueForKey("Vampire","1",false);
		//SetValueForKey("DemonTrickster","1",false);
		//SetValueForKey("LavaDemon","1",false);
		//SetValueForKey("RotWraith","1",false);
		SetValueForKey("DarkRevenant","1",false);

		const char *key;
		const char *value;

		SetSection ("RevenantReplacements");
		while (NextInSection (key, value)){
			sdConfigValues+=key;
				sdConfigValues+=value;

		}

	}else{
		if(SetSection("RevenantReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdRevenantReps[count]=key;
						sdRevenantRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdRevenantRepCount=count;
			sdRevenantRange=chancesum;
		}
	}
	if (!SetSection ("ShotgunGuyReplacements")){
		SetSection("ShotgunGuyReplacements",true);
		SetValueForKey("SSGZombie","16",true);
		SetValueForKey("FCommander2","5",true);
		SetValueForKey("RepeaterZombie","4",true);
		SetValueForKey("XimRocketGuy","6",true);
		SetValueForKey("JetpackZombie","4",true);
		SetValueForKey("NailBorg","8",true);
		SetValueForKey("SonicRailgunZombie","6",true);
		SetValueForKey("Railbot","4",true);
		SetValueForKey("SawedOffShotgunGuy","16",true);
		SetValueForKey("NailBorgCommando","4",true);
		SetValueForKey("EvilDoomGuy","1",true);
		SetValueForKey("ScientistZombie","1",true);
		SetValueForKey("ScientistZombie2","1",true);
		SetValueForKey("ScientistZombie3","1",true);
		SetValueForKey("SpreadFireZombie0","4",true);
		SetValueForKey("FemaleSergeant","6",true);
		SetValueForKey("QuadShotgunZombie","16",true);
		SetValueForKey("ASGGuy","16",true);
		SetValueForKey("DroneSpawnerSD","4",true);
		SetValueForKey("ZombieFlyer","4",true);
		SetValueForKey("ZombieTank","2",true);
		SetValueForKey("ZombiePlasmaTank","2",true);
		SetValueForKey("BFGGuy","4",true);
		SetValueForKey("DevastatorZombie","2",true);
		SetValueForKey("DuneWarrior","4",true);


		const char *key;
		const char *value;

		SetSection ("ShotgunGuyReplacements");
		while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;

		}

	}else{
		if(SetSection("ShotgunGuyReplacements",true)){
			const char *key;
			const char *value;
			int count=0;
			int chancesum=0;
			int tmpVal = 0;
			bool skip=false;
			//SetValueForKey("GHKCustClass","2");
			while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
				skip=false;
				//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

				//if not atoi do a check for n:1, n:2
				//, then substr it and do atoi on that
				//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

				char tmpbuf[1000];
				char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
				char *pch = strtok(tmpbuf,":");
				value=pch;

				pch=strtok(NULL,":");

				if(pch!=NULL){

						//if(pch!=NULL){

							if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
								tmpVal = atoi(pch);
						//}

					if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
						if(tmpVal!=2)
							skip=true;
						else
							skip=false;
					}else{ //doom1
						if(tmpVal!=1)
							skip=true;
						else
							skip=false;
					}

				}

				if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
					chancesum+=atoi(value);
					sdShotgunGuyReps[count]=key;
					sdShotgunGuyRepChances[count]=chancesum;

					//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

					//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

					count++;
				}

			}
			sdShotgunGuyRepCount=count;
			sdShotgunGuyRange=chancesum;
		}
	}

	if (!SetSection ("SpiderMastermindReplacements")){
		SetSection("SpiderMastermindReplacements",true);
		SetValueForKey("CyberMastermindHARD","5",false);
		SetValueForKey("DemolisherHARD","5",false);
		SetValueForKey("SupremeFiendHARD","5",false);
		SetValueForKey("BigInfernalSpiderHARD","5",false);
		SetValueForKey("SpiderMastermindHARD","5",false);
		SetValueForKey("ArachnophyteHARD","5",false);
		SetValueForKey("ScourgeBossHARD","5",false);
		//SetValueForKey("AnnihilatorHARD","1",false);
		//SetValueForKey("AzaniginHARD","1",false);
		//SetValueForKey("ThamuzHARD","1",false);
		//SetValueForKey("InfernuxGrandHARD","1",false);
		//SetValueForKey("ApprenticeHARD","1",false);
		//SetValueForKey("Hellsmith1HARD","1",false);
		//SetValueForKey("AvatarHARD","1",false);
		SetValueForKey("OverLordHARD","5",false);
		//SetValueForKey("InfernoDemonHARD","1",false);
		//SetValueForKey("CardinalHARD","1",false);
		//SetValueForKey("MolochHARD","1",false);
		//SetValueForKey("CardinalHARD","1",false);
		//SetValueForKey("SuperCyber","1",false);
		SetValueForKey("InsanitySpider","5",false);
		SetValueForKey("InsanityArachnophyte","5",false);
		SetValueForKey("HammerHound","1",false);
		SetValueForKey("HellsBattery","1",false);
		//SetValueForKey("Baphomet","1",false);
		//SetValueForKey("Cyberbot","1",false);
		//SetValueForKey("DeathBringer","1",false);

		const char *key;
		const char *value;

		SetSection ("SpiderMastermindReplacements")	;
		while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;

		}

	}else{
		if(SetSection("SpiderMastermindReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdSpiderMastermindReps[count]=key;
						sdSpiderMastermindRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdSpiderMastermindRepCount=count;
			sdSpiderMastermindRange=chancesum;
		}

	}
	if (!SetSection ("WolfensteinSSReplacements")){
		SetSection("WolfensteinSSReplacements",true);
		//SetValueForKey("GHKCustClass","2",false);
		const char *key;
		const char *value;

		SetSection ("WolfensteinSSReplacements");
		while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;

		}

	}else{
		if(SetSection("WolfensteinSSReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdWolfensteinSSReps[count]=key;
						sdWolfensteinSSRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdWolfensteinSSRepCount=count;
			sdWolfensteinSSRange=chancesum;
		}
	}
	if (!SetSection ("ZombieManReplacements")){
		SetSection("ZombieManReplacements",true);
		SetValueForKey("ChainsawZombie","4",true);
		SetValueForKey("RapidFireTrooper","30",true);
		SetValueForKey("PlasmaZombie2","3",true);
		SetValueForKey("ZombieRailgunner","8",true);
		SetValueForKey("ZSec","6",true);
		SetValueForKey("ZombieMarine","8",true);
		SetValueForKey("RocketGuy","9",true);
		SetValueForKey("FZombieMan","12",true);
		SetValueForKey("PlasmaZombie","2",true);
		SetValueForKey("AugerGuy","4",true);
		SetValueForKey("SniperRifleGuy","3",true);
		SetValueForKey("DemonDog","4",true);
		SetValueForKey("FreezeRifleGuy","3",true);
		SetValueForKey("ZSoldier","4",true);
		SetValueForKey("ScientistZombie","2",true);
		SetValueForKey("ScientistZombie2","1",true);
		SetValueForKey("ScientistZombie3","1",true);
		SetValueForKey("FlamerZombie","3",true);
		SetValueForKey("ZombieHenchmanSmaller","4",true);
		SetValueForKey("FemaleZombie","6",true);
		SetValueForKey("ZSpecOpsMachinegun","3",true);
		SetValueForKey("KarasawaGuy","3",true);
		SetValueForKey("BeamGuy","3",true);
		SetValueForKey("SmokeMonster","2",true);
		SetValueForKey("SMGGuy","20",true);
		SetValueForKey("FemaleRail","3",true);
		SetValueForKey("UnmakerGuy","3",true);
		SetValueForKey("SuicideBomber","3",true);
		SetValueForKey("FemalePlasma","3",true);

		const char *key;
		const char *value;

		SetSection("ZombieManReplacements");
		while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;

		}

	}else{
		if(SetSection("ZombieManReplacements",true)){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdZombieManReps[count]=key;
						sdZombieManRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdZombieManRepCount=count;
			sdZombieManRange=chancesum;
		}

	}

		//if(*(sd_configvalues_md5)==0)
			//sd_configvalues_md5="0";

		md5wrapper md5;
		//std::string ghktemp(md5.getHashFromString(sdConfigValues.c_str()));
		sdConfigValues=md5.getHashFromString(sdConfigValues.c_str());
		sd_configvalues_md5=sdConfigValues.c_str();
		//sd_configvalues_md5=ghktemp.c_str();
		//confighash=ghktemp.c_str();

		//return confighash;

}

//GHK
//Used for setting custom monster lists from a sdcustom.ini file
void FGameConfigFile::GetSDUserReplacementsSDM (FGameConfigFile *SDMConfig)
{


	//std::string sdConfigValues("0");

	//get 'chance' values first
	if(!sd_first_run){

		SDMConfig->SetSection ("Doom.ConsoleVariables");

		cvar_set("sp_random_custom_monsters2",SDMConfig->GetValueForKey("sp_random_custom_monsters"));
		cvar_set("sd_bossrush2",SDMConfig->GetValueForKey("sd_bossrush"));

		cvar_set("sd_defaultmonstermaps_replace_all2",SDMConfig->GetValueForKey("sd_defaultmonstermaps_replace_all"));

		sdConfigValues+="sd_rep_zombieman_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_zombieman_chance");

		//sd_rep_zombieman_chance.UnSetArchiveBit();
		//GHK USE cvar_forceset instead?
		cvar_set("sd_rep_zombieman_chance2",SDMConfig->GetValueForKey("sd_rep_zombieman_chance"));


		sdConfigValues+="sd_rep_wolfensteinss_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_wolfensteinss_chance");

		//sd_rep_wolfensteinss_chance.UnSetArchiveBit();
		cvar_set("sd_rep_wolfensteinss_chance2",SDMConfig->GetValueForKey("sd_rep_wolfensteinss_chance"));


		sdConfigValues+="sd_rep_spidermastermind_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_spidermastermind_chance");

		//sd_rep_spidermastermind_chance.UnSetArchiveBit();
		cvar_set("sd_rep_spidermastermind_chance2",SDMConfig->GetValueForKey("sd_rep_spidermastermind_chance"));


		sdConfigValues+="sd_rep_shotgunguy_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_shotgunguy_chance");

		//sd_rep_shotgunguy_chance.UnSetArchiveBit();
		cvar_set("sd_rep_shotgunguy_chance2",SDMConfig->GetValueForKey("sd_rep_shotgunguy_chance"));



		sdConfigValues+="sd_rep_revenant_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_revenant_chance");

		//sd_rep_revenant_chance.UnSetArchiveBit();
		cvar_set("sd_rep_revenant_chance2",SDMConfig->GetValueForKey("sd_rep_revenant_chance"));


		sdConfigValues+="sd_rep_painelemental_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_painelemental_chance");

		//sd_rep_painelemental_chance.UnSetArchiveBit();
		cvar_set("sd_rep_painelemental_chance2",SDMConfig->GetValueForKey("sd_rep_painelemental_chance"));


		sdConfigValues+="sd_rep_lostsoul_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_lostsoul_chance");

		//sd_rep_lostsoul_chance.UnSetArchiveBit();
		cvar_set("sd_rep_lostsoul_chance2",SDMConfig->GetValueForKey("sd_rep_lostsoul_chance"));


		sdConfigValues+="sd_rep_fatso_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_fatso_chance");

		//sd_rep_fatso_chance.UnSetArchiveBit();
		cvar_set("sd_rep_fatso_chance2",SDMConfig->GetValueForKey("sd_rep_fatso_chance"));


		sdConfigValues+="sd_rep_doomimp_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_doomimp_chance");

		//sd_rep_doomimp_chance.UnSetArchiveBit();
		cvar_set("sd_rep_doomimp_chance2",SDMConfig->GetValueForKey("sd_rep_doomimp_chance"));


		sdConfigValues+="sd_rep_chaingunguy_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_chaingunguy_chance");

		//sd_rep_chaingunguy_chance.UnSetArchiveBit();
		cvar_set("sd_rep_chaingunguy_chance2",SDMConfig->GetValueForKey("sd_rep_chaingunguy_chance"));



		sdConfigValues+="sd_rep_spectre_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_spectre_chance");

		//sd_rep_spectre_chance.UnSetArchiveBit();
		cvar_set("sd_rep_spectre_chance2",SDMConfig->GetValueForKey("sd_rep_spectre_chance"));


		sdConfigValues+="sd_rep_demon_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_demon_chance");

		//sd_rep_demon_chance.UnSetArchiveBit();
		cvar_set("sd_rep_demon_chance2",SDMConfig->GetValueForKey("sd_rep_demon_chance"));


		sdConfigValues+="sd_rep_cyberdemon_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_cyberdemon_chance");

		//sd_rep_cyberdemon_chance.UnSetArchiveBit();
		cvar_set("sd_rep_cyberdemon_chance2",SDMConfig->GetValueForKey("sd_rep_cyberdemon_chance"));



		sdConfigValues+="sd_rep_cacodemon_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_cacodemon_chance");

		//sd_rep_cacodemon_chance.UnSetArchiveBit();
		cvar_set("sd_rep_cacodemon_chance2",SDMConfig->GetValueForKey("sd_rep_cacodemon_chance"));


		sdConfigValues+="sd_rep_hellknight_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_hellknight_chance");

		//sd_rep_hellknight_chance.UnSetArchiveBit();
		cvar_set("sd_rep_hellknight_chance2",SDMConfig->GetValueForKey("sd_rep_hellknight_chance"));


		sdConfigValues+="sd_rep_baronofhell_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_baronofhell_chance");

		//sd_rep_baronofhell_chance.UnSetArchiveBit();
		cvar_set("sd_rep_baronofhell_chance2",SDMConfig->GetValueForKey("sd_rep_baronofhell_chance"));


		sdConfigValues+="sd_rep_archvile_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_archvile_chance");

		//sd_rep_archvile_chance.UnSetArchiveBit();
		cvar_set("sd_rep_archvile_chance2",SDMConfig->GetValueForKey("sd_rep_archvile_chance"));


		sdConfigValues+="sd_rep_burningbarrel_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_burningbarrel_chance");

		//sd_rep_burningbarrel_chance.UnSetArchiveBit();
		cvar_set("sd_rep_burningbarrel_chance2",SDMConfig->GetValueForKey("sd_rep_burningbarrel_chance"));


		sdConfigValues+="sd_rep_explosivebarrel_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_explosivebarrel_chance");

		//sd_rep_explosivebarrel_chance.UnSetArchiveBit();
		cvar_set("sd_rep_explosivebarrel_chance2",SDMConfig->GetValueForKey("sd_rep_explosivebarrel_chance"));


		sdConfigValues+="sd_rep_arachnotron_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_rep_arachnotron_chance");

		//sd_rep_arachnotron_chance.UnSetArchiveBit();
		cvar_set("sd_rep_arachnotron_chance2",SDMConfig->GetValueForKey("sd_rep_arachnotron_chance"));


		sdConfigValues+="sd_artifact_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_artifact_chance");

		//sd_artifact_chance.UnSetArchiveBit();
		cvar_set("sd_artifact_chance2",SDMConfig->GetValueForKey("sd_artifact_chance"));


		sdConfigValues+="sd_extrahealth_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_extrahealth_chance");

		//sd_extrahealth_chance.UnSetArchiveBit();
		cvar_set("sd_extrahealth_chance2",SDMConfig->GetValueForKey("sd_extrahealth_chance"));

		sdConfigValues+="sd_kill_artifact_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_kill_artifact_chance");

		cvar_set("sd_kill_artifact_chance2",SDMConfig->GetValueForKey("sd_kill_artifact_chance"));


		sdConfigValues+="sd_kill_extrahealth_chance";
		sdConfigValues+=SDMConfig->GetValueForKey("sd_kill_extrahealth_chance");

		cvar_set("sd_kill_extrahealth_chance2",SDMConfig->GetValueForKey("sd_kill_extrahealth_chance"));


	}


		if(SDMConfig->SetSection("BaronChainReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdBaronChainReps[count]=key;
						sdBaronChainChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdBaronChainRepCount=count;
				sdBaronChainRange=chancesum;
		}



		if(SDMConfig->SetSection("CyberChainReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdCyberChainReps[count]=key;
						sdCyberChainChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdCyberChainRepCount=count;
				sdCyberChainRange=chancesum;
		}



		if(SDMConfig->SetSection("SpiderChainReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdSpiderChainReps[count]=key;
						sdSpiderChainChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdSpiderChainRepCount=count;
				sdSpiderChainRange=chancesum;
		}



		if(SDMConfig->SetSection("ExtraHealthReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdExtraHealthReps[count]=key;
						sdExtraHealthRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdExtraHealthRepCount=count;
				sdExtraHealthRange=chancesum;
		}




		if(SDMConfig->SetSection("ArtiReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdArtiReps[count]=key;
						sdArtiRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdArtiRepCount=count;
				sdArtiRange=chancesum;
		}






		if(SDMConfig->SetSection("ExplosiveBarrelReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdExplosiveBarrelReps[count]=key;
						sdExplosiveBarrelRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdExplosiveBarrelRepCount=count;
				sdExplosiveBarrelRange=chancesum;
		}





		if(SDMConfig->SetSection("BurningBarrelReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdBurningBarrelReps[count]=key;
						sdBurningBarrelRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdBurningBarrelRepCount=count;
				sdBurningBarrelRange=chancesum;
		}



		if(SDMConfig->SetSection("ArchvileReplacements")){
			const char *key;
			const char *value;
			int count=0;
			int chancesum=0;
			int tmpVal = 0;
			bool skip=false;
			//SetValueForKey("GHKCustClass","2");
			while (SDMConfig->NextInSection (key, value)){
				sdConfigValues+=key;
				sdConfigValues+=value;
				skip=false;
				//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

				//if not atoi do a check for n:1, n:2
				//, then substr it and do atoi on that
				//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

				char tmpbuf[1000];
				char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
				char *pch = strtok(tmpbuf,":");
				value=pch;

				pch=strtok(NULL,":");

				if(pch!=NULL){

						//if(pch!=NULL){

							if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
								tmpVal = atoi(pch);
						//}

					if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
						if(tmpVal!=2)
							skip=true;
						else
							skip=false;
					}else{ //doom1
						if(tmpVal!=1)
							skip=true;
						else
							skip=false;
					}

				}

				if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
					chancesum+=atoi(value);
					sdArchvileReps[count]=key;
					sdArchvileRepChances[count]=chancesum;

					//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

					//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

					count++;
				}

			}
			sdArchvileRepCount=count;
			sdArchvileRange=chancesum;


			/*
			const char **p;

			int i=0;
			for (p = sdArchvileReps; p < &sdArchvileReps[countof(sdArchvileReps)]; p++){

				if(i>=count)
					break;

				i++;
				if (debugfile) fprintf (debugfile, "custClass:%s",*p);

				//if(*p==NULL)//No more entries.
					//break;

			}

			for(int i=0;i < countof(sdArchvileRepChances); i++){

				if(i>=count)
					break;

				if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchvileRepChances[i]);

				//if(!sdArchiveRepChances[i])//No more entries.
					//break;
			}
			*/
		}


		if(SDMConfig->SetSection("ArachnotronReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdArachnotronReps[count]=key;
						sdArachnotronRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdArachnotronRepCount=count;
				sdArachnotronRange=chancesum;
		}



		if(SDMConfig->SetSection("BaronOfHellReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdBaronOfHellReps[count]=key;
						sdBaronOfHellRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdBaronOfHellRepCount=count;
				sdBaronOfHellRange=chancesum;
		}


		if(SDMConfig->SetSection("HellKnightReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdHellKnightReps[count]=key;
						sdHellKnightRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
				sdHellKnightRepCount=count;
				sdHellKnightRange=chancesum;
		}



		if(SDMConfig->SetSection("CacodemonReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdCacodemonReps[count]=key;
						sdCacodemonRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdCacodemonRepCount=count;
			sdCacodemonRange=chancesum;
		}



		if(SDMConfig->SetSection("CyberdemonReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;

					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdCyberdemonReps[count]=key;
						sdCyberdemonRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdCyberdemonRepCount=count;
			sdCyberdemonRange=chancesum;
		}


		if(SDMConfig->SetSection("DemonReplacements")){
					const char *key;
					const char *value;
					int count=0;
					int chancesum=0;
					int tmpVal = 0;
					bool skip=false;
					//SetValueForKey("GHKCustClass","2");
					while (SDMConfig->NextInSection (key, value)){
						sdConfigValues+=key;
						sdConfigValues+=value;
						skip=false;
						//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

						//if not atoi do a check for n:1, n:2
						//, then substr it and do atoi on that
						//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

						char tmpbuf[1000];
						char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
						char *pch = strtok(tmpbuf,":");
						value=pch;

						pch=strtok(NULL,":");

						if(pch!=NULL){

								//if(pch!=NULL){

									if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
										tmpVal = atoi(pch);
								//}

							if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
								if(tmpVal!=2)
									skip=true;
								else
									skip=false;
							}else{ //doom1
								if(tmpVal!=1)
									skip=true;
								else
									skip=false;
							}

						}

						if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
							chancesum+=atoi(value);
							sdDemonReps[count]=key;
							sdDemonRepChances[count]=chancesum;

							//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

							//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

							count++;
						}

					}
				sdDemonRepCount=count;
				sdDemonRange=chancesum;
			}



		if(SDMConfig->SetSection("SpectreReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;

					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdSpectreReps[count]=key;
						sdSpectreRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdSpectreRepCount=count;
			sdSpectreRange=chancesum;
		}


		if(SDMConfig->SetSection("ChaingunGuyReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;

					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdChaingunGuyReps[count]=key;
						sdChaingunGuyRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdChaingunGuyRepCount=count;
			sdChaingunGuyRange=chancesum;
		}


		if(SDMConfig->SetSection("DoomImpReplacements")){
			const char *key;
			const char *value;
			int count=0;
			int chancesum=0;
			int tmpVal = 0;
			bool skip=false;
			//SetValueForKey("GHKCustClass","2");
			while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;

				skip=false;
				//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

				//if not atoi do a check for n:1, n:2
				//, then substr it and do atoi on that
				//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

				char tmpbuf[1000];
				char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
				char *pch = strtok(tmpbuf,":");
				value=pch;

				pch=strtok(NULL,":");

				if(pch!=NULL){

						//if(pch!=NULL){

							if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
								tmpVal = atoi(pch);
						//}

					if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
						if(tmpVal!=2)
							skip=true;
						else
							skip=false;
					}else{ //doom1
						if(tmpVal!=1)
							skip=true;
						else
							skip=false;
					}

				}

				if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
					chancesum+=atoi(value);
					sdDoomImpReps[count]=key;
					sdDoomImpRepChances[count]=chancesum;

					//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

					//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

					count++;
				}

			}
			sdDoomImpRepCount=count;
			sdDoomImpRange=chancesum;
		}


		if(SDMConfig->SetSection("FatsoReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdFatsoReps[count]=key;
						sdFatsoRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdFatsoRepCount=count;
			sdFatsoRange=chancesum;
		}


		if(SDMConfig->SetSection("LostSoulReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdLostSoulReps[count]=key;
						sdLostSoulRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdLostSoulRepCount=count;
			sdLostSoulRange=chancesum;
		}



		if(SDMConfig->SetSection("PainElementalReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdPainElementalReps[count]=key;
						sdPainElementalRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdPainElementalRepCount=count;
			sdPainElementalRange=chancesum;
		}


		if(SDMConfig->SetSection("RevenantReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdRevenantReps[count]=key;
						sdRevenantRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdRevenantRepCount=count;
			sdRevenantRange=chancesum;
		}


		if(SDMConfig->SetSection("ShotgunGuyReplacements")){
			const char *key;
			const char *value;
			int count=0;
			int chancesum=0;
			int tmpVal = 0;
			bool skip=false;
			//SetValueForKey("GHKCustClass","2");
			while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
				skip=false;
				//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

				//if not atoi do a check for n:1, n:2
				//, then substr it and do atoi on that
				//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

				char tmpbuf[1000];
				char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
				char *pch = strtok(tmpbuf,":");
				value=pch;

				pch=strtok(NULL,":");

				if(pch!=NULL){

						//if(pch!=NULL){

							if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
								tmpVal = atoi(pch);
						//}

					if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
						if(tmpVal!=2)
							skip=true;
						else
							skip=false;
					}else{ //doom1
						if(tmpVal!=1)
							skip=true;
						else
							skip=false;
					}

				}

				if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
					chancesum+=atoi(value);
					sdShotgunGuyReps[count]=key;
					sdShotgunGuyRepChances[count]=chancesum;

					//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

					//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

					count++;
				}

			}
			sdShotgunGuyRepCount=count;
			sdShotgunGuyRange=chancesum;
		}



		if(SDMConfig->SetSection("SpiderMastermindReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdSpiderMastermindReps[count]=key;
						sdSpiderMastermindRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdSpiderMastermindRepCount=count;
			sdSpiderMastermindRange=chancesum;
		}



		if(SDMConfig->SetSection("WolfensteinSSReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdWolfensteinSSReps[count]=key;
						sdWolfensteinSSRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdWolfensteinSSRepCount=count;
			sdWolfensteinSSRange=chancesum;
		}


		if(SDMConfig->SetSection("ZombieManReplacements")){
				const char *key;
				const char *value;
				int count=0;
				int chancesum=0;
				int tmpVal = 0;
				bool skip=false;
				//SetValueForKey("GHKCustClass","2");
				while (SDMConfig->NextInSection (key, value)){
					sdConfigValues+=key;
					sdConfigValues+=value;
					skip=false;
					//SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

					//if not atoi do a check for n:1, n:2
					//, then substr it and do atoi on that
					//if(atoi(value)){ //<--atoi returning 4 for 4:1 etc...

					char tmpbuf[1000];
					char * tmp = strcpy(tmpbuf,value); //<-- needs to be of type str[xxxx]
					char *pch = strtok(tmpbuf,":");
					value=pch;

					pch=strtok(NULL,":");

					if(pch!=NULL){

							//if(pch!=NULL){

								if(atoi(pch)) //ghk: get next integer after ":", should be 1 or 2
									tmpVal = atoi(pch);
							//}

						if(gameinfo.flags & GI_MAPxx){ //check for doom2 or doom 1, this line is doom2
							if(tmpVal!=2)
								skip=true;
							else
								skip=false;
						}else{ //doom1
							if(tmpVal!=1)
								skip=true;
							else
								skip=false;
						}

					}

					if(PClass::FindClass(key)&&atoi(value)>0&&!skip){
						chancesum+=atoi(value);
						sdZombieManReps[count]=key;
						sdZombieManRepChances[count]=chancesum;

						//if (debugfile) fprintf (debugfile, "custClass:%s",sdArchiveReps[count]);

						//if (debugfile) fprintf (debugfile, "chancesum:%d",sdArchiveRepChances[count]);

						count++;
					}

				}
			sdZombieManRepCount=count;
			sdZombieManRange=chancesum;
		}



		//if(*(sd_configvalues_md5)==0)
			//sd_configvalues_md5="0";

		md5wrapper md5;
		//std::string ghktemp(md5.getHashFromString(sdConfigValues.c_str()));
		sdConfigValues=md5.getHashFromString(sdConfigValues.c_str());
		sd_configvalues_md5=sdConfigValues.c_str();
		//sd_configvalues_md5=ghktemp.c_str();
		//confighash=ghktemp.c_str();

		//return confighash;

}


//GHK
 //Important, for stopping re-writing of user edits on engine-start
void FGameConfigFile::SetSDUserReplacements ()
{
	const char *key;
	const char *value;

	if(SetSection("ArchvileReplacements",true)){

		while (NextInSection (key, value)){

			SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}
	if(SetSection("ArachnotronReplacements",true)){
		while (NextInSection (key, value)){

			SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}

	if(SetSection("BaronOfHellReplacements",true)){
			while (NextInSection (key, value)){

				SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}

	if(SetSection("HellKnightReplacements",true)){
		while (NextInSection (key, value)){

			SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}

	if(SetSection("CacodemonReplacements",true)){
		while (NextInSection (key, value)){

				SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}
	if(SetSection("CyberdemonReplacements",true)){
		while (NextInSection (key, value)){

				SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}

	if(SetSection("DemonReplacements",true)){
		while (NextInSection (key, value)){

			SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}


	if(SetSection("SpectreReplacements",true)){
		while (NextInSection (key, value)){

			SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}

	if(SetSection("ChaingunGuyReplacements",true)){
		while (NextInSection (key, value)){

			SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}

	if(SetSection("DoomImpReplacements",true)){
		while (NextInSection (key, value)){

			SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}

	if(SetSection("FatsoReplacements",true)){
		while (NextInSection (key, value)){

			SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}

	if(SetSection("LostSoulReplacements",true)){
		while (NextInSection (key, value)){

			SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}

	if(SetSection("PainElementalReplacements",true)){
		while (NextInSection (key, value)){

			SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}

	if(SetSection("RevenantReplacements",true)){
		while (NextInSection (key, value)){

			SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}

	if(SetSection("ShotgunGuyReplacements",true)){
		while (NextInSection (key, value)){

		SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}

	if(SetSection("SpiderMastermindReplacements",true)){
		while (NextInSection (key, value)){

			SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}

	if(SetSection("WolfensteinSSReplacements",true)){
		while (NextInSection (key, value)){

			SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}

	if(SetSection("ZombieManReplacements",true)){
		while (NextInSection (key, value)){

			SetValueForKey(key, value); //Important, for stopping re-writing of user edits on engine-start

		}
	}

}


//GHK For now set the hi scores in the config file
//Possibly might need to create a new config file for hi scores
//since the permutation of wad configurations can be very large.
//and possibly impede performance, but all configfile I/O should
//not be during gameplay!
bool FGameConfigFile::SetHiScore(const char *myScore, char *levelname,bool isWadHiScore){
	const char *arg;
	//char *buffer=(char *)HeapAlloc (GetProcessHeap(), 0, 16384);
	char buffer[16384];
	int i,p;
	p=0;
	//char *hiScore;



	//identifier is the wad list delimited by pipes
	//i.e. |doom2.wad|icarus.wad|icar28a.wad
	//full doom2 entry is |doom2.wad|icarus.wad|icar28a.wad=12783203
	//full doom 1 episode entry is: |doom.wad|2002.wad|e1=12783203
    //doom 1 map episode entry is: |doom.wad|2002.wad|e1m1=12783203
	//doom 2 map entry is: |doom2.wad|icarus.wad|icar28a.wad|mapname=12783203


	//GHK: i=2, not i=0, so dont get gzscoredoom.pk3 nor gzscoredoom.pk3:BBAX16.WAD,  Still need IWAD though
	//since doom2.wad, tnt.wad, plutonia.wad dont have episodes.
	//might need to strip out .wad and .pk3 just in case, saves space too.

	for (i = 2; (arg = Wads.GetWadName (i)) != NULL; ++i)
	{
		//ignore sdgldefs.pk3
		if(!strcmp(arg, "sdgldefs.pk3"))
			continue;

		//if(!stricmp(arg, "SD-ADDONPACK.WAD")){
			//arg="A";
		//}

		/*

		if(!strcmp(arg, "SDMONSTERS1.9.WAD")){
			arg="M19";
		}else if(!strcmp(arg, "SD-ARTIFACTS.WAD")){
			arg="A";
		//if(strstr(arg, "sdmonsters")!=NULL)
			//arg="M14"
		}else if(!strcmp(arg, "BBA.WAD")||!strcmp(arg, "BBA.RFF")||!strcmp(arg, "BBA-16X.RFF")||!strcmp(arg, "BBA-16X.WAD")){
			arg="BBA";
			}
		if(strcmp(arg, "BBA"))//DON'T CARE ABOUT BBA wad.
			p += sprintf (buffer+p, "|%s",arg);

		*/
		p += sprintf (buffer+p, "|%s",arg);

	}

	//get the level name
	if(!isWadHiScore){
		p += sprintf (buffer+p, "|%s",levelname);

	}else{
		//check for doom 1 episodes for full wad saves, since doom is per episode.
		if((tolower(levelname[0])=='e' && levelname[1]>='1' && levelname[1]<='4' && tolower(levelname[2])=='m')){
			char epName[3];
			//strncpy(epName,levelname,2);

			epName[0]=tolower(levelname[0]);
			epName[1]=tolower(levelname[1]);
			epName[2]='\0';
			//p+=sprintf (buffer+p, "|%s",strncpy(epName,levelname,1));
			p+=sprintf (buffer+p, "|%s",epName);

		}
	}
	//lets allow for separate offline non-infighting & infighting scores.
	p+=sprintf (buffer+p, "|%d",sd_notarget);

	//lets allow for separate pistol start  scores.
		p+=sprintf (buffer+p, "|%d",sd_piststart);


	if(SetSection("HiScores",true)){
		const char *hiScore = GetValueForKey (buffer);
		if(hiScore==NULL){
			//hi score isnt set yet for the wad configuration
			SetValueForKey(buffer,myScore);
			return true;


		}else{
			//convert to ints and compare and save if need be.
			if(atoi(myScore)>atoi(hiScore)){
				SetValueForKey(buffer,myScore);
				return true;
			}


		}

	}

	return false; //hi score not set

}

//GHK
bool FGameConfigFile::M_RemoteSaveSDPWDOK (const char * playersname, const char * oldpwd, const char * newpwd){

	//char buffer[16384];
	int p,found;
	p=0;

	//char lbuf [1];
	if (debugfile) fprintf (debugfile, "GHKSetRemotePlayerPWD:\n");

	std::string playername(playersname);

	std::string playerpwd(newpwd);

	found=playername.find(':');

	if(found!=std::string::npos){
		Printf("Player name must not contain ':' characters when querying the hi score server\n");
		//Printf("**ONLINE HI SCORING DISABLED**\nReverting to Local Hi Scores\n");
		//sd_remote_hiscores_ok=false;
		return false;
	}

	found=playerpwd.find(':');
	if(found!=std::string::npos){
		Printf("Player password must not contain ':' characters when querying the hi score server\n");
		//Printf("**ONLINE HI SCORING DISABLED**\nReverting to Local Hi Scores\n");
		//sd_remote_hiscores_ok=false;
		return false;
	}

	Request	myRequest;

	std::string	sHeaderSend, sHeaderReceive, sMessage;

	sHeaderSend="playername=";
	sHeaderSend.append(playersname);
	sHeaderSend+="&oldpwd=";
	sHeaderSend.append(oldpwd);
	sHeaderSend+="&newpwd=";
	sHeaderSend.append(newpwd);

	std::string sURL(sd_hiscoreserver_url);
    std::string sURLPage("/playersd.php");
	sURL+=sURLPage;
	//first arg is postdata flag. sheadersend is for post data.
	myRequest.SendRequest(1, (LPCTSTR)sURL.c_str(), sHeaderSend, sHeaderReceive, sMessage);

	if (debugfile) fprintf (debugfile, "GHKSetRemotePlayerPWD url:%s",sURL.c_str());
	if (debugfile) fprintf (debugfile, "GHKSetRemotePlayerPWD Header Send:%s",sHeaderSend.c_str());


	if (debugfile) fprintf (debugfile, "GHKSetRemotePlayerPWD Header Received:%s",sHeaderReceive.c_str());
	if (debugfile) fprintf (debugfile, "GHKSetRemotePlayerPWD Message Received:%s", sMessage.c_str());


	if(!sMessage.c_str()||sMessage.find("returnval")==std::string::npos){
		Printf("Connection error to the hi score server\n");
		//Printf("**ONLINE HI SCORING DISABLED**\nReverting to Local Hi Scores\n");
		//sd_remote_hiscores_ok=false;
		if (debugfile) fprintf (debugfile, "GHKSetRemotePlayerPWD Header Received:%s",sHeaderReceive.c_str());
		if (debugfile) fprintf (debugfile, "GHKSetRemotePlayerPWD Message Received:%s", sMessage.c_str());

		return false;

	}else{
		//Parse the return string of the form:
		//"returnval:'errorflag':'retmessage':'wad player name':'wadhiscore':'level player name':'levelhiscore':
		//if errorflag = 1, will be of the form: returnval:'errorflag':'retmessage':

		if(sMessage.find("returnval")!=std::string::npos){

			int getstartpos=sMessage.find("returnval");
			int found=getstartpos;
			int foundnext=getstartpos;

			//Get Error Flag
			if(sMessage[getstartpos + 10]=='0'){ //use compare() for this instead?

				Printf("Player password set on the hi score server\n");

				return true;
			}else{

				//got error, lets explain to player.
				foundnext=sMessage.find(":",12);
				if(foundnext!=std::string::npos)
					Printf("%s\n",sMessage.substr(12,foundnext-12).c_str());
					//Printf("**ONLINE HI SCORING DISABLED**\nReverting to Local Hi Scores\n");
					//sd_remote_hiscores_ok=false;

				}
				return false;
		}else{
			Printf("Error connecting to the hi score server\n");
			//Printf("**ONLINE HI SCORING DISABLED**\nReverting to Local Hi Scores\n");
			//sd_remote_hiscores_ok=false;
			if (debugfile) fprintf (debugfile, "GHKSetRemotePlayerPWD Header Received:%s",sHeaderReceive.c_str());
			if (debugfile) fprintf (debugfile, "GHKSetRemotePlayerPWD Message Received:%s", sMessage.c_str());
			return false;
		}

	}

	return false;
}




//GHK added
const char * FGameConfigFile::SetRemoteHiScores(char *lvlhiscoreset, char *wadhiscoreset, char *levelname,const char * lvlscore, const char * wadscore, bool blIsFinale){
const char *arg;
	//char *buffer=(char *)HeapAlloc (GetProcessHeap(), 0, 16384);
	char buffer[16384];
	int i,p,found;
	p=0;

	char lbuf [1];
	//char *hiScore;
	if (debugfile) fprintf (debugfile, "GHKSetRemoteScore:\n");

	std::string playername(players[0].userinfo.netname);

	std::string playerpwd(sd_hiscoreserver_pwd);

	found=playername.find(':');

	if(found!=std::string::npos){
		Printf("Player name must not contain ':' characters when querying the hi score server\n");
		Printf("**ONLINE HI SCORING DISABLED**\nReverting to Local Hi Scores\n");
		sd_remote_hiscores_ok=false;
		return "";
	}

	found=playerpwd.find(':');
	if(found!=std::string::npos){
		Printf("Player password must not contain ':' characters when querying the hi score server\n");
		Printf("**ONLINE HI SCORING DISABLED**\nReverting to Local Hi Scores\n");
		sd_remote_hiscores_ok=false;
		return "";
	}

	Request	myRequest;

	std::string	sHeaderSend, sHeaderReceive, sMessage;

	sHeaderSend="playername=";
	sHeaderSend.append(players[0].userinfo.netname);
	sHeaderSend+="&pwd=";
	sHeaderSend.append(sd_hiscoreserver_pwd);
	sHeaderSend+="&globalhash=";
	sHeaderSend.append(sd_global_md5);
	sHeaderSend+="&patchinfo=";
	sHeaderSend.append(sd_patches2);
	sHeaderSend+="&wadhash=";
	sHeaderSend.append(sd_wads_md5);
	sHeaderSend+="&addonpackhash=";
	sHeaderSend.append(sd_addonpack_md5);
	sHeaderSend+="&sdgldefshash=";
	sHeaderSend.append(sd_gldefs_md5);
	sHeaderSend+="&configvalueshash=";
	sHeaderSend.append(sd_configvalues_md5);
	sHeaderSend+="&wadhiscore=";
	sHeaderSend.append(wadscore);
	sHeaderSend+="&levelhiscore=";
	sHeaderSend.append(lvlscore);
	sHeaderSend+="&levelname=";
	sHeaderSend.append(levelname);
	sHeaderSend+="&mode=";
	sprintf(lbuf,"%d",sd_online_level);
	sHeaderSend.append(lbuf);
	sHeaderSend+="&noinfighting=";
	sprintf(lbuf,"%d",sd_notarget==1?1:0);
	sHeaderSend.append(lbuf);
	sHeaderSend+="&pistolstart=";
	sprintf(lbuf,"%d",sd_piststart==1?1:0);
	sHeaderSend.append(lbuf);
	sHeaderSend+="&isfinale=";
	sprintf(lbuf,"%d",blIsFinale==true?1:0);
	sHeaderSend.append(lbuf);
	sHeaderSend+="&wadepiname=";


	//GHK: i=2, not i=0, so dont get gzscoredoom.pk3 nor gzscoredoom.pk3:BBAX16.WAD
	for (i = 2; (arg = Wads.GetWadName (i)) != NULL; ++i)
	{
		//buffer += wsprintf (buffer, "|%s",arg);

		/*if(!strcmp(arg, "SDMONSTERS1.9.WAD")){
			arg="M19";
		}else if(!strcmp(arg, "SD-ARTIFACTS.WAD")){
			arg="A";
		//if(strstr(arg, "sdmonsters")!=NULL)
			//arg="M14"
		}else if(!strcmp(arg, "BBA.WAD")||!strcmp(arg, "BBA.RFF")||!strcmp(arg, "BBA-16X.RFF")||!strcmp(arg, "BBA-16X.WAD")){
			arg="BBA";
			}
		if(strcmp(arg, "BBA"))//DON'T CARE ABOUT BBA wad.
		*/
			p+=sprintf (buffer+p, "|%s",arg);
	}


	//check for doom 1 episodes for full wad saves, since doom is per episode.
		if((tolower(levelname[0])=='e' && levelname[1]>='1' && levelname[1]<='4' && tolower(levelname[2])=='m')){
			//buffer +=wsprintf (buffer, "|%s%s",tolower(levelname[0]),tolower(levelname[1]));
			char epName[3];
			//strncpy(epName,levelname,2);

  			epName[0]=tolower(levelname[0]);
			epName[1]=tolower(levelname[1]);
			epName[2]='\0';
			//p+=sprintf (buffer+p, "|%s",strncpy(epName,levelname,1));
			p+=sprintf (buffer+p, "|%s",epName);
			//sHeaderSend.append(epName);
			//buffer +=wsprintf (buffer, "|%s%s",levelname[0],levelname[1]);

		}
		sHeaderSend.append(buffer);

	//Printf("sHeaderSend: %s",sHeaderSend.c_str());

		//if (debugfile) fprintf (debugfile, "sHeaderSend: %s\n",sHeaderSend.c_str());

	//myRequest.SendRequest(IsPost, (LPCTSTR)m_sURL, sHeaderSend, sHeaderReceive, sMessage);
	//std::string sURL("http://localhost:81/sdhiscoretest.jsp");
	std::string sURL(sd_hiscoreserver_url); //sURL("http://bilbohicks.007gb.com/hiscores.php"); //sURL("http://localhost/sdcoretest.php");
    std::string sURLPage("/hiscores.php");
	sURL+=sURLPage;
	//first arg is postdata flag. sheadersend is for post data.
	myRequest.SendRequest(1, (LPCTSTR)sURL.c_str(), sHeaderSend, sHeaderReceive, sMessage);

	if (debugfile) fprintf (debugfile, "sHeaderSend2: ", sHeaderSend.c_str());
	if (debugfile) fprintf (debugfile, "sMessage: ", sMessage.c_str());

	if(!sMessage.c_str()||sMessage.find("returnval")==std::string::npos){
		Printf("Connection error to the hi score server\n");
		Printf("**ONLINE HI SCORING DISABLED**\nReverting to Local Hi Scores\n");
		sd_remote_hiscores_ok=false;
		if (debugfile) fprintf (debugfile, "SetRemoteHiScores Header Received:%s",sHeaderReceive.c_str());
		if (debugfile) fprintf (debugfile, "SetRemoteHiScores Message Received:%s", sMessage.c_str());


	}else{
		//Parse the return string of the form:
		//"returnval:'errorflag':'retmessage':'wad player name':'wadhiscore':'level player name':'levelhiscore':
		//if errorflag = 1, will be of the form: returnval:'errorflag':'retmessage':

		if(sMessage.find("returnval")!=std::string::npos){

			int getstartpos=sMessage.find("returnval");
			int found=getstartpos;
			int foundnext=getstartpos;

			//Get Error Flag
			if(sMessage[getstartpos + 10]=='0'){ //use compare() for this instead?
				//no error


				//get the retmessage
				foundnext=sMessage.find(':',getstartpos + 12);
				found=foundnext;
				if(foundnext!=std::string::npos){
					//Printf("%s\n",sMessage.substr(12,foundnext-12).c_str());
					if (debugfile) fprintf (debugfile, "retmsg: %s\n",sMessage.substr(12,foundnext-12).c_str());

				}

				//get the levelhiscoreset flag
				foundnext=sMessage.find(":",found+1);

				if(foundnext!=std::string::npos){
					//Printf("%s\n",sMessage.substr(found+1,foundnext-found-1).c_str());
					if (debugfile) fprintf (debugfile, "levelhiscoreset: %s\n",sMessage.substr(found+1,foundnext-found-1).c_str());

					strcpy(lvlhiscoreset,sMessage.substr(found+1,foundnext-found-1).c_str());
					//sMessage.substr(found+1,foundnext-found-2).c_str();


				}
				found=foundnext;
				//get the wadhiscoreset flag
				foundnext=sMessage.find(":",found+1);

				if(foundnext!=std::string::npos){

					//Printf("%s\n",sMessage.substr(found+1,foundnext-found-1).c_str());

					strcpy(wadhiscoreset,sMessage.substr(found+1,foundnext-found-1).c_str());
					//wadHiScore = sMessage.substr(found+1,foundnext-found-1).c_str();

					if (debugfile) fprintf (debugfile, "wadhiscoreset: %s\n",sMessage.substr(found+1,foundnext-found-1).c_str());


				}

				//THE FOLLOWING CODE SHOULD NEVER BE REACHED... GHK
				found=foundnext;

				//get level hi score player name
				foundnext=sMessage.find(":",found+1);

				if(foundnext!=std::string::npos){

					//Printf("%s\n",sMessage.substr(found+1,foundnext-found-1).c_str());

					if (debugfile) fprintf (debugfile, "playerlvlname: %s\n",sMessage.substr(found+1,foundnext-found-1).c_str());


				}
				found=foundnext;

				//get level hi score
				foundnext=sMessage.find(":",found+1);

				if(foundnext!=std::string::npos){
					//Printf("%s\n",sMessage.substr(found+1,foundnext-found-1).c_str());

					if (debugfile) fprintf (debugfile, "playerwadname: %s\n",sMessage.substr(found+1,foundnext-found-1).c_str());


					//lvlHiScore = sMessage.substr(found+1,foundnext-found-1).c_str();

				}
				//found=foundnext;


			}else{

				//got error, lets explain to player.
				foundnext=sMessage.find(":",12);
				if(foundnext!=std::string::npos)
					Printf("%s\n",sMessage.substr(12,foundnext-12).c_str());
					Printf("**ONLINE HI SCORING DISABLED**\nReverting to Local Hi Scores\n");
					sd_remote_hiscores_ok=false;

			}

		}else{
			Printf("Error connecting to the hi score server\n");
			Printf("**ONLINE HI SCORING DISABLED**\nReverting to Local Hi Scores\n");
			sd_remote_hiscores_ok=false;
			if (debugfile) fprintf (debugfile, "GetRemoteHiScores Header Received:%s",sHeaderReceive.c_str());
			if (debugfile) fprintf (debugfile, "GetRemoteHiScores Message Received:%s", sMessage.c_str());

		}

	}

	//if (debugfile) fprintf (debugfile, "GHKRESPONSE:%s",sMessage.c_str());


 return "";
}

//GHK added
const char * FGameConfigFile::GetRemoteHiScores(char *lvlHiScore, char *wadHiScore, char *levelname){
const char *arg;
	//char *buffer=(char *)HeapAlloc (GetProcessHeap(), 0, 16384);
	char buffer[16384];
	int i,p,found;
	p=0;
	char lbuf [1];

	//char *hiScore;
	if (debugfile) fprintf (debugfile, "GHKGetRemoteScore:\n");

	std::string playername(players[0].userinfo.netname);

	std::string playerpwd(sd_hiscoreserver_pwd);

	found=playername.find(':');

	if(found!=std::string::npos){
		Printf("Player name must not contain ':' characters when querying the hi score server\n");
		Printf("**ONLINE HI SCORING DISABLED**\nReverting to Local Hi Scores\n");
		sd_remote_hiscores_ok=false;
		return "";
	}

	found=playerpwd.find(':');
	if(found!=std::string::npos){
		Printf("Player password must not contain ':' characters when querying the hi score server\n");
		Printf("**ONLINE HI SCORING DISABLED**\nReverting to Local Hi Scores\n");
		sd_remote_hiscores_ok=false;
		return "";
	}

	//if (debugfile) fprintf (debugfile, "ghklevelSIZE: %d ",strlen(levelname));

	Request	myRequest;

	std::string	sHeaderSend, sHeaderReceive, sMessage;

	sHeaderSend="playername=";
	sHeaderSend.append(players[0].userinfo.netname);
	sHeaderSend+="&pwd=";
	sHeaderSend.append(sd_hiscoreserver_pwd);
	sHeaderSend+="&globalhash=";
	sHeaderSend.append(sd_global_md5);
	sHeaderSend+="&patchinfo=";
	sHeaderSend.append(sd_patches2);
	sHeaderSend+="&wadhash=";
	sHeaderSend.append(sd_wads_md5);
	sHeaderSend+="&addonpackhash=";
	sHeaderSend.append(sd_addonpack_md5);
	sHeaderSend+="&sdgldefshash=";
	sHeaderSend.append(sd_gldefs_md5);
	sHeaderSend+="&configvalueshash=";
	sHeaderSend.append(sd_configvalues_md5);
	sHeaderSend+="&levelname=";
	sHeaderSend.append(levelname);
	sHeaderSend+="&mode=";
	sprintf(lbuf,"%d",sd_online_level);
	sHeaderSend.append(lbuf);
	sHeaderSend+="&noinfighting=";
	sprintf(lbuf,"%d",sd_notarget==1?1:0);
	sHeaderSend.append(lbuf);
	sHeaderSend+="&pistolstart=";
	sprintf(lbuf,"%d",sd_piststart==1?1:0);
	sHeaderSend.append(lbuf);
	sHeaderSend+="&wadepiname=";
	//GHK: i=2, not i=0, so dont get gzscoredoom.pk3 nor gzscoredoom.pk3:BBAX16.WAD

	//GHK: for getting generating wadhash
	if (debugfile) fprintf (debugfile, "ghklevelname: %s ghkwadhash: %s",levelname,sd_wads_md5);


	for (i = 2; (arg = Wads.GetWadName (i)) != NULL; ++i)
	{
		//buffer += wsprintf (buffer, "|%s",arg);

		/*if(!strcmp(arg, "SDMONSTERS1.9.WAD")){
			arg="M19";
		}else if(!strcmp(arg, "SD-ARTIFACTS.WAD")){
			arg="A";
		//if(strstr(arg, "sdmonsters")!=NULL)
			//arg="M14"
		}else if(!strcmp(arg, "BBA.WAD")||!strcmp(arg, "BBA.RFF")||!strcmp(arg, "BBA-16X.RFF")||!strcmp(arg, "BBA-16X.WAD")){
			arg="BBA";
			}
		if(strcmp(arg, "BBA"))//DON'T CARE ABOUT BBA wad.
		*/
			p+=sprintf (buffer+p, "|%s",arg);
	}


	//check for doom 1 episodes for full wad saves, since doom is per episode.
		if((tolower(levelname[0])=='e' && levelname[1]>='1' && levelname[1]<='4' && tolower(levelname[2])=='m')){
			//buffer +=wsprintf (buffer, "|%s%s",tolower(levelname[0]),tolower(levelname[1]));
			char epName[3];
			//strncpy(epName,levelname,2);

  			epName[0]=tolower(levelname[0]);
			epName[1]=tolower(levelname[1]);
			epName[2]='\0';
			//p+=sprintf (buffer+p, "|%s",strncpy(epName,levelname,1));
			p+=sprintf (buffer+p, "|%s",epName);
			//sHeaderSend.append(epName);
			//buffer +=wsprintf (buffer, "|%s%s",levelname[0],levelname[1]);

		}
		sHeaderSend.append(buffer);

	//Printf("sHeaderSend: %s",sHeaderSend.c_str());

		//if (debugfile) fprintf (debugfile, "GHKHeadersend:%s",sHeaderSend.c_str());

	//myRequest.SendRequest(IsPost, (LPCTSTR)m_sURL, sHeaderSend, sHeaderReceive, sMessage);
	//std::string sURL("http://localhost:81/sdhiscoretest.jsp");
	std::string sURL(sd_hiscoreserver_url); //sURL("http://bilbohicks.007gb.com/hiscores.php"); //sURL("http://localhost/sdcoretest.php");
    std::string sURLPage("/hiscores.php");
	sURL+=sURLPage;
	//first arg is postdata flag. sheadersend is for post data.
	//if (debugfile) fprintf (debugfile, "sHeaderSend: %s", sHeaderSend.c_str());

	if (debugfile) fprintf (debugfile, "sHeaderSend2: ", sHeaderSend.c_str());
	if (debugfile) fprintf (debugfile, "sMessage: ", sMessage.c_str());


	myRequest.SendRequest(1, (LPCTSTR)sURL.c_str(), sHeaderSend, sHeaderReceive, sMessage);

	if(!sMessage.c_str()||sMessage.find("returnval")==std::string::npos){
		Printf("Connection error to the hi score server\n");
		Printf("**ONLINE HI SCORING DISABLED**\nReverting to Local Hi Scores\n");
		sd_remote_hiscores_ok=false;
		if (debugfile) fprintf (debugfile, "GetRemoteHiScores Header Received:%s",sHeaderReceive.c_str());
		if (debugfile) fprintf (debugfile, "GetRemoteHiScores Message Received:%s", sMessage.c_str());


	}else{
		//Parse the return string of the form:
		//"returnval:'errorflag':'retmessage':'wad player name':'wadhiscore':'level player name':'levelhiscore':
		//if errorflag = 1, will be of the form: returnval:'errorflag':'retmessage':



		if(sMessage.find("returnval")!=std::string::npos){

			int getstartpos=sMessage.find("returnval");
			int found=getstartpos;
			int foundnext=getstartpos;

			//Get Error Flag
			if(sMessage[getstartpos+10]=='0'){ //use compare() for this instead?
				//no error


				//get the retmessage
				foundnext=sMessage.find(':',getstartpos+12);
				found=foundnext;
				if(foundnext!=std::string::npos){
					//Printf("%s\n",sMessage.substr(12,foundnext-12).c_str());
					if (debugfile) fprintf (debugfile, "retmsg: %s\n",sMessage.substr(12,foundnext-12).c_str());

				}

				//get wad hi score player name
				foundnext=sMessage.find(":",found+1);

				if(foundnext!=std::string::npos){
					//wad_hiscore_name=sMessage.substr(found+1,foundnext-found-1).c_str();
					strcpy(wad_hiscore_name,sMessage.substr(found+1,foundnext-found-1).c_str());

					//Printf("%s\n",wad_hiscore_name);
					if (debugfile) fprintf (debugfile, "playerwadname: %s\n",wad_hiscore_name);




				}
				found=foundnext;
				//get wad hi score
				foundnext=sMessage.find(":",found+1);

				if(foundnext!=std::string::npos){

					//Printf("%s\n",sMessage.substr(found+1,foundnext-found-1).c_str());

					strcpy(wadHiScore,sMessage.substr(found+1,foundnext-found-1).c_str());
					//wadHiScore = sMessage.substr(found+1,foundnext-found-1).c_str();

					if (debugfile) fprintf (debugfile, "wadhiscore: %s\n",sMessage.substr(found+1,foundnext-found-1).c_str());


				}
				found=foundnext;

				//get level hi score player name
				foundnext=sMessage.find(":",found+1);

				if(foundnext!=std::string::npos){
					//lvl_hiscore_name=sMessage.substr(found+1,foundnext-found-1).c_str();

					strcpy(lvl_hiscore_name,sMessage.substr(found+1,foundnext-found-1).c_str());

					//Printf("%s\n",lvl_hiscore_name);

					if (debugfile) fprintf (debugfile, "playerlvlname: %s\n",lvl_hiscore_name);


				}
				found=foundnext;

				//get level hi score
				foundnext=sMessage.find(":",found+1);

				if(foundnext!=std::string::npos){
					//Printf("%s\n",sMessage.substr(found+1,foundnext-found-1).c_str());

					if (debugfile) fprintf (debugfile, "playerwadname: %s\n",sMessage.substr(found+1,foundnext-found-1).c_str());

					strcpy(lvlHiScore,sMessage.substr(found+1,foundnext-found-1).c_str());
					//lvlHiScore = sMessage.substr(found+1,foundnext-found-1).c_str();

				}
				//found=foundnext;


			}else{

				//got error, lets explain to player.
				foundnext=sMessage.find(":",getstartpos+12);

				if(foundnext!=std::string::npos)
					Printf("%s\n",sMessage.substr(getstartpos+12,foundnext-12-getstartpos).c_str());
					Printf("**ONLINE HI SCORING DISABLED**\nReverting to Local Hi Scores\n");
					sd_remote_hiscores_ok=false;

			}

		}else{
			Printf("Error connecting to the hi score server\n");
			Printf("**ONLINE HI SCORING DISABLED**\nReverting to Local Hi Scores\n");
			sd_remote_hiscores_ok=false;
			if (debugfile) fprintf (debugfile, "GetRemoteHiScores Header Received:%s",sHeaderReceive.c_str());
			if (debugfile) fprintf (debugfile, "GetRemoteHiScores Message Received:%s", sMessage.c_str());

		}

	}

	if (debugfile) fprintf (debugfile, "GHKRESPONSE:%s",sMessage.c_str());


 return "";
}



//GHK added
const char * FGameConfigFile::GetHiScore(const char *hiScore, char *levelname,bool isWadHiScore){
const char *arg;
	//char *buffer=(char *)HeapAlloc (GetProcessHeap(), 0, 16384);
	char buffer[16384];
	int i,p;
	p=0;
	//char *hiScore;





	//identifier is the wad list delimited by pipes
	//i.e. |doom2.wad|icarus.wad|icar28a.wad
	//full doom2 entry is |doom2.wad|icarus.wad|icar28a.wad=12783203
	//full doom 1 episode entry is: |doom.wad|2002.wad|e1=12783203
    //doom 1 map episode entry is: |doom.wad|2002.wad|e1m1=12783203
	//doom 2 map entry is: |doom2.wad|icarus.wad|icar28a.wad|mapname=12783203

	//ghk i=2, not i=0, so only get pwads now, since thats all we really need
	//might need to strip out .wad and .pk3 just in case, saves space too.
	//GHK: i=2, not i=0, so dont get gzscoredoom.pk3 nor gzscoredoom.pk3:BBAX16.WAD
	for (i = 2; (arg = Wads.GetWadName (i)) != NULL; ++i)
	{

		//ignore sdgldefs.pk3
		if(!strcmp(arg, "sdgldefs.pk3"))
			continue;

		//if(!strcmp(arg, "SD-ADDONPACK.WAD")){
			//arg="A";
		//}

		//buffer += wsprintf (buffer, "|%s",arg);

		/*
		if(!strcmp(arg, "SDMONSTERS1.9.WAD")){
			arg="M19";
		}else if(!strcmp(arg, "SD-ARTIFACTS.WAD")){
			arg="A";
		//if(strstr(arg, "sdmonsters")!=NULL)
			//arg="M14"
		}else if(!strcmp(arg, "BBA.WAD")||!strcmp(arg, "BBA.RFF")||!strcmp(arg, "BBA-16X.RFF")||!strcmp(arg, "BBA-16X.WAD")){
			arg="BBA";
			}
		if(strcmp(arg, "BBA"))//DON'T CARE ABOUT BBA wad.
			p+=sprintf (buffer+p, "|%s",arg);
	*/
		p += sprintf (buffer+p, "|%s",arg);
	}


	//get the level name
	if(!isWadHiScore){
		p+= sprintf (buffer+p, "|%s",levelname);

	}else{

		//check for doom 1 episodes for full wad saves, since doom is per episode.
		if((tolower(levelname[0])=='e' && levelname[1]>='1' && levelname[1]<='4' && tolower(levelname[2])=='m')){
			//buffer +=wsprintf (buffer, "|%s%s",tolower(levelname[0]),tolower(levelname[1]));
			char epName[3];
			//strncpy(epName,levelname,2);

  			epName[0]=tolower(levelname[0]);
			epName[1]=tolower(levelname[1]);
			epName[2]='\0';
			//p+=sprintf (buffer+p, "|%s",strncpy(epName,levelname,1));
			p+=sprintf (buffer+p, "|%s",epName);
			//buffer +=wsprintf (buffer, "|%s%s",levelname[0],levelname[1]);

		}
	}
	//lets allow for separate offline non-infighting & infighting scores.
	p+=sprintf (buffer+p, "|%d",sd_notarget);

	//lets allow for separate pistol start  scores.
	p+=sprintf (buffer+p, "|%d",sd_piststart);

	if(SetSection("HiScores",true)){
		hiScore = GetValueForKey (buffer);
		if(hiScore==NULL){
			//hi score isnt set yet for the wad configuration
			hiScore ="0";
		}
	}
 return hiScore;
}


//GHK read sdcustom.ini for any user maps set to only use default doom monsters
//either due to performance with ad-on pack, MAP30 SOD, or just plain hardness.
//dropping down to default monsters will almost always hurt points, so dont
//penalize if onn high score server.
const int FGameConfigFile::GetSDUserMapDefaultMonsters(FGameConfigFile * SDMConfig, char *levelname){
const char *arg;
	//char *buffer=(char *)HeapAlloc (GetProcessHeap(), 0, 16384);
	char buffer[16384];
	int i,p;
	p=0;
	int defaultmonstermap=0;
	//char *hiScore;





	//identifier is the wad list delimited by pipes
	//i.e. |doom2.wad|icarus.wad|icar28a.wad
	//full doom2 entry is |doom2.wad|icarus.wad|icar28a.wad=12783203
	//full doom 1 episode entry is: |doom.wad|2002.wad|e1=12783203
    //doom 1 map episode entry is: |doom.wad|2002.wad|e1m1=12783203
	//doom 2 map entry is: |doom2.wad|icarus.wad|icar28a.wad|mapname=12783203

	//ghk i=2, not i=0, so only get pwads now, since thats all we really need
	//might need to strip out .wad and .pk3 just in case, saves space too.
	//GHK: i=2, not i=0, so dont get gzscoredoom.pk3 nor gzscoredoom.pk3:BBAX16.WAD
	for (i = 2; (arg = Wads.GetWadName (i)) != NULL; ++i)
	{

		//ignore sdgldefs.pk3
		if(!strcmp(arg, "sdgldefs.pk3"))
			continue;

		//if(!strcmp(arg, "SD-ADDONPACK.WAD")){
			//arg="A";
		//}

		//buffer += wsprintf (buffer, "|%s",arg);

		/*
		if(!strcmp(arg, "SDMONSTERS1.9.WAD")){
			arg="M19";
		}else if(!strcmp(arg, "SD-ARTIFACTS.WAD")){
			arg="A";
		//if(strstr(arg, "sdmonsters")!=NULL)
			//arg="M14"
		}else if(!strcmp(arg, "BBA.WAD")||!strcmp(arg, "BBA.RFF")||!strcmp(arg, "BBA-16X.RFF")||!strcmp(arg, "BBA-16X.WAD")){
			arg="BBA";
			}
		if(strcmp(arg, "BBA"))//DON'T CARE ABOUT BBA wad.
			p+=sprintf (buffer+p, "|%s",arg);
	*/
		p += sprintf (buffer+p, "|%s",arg);
	}


	//get the level name
	if(true){ //(!isWadHiScore){
		p+= sprintf (buffer+p, "|%s",levelname);

	}else{

		//check for doom 1 episodes for full wad saves, since doom is per episode.
		if((tolower(levelname[0])=='e' && levelname[1]>='1' && levelname[1]<='4' && tolower(levelname[2])=='m')){
			//buffer +=wsprintf (buffer, "|%s%s",tolower(levelname[0]),tolower(levelname[1]));
			char epName[3];
			//strncpy(epName,levelname,2);

  			epName[0]=tolower(levelname[0]);
			epName[1]=tolower(levelname[1]);
			epName[2]='\0';
			//p+=sprintf (buffer+p, "|%s",strncpy(epName,levelname,1));
			p+=sprintf (buffer+p, "|%s",epName);
			//buffer +=wsprintf (buffer, "|%s%s",levelname[0],levelname[1]);

		}
	}
	//lets allow for separate offline non-infighting & infighting scores.
	//p+=sprintf (buffer+p, "|%d",sd_notarget);

	//convert to upper case
	for ( int ix = 0; buffer[ix] != '\0'; ix++)
	{
     buffer[ix] = toupper( (unsigned char) buffer[ix] );
	}
	if (debugfile) fprintf (debugfile, "DefaultMonsterMapsVal:%s\n",buffer);
	if(SDMConfig->SetSection("DefaultMonsterMaps")){
		const char *val = SDMConfig->GetValueForKey (buffer);
		if(val==NULL){
			//hi score isnt set yet for the wad configuration
			defaultmonstermap=0;

		}else{
			defaultmonstermap=atoi(val);



		}
	}

 return defaultmonstermap;
}

//GHK: rnd Seed for Custom monsters.
const int FGameConfigFile::GetWadsLevelSeed(char *levelname){
const char *arg;
	//char *buffer=(char *)HeapAlloc (GetProcessHeap(), 0, 16384);
	char buffer[16384];
	int i,p,wadslevelseed;
	p=0;
	//char *hiScore;


	//identifier is the wad list delimited by pipes
	//i.e. |doom2.wad|icarus.wad|icar28a.wad
	//full doom2 entry is |doom2.wad|icarus.wad|icar28a.wad=12783203
	//full doom 1 episode entry is: |doom.wad|2002.wad|e1=12783203
    //doom 1 map episode entry is: |doom.wad|2002.wad|e1m1=12783203
	//doom 2 map entry is: |doom2.wad|icarus.wad|icar28a.wad|mapname=12783203

	//ghk i=2, not i=0, so only get pwads now, since thats all we really need
	//might need to strip out .wad and .pk3 just in case, saves space too.
	//md5wrapper md5;

	for (i = 2; (arg = Wads.GetWadName (i)) != NULL; ++i)
	{
		//if (debugfile) fprintf (debugfile, "wadnames:%s\n",arg);
		//std::string hash2 = md5.getHashFromFile(Wads.GetWadFullName(i));

		//if (debugfile) fprintf (debugfile, "wadname:%s, wadhash:%s",arg,hash2.c_str());
		//buffer += wsprintf (buffer, "|%s",arg);

		if(!strcmp(arg, "SD-ADDONPACK.WAD")){
			arg="A";
		}

		//ignore sdgldefs.pk3
		if(!strcmp(arg, "sdgldefs.pk3"))
			continue;


		/*
		if(!strcmp(arg, "SDMONSTERS1.9.WAD")){
			arg="M19";
			continue;
		}else if(!strcmp(arg, "SD-ARTIFACTS.WAD")){
			arg="A";
			continue;
		//if(strstr(arg, "sdmonsters")!=NULL)
			//arg="M14"
		}else if(!strcmp(arg, "BBA.WAD")||!strcmp(arg, "BBA.RFF")||!strcmp(arg, "BBA-16X.RFF")||!strcmp(arg, "BBA-16X.WAD")){
			arg="BBA";
			continue;
		}else{

		//if(strcmp(arg, "BBA"))//DON'T CARE ABOUT BBA wad.
			//if (debugfile) fprintf (debugfile, "customwad:%s",arg);
			p+=sprintf (buffer+p, "|%s",arg);
		}

		*/
	}
	//if (debugfile) fprintf (debugfile, "wadname:%s, wadhash:",hash2.c_str());


	char bufferq[17];
	int q = 0;
    int m = 1;
	wadslevelseed=0;
	q+= sprintf (bufferq+q, "%s",sd_wads_md5);

	//if (debugfile) fprintf (debugfile, "sd_wads_md5:%s\n",sd_wads_md5);
	for(int k=0; k<=q; k++){
		m+=tolower(bufferq[k])*k;
	}
	//if (debugfile) fprintf (debugfile, "wadslevelseedm1:%d\n",m);

	//get the level name
		p+= sprintf (buffer+p, "|%s",levelname);
		//if (debugfile) fprintf (debugfile, "levelnameA%s\n",levelname);
	//wadslevelseed=0;
	//add the total ascii char values to generate the seed value for the wads + level num
	//for(int k=0; k<sizeof(buffer); k++)
		//wadslevelseed+=p; //added tolower just to be consistent.
	//if (debugfile) fprintf (debugfile, "buffersize:%d",sizeof(buffer));
	//if (debugfile) fprintf (debugfile, "psize:%s",buffer);
	for(int k=0; k<=p; k++){
		wadslevelseed+=tolower(buffer[k])*k; //added tolower just to be consistent.
		//if (debugfile) fprintf (debugfile, "asciival:%d\n",tolower(buffer[k]));

	}


	//if (debugfile) fprintf (debugfile, "wadslevelseed2:%d\n",wadslevelseed);

	wadslevelseed*=m;
	//if (debugfile) fprintf (debugfile, "wadslevelseedM:%d\n",wadslevelseed);

	//if (debugfile) fprintf (debugfile, "wadslevelseedFFF:%d\n",wadslevelseed&0xfff);

	return wadslevelseed;
}


void FGameConfigFile::DoGlobalSetup ()
{
	if (SetSection ("GlobalSettings.Unknown"))
	{
		ReadCVars (CVAR_GLOBALCONFIG);
	}
	if (SetSection ("GlobalSettings"))
	{
		ReadCVars (CVAR_GLOBALCONFIG);
	}
	if (SetSection ("LastRun"))
	{
		const char *lastver = GetValueForKey ("Version");
		if (lastver != NULL)
		{
			double last = atof (lastver);
			if (last < 123.1)
			{
				FBaseCVar *noblitter = FindCVar ("vid_noblitter", NULL);
				if (noblitter != NULL)
				{
					noblitter->ResetToDefault ();
				}
			}
			if (last < 201)
			{
				// Be sure the Hexen fourth weapons are assigned to slot 4
				// If this section does not already exist, then they will be
				// assigned by SetupWeaponList().
				if (SetSection ("Hexen.WeaponSlots"))
				{
					SetValueForKey ("Slot[4]", "FWeapQuietus CWeapWraithverge MWeapBloodscourge");
				}
			}
			if (last < 202)
			{
				// Make sure the Hexen hotkeys are accessible by default.
				if (SetSection ("Hexen.Bindings"))
				{
					SetValueForKey ("\\", "use ArtiHealth");
					SetValueForKey ("scroll", "+showscores");
					SetValueForKey ("0", "useflechette");
					SetValueForKey ("9", "use ArtiBlastRadius");
					SetValueForKey ("8", "use ArtiTeleport");
					SetValueForKey ("7", "use ArtiTeleportOther");
					SetValueForKey ("6", "use ArtiEgg");
					SetValueForKey ("5", "use ArtiInvulnerability");
				}
			}
			if (last < 204)
			{ // The old default for vsync was true, but with an unlimited framerate
			  // now, false is a better default.
				FBaseCVar *vsync = FindCVar ("vid_vsync", NULL);
				if (vsync != NULL)
				{
					vsync->ResetToDefault ();
				}
			}
		}
	}
}

void FGameConfigFile::DoGameSetup (const char *gamename)
{
	const char *key;
	const char *value;
	enum { Doom, Heretic, Hexen, Strife } game;

	if (strcmp (gamename, "Heretic") == 0)
		game = Heretic;
	else if (strcmp (gamename, "Hexen") == 0)
		game = Hexen;
	else if (strcmp (gamename, "Strife") == 0)
		game = Strife;
	else
		game = Doom;

	if (bMigrating)
	{
		MigrateOldConfig ();
	}
	subsection = section + sprintf (section, "%s.", gamename);

	strcpy (subsection, "UnknownConsoleVariables");
	if (SetSection (section))
	{
		ReadCVars (0);
	}

	strcpy (subsection, "ConsoleVariables");
	if (SetSection (section))
	{
		ReadCVars (0);
	}

	if (game != Doom && game != Strife)
	{
		SetRavenDefaults (game == Hexen);
	}

	// The NetServerInfo section will be read when it's determined that
	// a netgame is being played.
	strcpy (subsection, "LocalServerInfo");
	if (SetSection (section))
	{
		ReadCVars (0);
	}

	strcpy (subsection, "Player");
	if (SetSection (section))
	{
		ReadCVars (0);
	}

	strcpy (subsection, "Bindings");
	if (!SetSection (section))
	{ // Config has no bindings for the given game
		if (!bMigrating)
		{
			C_SetDefaultBindings ();
		}
	}
	else
	{
		C_UnbindAll ();
		while (NextInSection (key, value))
		{
			C_DoBind (key, value, false);
		}
	}

	strcpy (subsection, "DoubleBindings");
	if (SetSection (section))
	{
		while (NextInSection (key, value))
		{
			C_DoBind (key, value, true);
		}
	}

	strcpy (subsection, "ConsoleAliases");
	if (SetSection (section))
	{
		const char *name = NULL;
		while (NextInSection (key, value))
		{
			if (stricmp (key, "Name") == 0)
			{
				name = value;
			}
			else if (stricmp (key, "Command") == 0 && name != NULL)
			{
				C_SetAlias (name, value);
				name = NULL;
			}
		}
	}
}

// Separated from DoGameSetup because it needs all the weapons properly defined
void FGameConfigFile::DoWeaponSetup (const char *gamename)
{
	strcpy (subsection, "WeaponSlots");

	if (!SetSection (section) || !LocalWeapons.RestoreSlots (*this))
	{
		SetupWeaponList (gamename);
	}
}

void FGameConfigFile::ReadNetVars ()
{
	strcpy (subsection, "NetServerInfo");
	if (SetSection (section))
	{
		ReadCVars (0);
	}
}

void FGameConfigFile::ReadCVars (DWORD flags)
{
	const char *key, *value;
	FBaseCVar *cvar;
	UCVarValue val;

	while (NextInSection (key, value))
	{
		cvar = FindCVar (key, NULL);
		if (cvar == NULL)
		{
			cvar = new FStringCVar (key, NULL,
				CVAR_AUTO|CVAR_UNSETTABLE|CVAR_ARCHIVE|flags);
		}
		val.String = const_cast<char *>(value);
		cvar->SetGenericRep (val, CVAR_String);
	}
}

void FGameConfigFile::ArchiveGameData (const char *gamename)
{
	char section[32*3], *subsection;

	subsection = section + sprintf (section, "%s.", gamename);

	strcpy (subsection, "Player");
	SetSection (section, true);
	ClearCurrentSection ();
	C_ArchiveCVars (this, 4);

	strcpy (subsection, "ConsoleVariables");
	SetSection (section, true);
	ClearCurrentSection ();
	C_ArchiveCVars (this, 0);

	strcpy (subsection, netgame ? "NetServerInfo" : "LocalServerInfo");
	if (!netgame || consoleplayer == 0)
	{ // Do not overwrite this section if playing a netgame, and
	  // this machine was not the initial host.
		SetSection (section, true);
		ClearCurrentSection ();
		C_ArchiveCVars (this, 5);
	}

	strcpy (subsection, "UnknownConsoleVariables");
	SetSection (section, true);
	ClearCurrentSection ();
	C_ArchiveCVars (this, 2);

	strcpy (subsection, "ConsoleAliases");
	SetSection (section, true);
	ClearCurrentSection ();
	C_ArchiveAliases (this);

	M_SaveCustomKeys (this, section, subsection);

	strcpy (subsection, "Bindings");
	SetSection (section, true);
	ClearCurrentSection ();
	C_ArchiveBindings (this, false);

	strcpy (subsection, "DoubleBindings");
	SetSection (section, true);
	ClearCurrentSection ();
	C_ArchiveBindings (this, true);

	if (WeaponSection.IsEmpty())
	{
		strcpy (subsection, "WeaponSlots");
	}
	else
	{
		sprintf (subsection, "%s.WeaponSlots", WeaponSection.GetChars());
	}
	SetSection (section, true);
	ClearCurrentSection ();
	LocalWeapons.SaveSlots (*this);
}

void FGameConfigFile::ArchiveGlobalData ()
{
	SetSection ("LastRun", true);
	ClearCurrentSection ();
	SetValueForKey ("Version", LASTRUNVERSION);

	SetSection ("GlobalSettings", true);
	ClearCurrentSection ();
	C_ArchiveCVars (this, 1);

	SetSection ("GlobalSettings.Unknown", true);
	ClearCurrentSection ();
	C_ArchiveCVars (this, 3);
}

FString FGameConfigFile::GetConfigPath (bool tryProg)
{
	char *pathval;
	FString path;

	pathval = Args.CheckValue ("-config");
	if (pathval != NULL)
		return FString(pathval);

#ifndef unix
	path = NULL;
	HRESULT hr;

	TCHAR uname[UNLEN+1];
	DWORD unamelen = countof(uname);

	// Because people complained, try for a user-specific .ini in the program directory first.
	// If that is not writeable, use the one in the home directory instead.
	hr = GetUserName (uname, &unamelen);
	if (SUCCEEDED(hr) && uname[0] != 0)
	{
		// Is it valid for a user name to have slashes?
		// Check for them and substitute just in case.
		char *probe = uname;
		while (*probe != 0)
		{
			if (*probe == '\\' || *probe == '/')
				*probe = '_';
			++probe;
		}

		path = progdir;
		path += "zdoom-";
		path += uname;
		path += ".ini";
		if (tryProg)
		{
			if (!FileExists (path.GetChars()))
			{
				path = "";
			}
		}
		else
		{ // check if writeable
			FILE *checker = fopen (path.GetChars(), "a");
			if (checker == NULL)
			{
				path = "";
			}
			else
			{
				fclose (checker);
			}
		}
	}

	if (path.IsEmpty())
	{
		if (Args.CheckParm ("-cdrom"))
			return "c:\\zdoomdat\\zdoom.ini";

		path = progdir;
		path += "zdoom.ini";
	}
	return path;
#else
	return GetUserFile ("zdoom.ini");
#endif
}
//ghk
FString FGameConfigFile::GetConfigPathSDM (bool tryProg)
{
	char *pathval;
	FString path;

	pathval = Args.CheckValue ("-sdmconfig");
	if (pathval != NULL)
		return FString(pathval);

#ifndef unix
	path = NULL;
	/*HRESULT hr;

	TCHAR uname[UNLEN+1];
	DWORD unamelen = countof(uname);

	// Because people complained, try for a user-specific .ini in the program directory first.
	// If that is not writeable, use the one in the home directory instead.
	hr = GetUserName (uname, &unamelen);
	if (SUCCEEDED(hr) && uname[0] != 0)
	{
		// Is it valid for a user name to have slashes?
		// Check for them and substitute just in case.
		char *probe = uname;
		while (*probe != 0)
		{
			if (*probe == '\\' || *probe == '/')
				*probe = '_';
			++probe;
		}

		path = progdir;
		path += "zdoom-";
		path += uname;
		path += ".ini";
		if (tryProg)
		{
			if (!FileExists (path.GetChars()))
			{
				path = "";
			}
		}
		else
		{ // check if writeable
			FILE *checker = fopen (path.GetChars(), "a");
			if (checker == NULL)
			{
				path = "";
			}
			else
			{
				fclose (checker);
			}
		}
	}
	*/

	if (path.IsEmpty())
	{
		//if (Args.CheckParm ("-cdrom"))
			//return "c:\\zdoomdat\\zdoom.ini";

		//check for drag n drop ini files:
		path = FString(sd_global_sdmini_path);
		if(!path.IsEmpty()){
			path = FString(sd_global_sdmini_path);
		}else{
			path = progdir;
			path += "sdcustom.ini";
		}

	}
	return path;
#else
	return GetUserFile ("zdoom.ini");
#endif
}


void FGameConfigFile::AddAutoexec (DArgs *list, const char *game)
{
	char section[64];
	const char *key;
	const char *value;

	sprintf (section, "%s.AutoExec", game);

	if (bMigrating)
	{
		FBaseCVar *autoexec = FindCVar ("autoexec", NULL);

		if (autoexec != NULL)
		{
			UCVarValue val;
			char *path;

			val = autoexec->GetGenericRep (CVAR_String);
			path = copystring (val.String);
			delete autoexec;
			SetSection (section, true);
			SetValueForKey ("Path", path);
			list->AppendArg (path);
			delete[] path;
		}
	}
	else
	{
		// If <game>.AutoExec section does not exist, create it
		// with a default autoexec.cfg file present.
		if (!SetSection (section))
		{
			FString path;

#ifndef unix
			if (Args.CheckParm ("-cdrom"))
			{
				path = "c:\\zdoomdat\\autoexec.cfg";
			}
			else
			{
				path = progdir;
				path += "autoexec.cfg";
			}
#else
			path = GetUserFile ("autoexec.cfg");
#endif
			SetSection (section, true);
			SetValueForKey ("Path", path.GetChars());
		}
		// Run any files listed in the <game>.AutoExec section
		if (SetSection (section))
		{
			while (NextInSection (key, value))
			{
				if (stricmp (key, "Path") == 0 && FileExists (value))
				{
					list->AppendArg (value);
				}
			}
		}
	}
}

void FGameConfigFile::SetRavenDefaults (bool isHexen)
{
	UCVarValue val;

	if (bMigrating)
	{
		con_centernotify.ResetToDefault ();
		msg0color.ResetToDefault ();
		dimcolor.ResetToDefault ();
		color.ResetToDefault ();
	}

	val.Bool = true;
	con_centernotify.SetGenericRepDefault (val, CVAR_Bool);
	snd_pitched.SetGenericRepDefault (val, CVAR_Bool);
	val.Int = 9;
	msg0color.SetGenericRepDefault (val, CVAR_Int);
	val.Int = 0x0000ff;
	dimcolor.SetGenericRepDefault (val, CVAR_Int);
	val.Int = CR_WHITE;
	msgmidcolor.SetGenericRepDefault (val, CVAR_Int);
	val.Int = CR_YELLOW;
	msgmidcolor2.SetGenericRepDefault (val, CVAR_Int);

	val.Int = 0x543b17;
	am_wallcolor.SetGenericRepDefault (val, CVAR_Int);
	val.Int = 0xd0b085;
	am_fdwallcolor.SetGenericRepDefault (val, CVAR_Int);
	val.Int = 0x734323;
	am_cdwallcolor.SetGenericRepDefault (val, CVAR_Int);

	// Fix the Heretic/Hexen automap colors so they are correct.
	// (They were wrong on older versions.)
	if (*am_wallcolor == 0x2c1808 && *am_fdwallcolor == 0x887058 && *am_cdwallcolor == 0x4c3820)
	{
		am_wallcolor.ResetToDefault ();
		am_fdwallcolor.ResetToDefault ();
		am_cdwallcolor.ResetToDefault ();
	}

	if (!isHexen)
	{
		val.Int = 0x3f6040;
		color.SetGenericRepDefault (val, CVAR_Int);
	}
}

void FGameConfigFile::SetupWeaponList (const char *gamename)
{
	for (int i = 0; i < NUM_WEAPON_SLOTS; ++i)
	{
		LocalWeapons.Slots[i].Clear ();
	}

	if (strcmp (gamename, "Heretic") == 0)
	{
		LocalWeapons.Slots[1].AddWeapon ("Staff");
		LocalWeapons.Slots[1].AddWeapon ("Gauntlets");
		LocalWeapons.Slots[2].AddWeapon ("GoldWand");
		LocalWeapons.Slots[3].AddWeapon ("Crossbow");
		LocalWeapons.Slots[4].AddWeapon ("Blaster");
		LocalWeapons.Slots[5].AddWeapon ("SkullRod");
		LocalWeapons.Slots[6].AddWeapon ("PhoenixRod");
		LocalWeapons.Slots[7].AddWeapon ("Mace");
	}
	else if (strcmp (gamename, "Hexen") == 0)
	{
		LocalWeapons.Slots[1].AddWeapon ("FWeapFist");
		LocalWeapons.Slots[2].AddWeapon ("FWeapAxe");
		LocalWeapons.Slots[3].AddWeapon ("FWeapHammer");
		LocalWeapons.Slots[4].AddWeapon ("FWeapQuietus");
		LocalWeapons.Slots[1].AddWeapon ("CWeapMace");
		LocalWeapons.Slots[2].AddWeapon ("CWeapStaff");
		LocalWeapons.Slots[3].AddWeapon ("CWeapFlame");
		LocalWeapons.Slots[4].AddWeapon ("CWeapWraithverge");
		LocalWeapons.Slots[1].AddWeapon ("MWeapWand");
		LocalWeapons.Slots[2].AddWeapon ("MWeapFrost");
		LocalWeapons.Slots[3].AddWeapon ("MWeapLightning");
		LocalWeapons.Slots[4].AddWeapon ("MWeapBloodscourge");
	}
	else if (strcmp (gamename, "Strife") == 0)
	{
		LocalWeapons.Slots[1].AddWeapon ("PunchDagger");
		LocalWeapons.Slots[2].AddWeapon ("StrifeCrossbow2");
		LocalWeapons.Slots[2].AddWeapon ("StrifeCrossbow");
		LocalWeapons.Slots[3].AddWeapon ("AssaultGun");
		LocalWeapons.Slots[4].AddWeapon ("MiniMissileLauncher");
		LocalWeapons.Slots[5].AddWeapon ("StrifeGrenadeLauncher2");
		LocalWeapons.Slots[5].AddWeapon ("StrifeGrenadeLauncher");
		LocalWeapons.Slots[6].AddWeapon ("FlameThrower");
		LocalWeapons.Slots[7].AddWeapon ("Mauler2");
		LocalWeapons.Slots[7].AddWeapon ("Mauler");
		LocalWeapons.Slots[8].AddWeapon ("Sigil");
	}
	else // Doom
	{
		LocalWeapons.Slots[1].AddWeapon ("Fist");
		LocalWeapons.Slots[1].AddWeapon ("Chainsaw");
		LocalWeapons.Slots[2].AddWeapon ("Pistol");
		LocalWeapons.Slots[3].AddWeapon ("Shotgun");
		LocalWeapons.Slots[3].AddWeapon ("SuperShotgun");
		LocalWeapons.Slots[4].AddWeapon ("Chaingun");
		LocalWeapons.Slots[5].AddWeapon ("RocketLauncher");
		LocalWeapons.Slots[6].AddWeapon ("PlasmaRifle");
		LocalWeapons.Slots[7].AddWeapon ("BFG9000");
	}
}

CCMD (whereisini)
{
	FString path = GameConfig->GetConfigPath (false);
	Printf ("%s\n", path.GetChars());
}

/*
** gameconfigfile.h
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

#ifndef __GAMECONFIGFILE_H__
#define __GAMECONFIGFILE_H__


#include "configfile.h"

class DArgs;

class FGameConfigFile : public FConfigFile
{
public:
	FGameConfigFile ();
	FGameConfigFile (char *sdmname);
	~FGameConfigFile ();

	void DoGlobalSetup ();
	bool SetHiScore(const char *myScore, char *level,bool isWadHiScore); //GHK added
	const char * GetHiScore(const char *hiScore, char *level,bool isWadHiScore); //GHK
	const char * GetRemoteHiScores(char *lvlHiScore, char *wadHiScore, char *levelname); //GHK
	const char * SetRemoteHiScores(char *lvlhiscoreset, char *wadhiscoreset, char *levelname,const char * lvlscore, const char * wadscore, bool blIsFinale=false); //GHK
	const int FGameConfigFile::GetWadsLevelSeed(char *levelname); //GHK
	void FGameConfigFile::GetSDUserReplacements (); //GHK
	void FGameConfigFile::SetSDUserReplacements (); //GHK
	void FGameConfigFile::GetSDUserReplacementsSDM (FGameConfigFile * SDMConfig); //GHK
	const int FGameConfigFile::GetSDUserMapDefaultMonsters (FGameConfigFile * SDMConfig, char *level); //GHK
	static bool FGameConfigFile::M_RemoteSaveSDPWDOK (const char * playername, const char * oldpwd, const char * newpwd); //GHK

	void DoGameSetup (const char *gamename);
	void DoWeaponSetup (const char *gamename);
	void ArchiveGlobalData ();
	void ArchiveGameData (const char *gamename);
	void AddAutoexec (DArgs *list, const char *gamename);
	FString GetConfigPath (bool tryProg);
	void ReadNetVars ();
	FString GetConfigPathSDM (bool tryProg); //ghk

protected:
	void WriteCommentHeader (FILE *file) const;

private:
	static void MigrateStub (const char *pathname, FConfigFile *config, void *userdata);

	void MigrateOldConfig ();
	void SetRavenDefaults (bool isHexen);
	void ReadCVars (DWORD flags);
	void SetupWeaponList (const char *gamename);

	bool bMigrating;

	char section[64];
	char *subsection;
};

extern FString WeaponSection;
extern FGameConfigFile *GameConfig;
extern FGameConfigFile *GameConfigSDM; //ghk

#endif //__GAMECONFIGFILE_H__

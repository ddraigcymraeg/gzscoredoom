/*
** Enhanced heads up 'overlay' for fullscreen
**
**---------------------------------------------------------------------------
** Copyright 2003-2005 Christoph Oelckers
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

// NOTE: Some stuff in here might seem a little redundant but I wanted this
// to be as true as possible to my original intent which means that it
// only uses that code from ZDoom's status bar that is the same as any
// copy would be.
#include "doomstat.h" //ghk
#include "doomtype.h"
#include "v_video.h"
#include "gi.h"
#include "c_cvars.h"
#include "c_dispatch.h"
#include "c_console.h"
#include "w_wad.h"
#include "a_keys.h"
#include "sbar.h"
#include "sc_man.h"
#include "templates.h"
#include "S_Sound.h" //ghk
#include "announcer.h" //ghk

#define HUMETA_AltIcon 0x10f000

EXTERN_CVAR(Bool,am_follow)
EXTERN_CVAR (Int, con_scaletext)
EXTERN_CVAR (Bool, idmypos)

EXTERN_CVAR (Bool, am_showtime)
EXTERN_CVAR (Bool, am_showtotaltime)

EXTERN_CVAR (Int, sd_global_hiscores) //ghk
EXTERN_CVAR (Int, sd_noinfight) //ghk
EXTERN_CVAR (Int, sd_pistolstart) //ghk

CVAR(Bool,hud_althudscale, true, CVAR_ARCHIVE)			// Scale the hud to 640x400?
CVAR(Bool,hud_althud, true, CVAR_ARCHIVE)				// Enable/Disable the alternate HUD
CVAR(Bool,hud_spreecenter, false, CVAR_ARCHIVE)				// ghk enable/disable center line chain count/spree count stats

														// These are intentionally not the same as in the automap!
CVAR (Bool,  hud_showsecrets,	true,CVAR_ARCHIVE);		// Show secrets on HUD
CVAR (Bool,  hud_showmonsters,	true,CVAR_ARCHIVE);		// Show monster stats on HUD
CVAR (Bool,  hud_showitems,		true,CVAR_ARCHIVE);	// Show item stats on HUD
CVAR (Bool,  hud_showstats,		true,	CVAR_ARCHIVE);	// for stamina and accuracy.
CVAR (Bool,  hud_showbarrels,	true,	CVAR_ARCHIVE);	// for stamina and accuracy. //ghk

CVAR (Int, hud_ammo_red, 25, CVAR_ARCHIVE)					// ammo percent less than which status is red
CVAR (Int, hud_ammo_yellow, 50, CVAR_ARCHIVE)				// ammo percent less is yellow more green
CVAR (Int, hud_health_red, 25, CVAR_ARCHIVE)				// health amount less than which status is red
CVAR (Int, hud_health_yellow, 50, CVAR_ARCHIVE)				// health amount less than which status is yellow
CVAR (Int, hud_health_green, 100, CVAR_ARCHIVE)				// health amount above is blue, below is green
CVAR (Int, hud_armor_red, 25, CVAR_ARCHIVE)					// armor amount less than which status is red
CVAR (Int, hud_armor_yellow, 50, CVAR_ARCHIVE)				// armor amount less than which status is yellow
CVAR (Int, hud_armor_green, 100, CVAR_ARCHIVE)				// armor amount above is blue, below is green

CVAR (Int, hudcolor_titl, CR_YELLOW, CVAR_ARCHIVE)			// color of automap title
CVAR (Int, hudcolor_time, CR_RED, CVAR_ARCHIVE)				// color of level/hub time
CVAR (Int, hudcolor_ltim, CR_ORANGE, CVAR_ARCHIVE)			// color of single level time
CVAR (Int, hudcolor_ttim, CR_GOLD, CVAR_ARCHIVE)			// color of total time
CVAR (Int, hudcolor_xyco, CR_GREEN, CVAR_ARCHIVE)			// color of coordinates

CVAR (Int, hudcolor_statnames, CR_RED, CVAR_ARCHIVE)		// For the letters befóre the stats
CVAR (Int, hudcolor_stats, CR_GREEN, CVAR_ARCHIVE)			// For the stats values themselves


CVAR(Bool, map_point_coordinates, true, CVAR_ARCHIVE|CVAR_GLOBALCONFIG)	// show player or map coordinates?

static FFont * HudFont;						// The font for the health and armor display
static FFont * IndexFont;					// The font for the inventory indices

// Icons
static FTexture * healthpic;				// Health icon
static FTexture * fragpic;					// Frags icon
static FTexture * invgems[4];				// Inventory arrows

static int hudwidth, hudheight;				// current width/height for HUD display
static bool showlog=false;

//GHK for beat hi score messages
//wadHiScorebeatenHUD should be made global, to stop it being triggerd each load.
bool wadHiScorebeatenHUD = true; //only show once. Though may show more than once due to par penalties
bool lvlHiScorebeatenHUD = true; //only show once. Though may show more than once due to par penalties
bool ptOverShowOnceHUD = false; //only show once.

bool lvlHiScorebeatenHUDReal=false; //flag for messages, for when the hi score is really beaten
bool wadHiScorebeatenHUDReal=false; //flag for messages, for when the hi score is really beaten

void AM_GetPosition(fixed_t & x, fixed_t & y);

//---------------------------------------------------------------------------
//
// Draws an image into a box with its bottom center at the bottom
// center of the box. The image is scaled down if it doesn't fit
//
//---------------------------------------------------------------------------
static void DrawImageToBox(FTexture * tex, int x, int y, int w, int h, int trans=0xc000)
{
	float scale1, scale2;

	if (tex)
	{
		int texwidth=tex->GetWidth();
		int texheight=tex->GetHeight();

		if (w<texwidth) scale1=(float)w/texwidth;
		else scale1=1.0f;
		if (h<texheight) scale2=(float)h/texheight;
		else scale2=1.0f;
		if (scale2<scale1) scale1=scale2;

		x+=w>>1;
		y+=h;

		w=(int)(texwidth*scale1);
		h=(int)(texheight*scale1);

		screen->DrawTexture(tex, x, y,
			DTA_KeepRatio, true,
			DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, trans,
			DTA_DestWidth, w, DTA_DestHeight, h, DTA_CenterBottomOffset, 1, TAG_DONE);

	}
}


//---------------------------------------------------------------------------
//
// Draws a text but uses a fixed width for all characters
//
//---------------------------------------------------------------------------

static void DrawHudText(int color, char * text, int x, int y, int trans=0xc000)
{
	int zerowidth = screen->Font->GetCharWidth('0');

	x+=zerowidth/2;
	for(int i=0;text[i];i++)
	{
		screen->DrawChar(color, x, y, text[i],
			DTA_KeepRatio, true,
			DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, trans,
			DTA_CenterBottomOffset, 1, TAG_DONE);
		x+=zerowidth;
	}
}


//---------------------------------------------------------------------------
//
// Draws a numberses a fixed widh for all characters
//
//---------------------------------------------------------------------------

static void DrawHudNumber(int color, int num, int x, int y, int trans=0xc000)
{
	char text[15];

	sprintf(text, "%d", num);
	DrawHudText(color, text, x, y, trans);
}


//===========================================================================
//
// draw the status (number of kills etc)
//
//===========================================================================

static void DrawStatus(player_t * CPlayer, int x, int y)
{
	char tempstr[50];
	int space;
	char line[64+10];
	//char line2[64+10];


	screen->SetFont(SmallFont);

	if (hud_showstats)
	{

		int totaltime = level.totaltime / TICRATE;
		int time = level.maptime / TICRATE;
		int timeleft = level.partime-time>0?level.partime-time:0;
		int goldWadScore=0;
		int goldLvlScore=0;
		int spread=0;
		int spreadLvl=0;
		const char *wadhiscorename=players[consoleplayer].userinfo.netname; //for remote hi scoring
		const char *lvlhiscorename=players[consoleplayer].userinfo.netname; //for remote hi scoring
		float adjParTime = float((level.partime-time)/(level.partime*5.0));

			if(time>level.partime){

					int minsLeft=0;
					int secsLeft=0;

					//Only give a max of 20 minutes of extra time to complete a level if past the partime
					//and a min of 10 minutes.
					//before giving 0 points.
					//Some levels with par times of >30 minutes, w.o. adjusted par times can take 3 hours
					//to penalise the player to 0 points. Thats too long.

					//Never allow more than 20 minutes past par time for any level to complete
					if(level.partime>1200){
							adjParTime = float((level.partime-time)/(1200.0));
							//partTimeOverMsg = "Par Time Over. 20 Minutes Left";
							minsLeft=20;
					}else if(level.partime<600){
					//Never allow less than 10 minutes past par time for any level to complete
							adjParTime = float((level.partime-time)/(600.0));
							//partTimeOverMsg = "Par Time Over. 10 Minutes Left";
							minsLeft=10;
					}else{
						//for par times between 10 and 20 minutes, allow the actual par time to finish
						//after par time
						adjParTime = float((level.partime-time))/float((level.partime));
						//partTimeOverMsg = "Par Time Over. 20 Minutes Left.";
						minsLeft = level.partime/60;
						secsLeft = level.partime%60;
						//Printf (PRINT_MEDIUM, "adjpartime:%d\n", float((level.partime-time))/ float((level.partime)));
						//sprintf(partTimeOverMsg,"Par Time Over. %d Minutes Left",minsLeft);

					}

					if(!ptOverShowOnceHUD){
						//const char *partTimeOverMsg;
						char* partTimeOverMsg = new char[50];
						if(secsLeft!=0){
							sprintf(partTimeOverMsg,"Par Time Over! %d Minutes %d Seconds Left\nFor Scoring",minsLeft,secsLeft);
						}else{
							sprintf(partTimeOverMsg,"Par Time Over! %d Minutes Left\nFor Scoring",minsLeft);
						}
						C_MidPrint (partTimeOverMsg);

						S_Sound (CHAN_VOICE, "misc/sdtout", 1, ATTN_NORM);


						ptOverShowOnceHUD=true;

						delete [] partTimeOverMsg;
						partTimeOverMsg = NULL;

					}
				}else{
					ptOverShowOnceHUD=false;
				}


		int curLvlScore=CPlayer->level_score + int(adjParTime*float(CPlayer->level_score));
		int curLvlScoreHUD=0;

		//GHK: Cant have less than zero points.
		if(curLvlScore<0)
			curLvlScore=0;


			int leaderScore=0;
					int leaderLvlScore=0;
					int leaderWadScore=0;
					char *name;
					int  leadernum=0;
					bool tied = false;

					for (int i=0 ; i<MAXPLAYERS ; i++)
					{
						if(playeringame[i]&&((players[i].level_score+players[i].wad_score)>leaderScore)){

							leaderScore=players[i].level_score+players[i].wad_score;
							leaderLvlScore=players[i].level_score;
							leaderWadScore = players[i].wad_score;
							name = players[i].userinfo.netname;
							leadernum = i;
							tied=false;
						}else if(playeringame[i]&&(players[i].level_score + players[i].wad_score)==leaderScore){
							tied=true;
						}


					}



					//dont show if scores are tied and not greater than 0
					if(leaderScore>0&&tied==false){
						//calculate the real scorebased on current par bonus.

						//GHK: Used for calculating infighting bonuses.
						//*BEFORE par bonus added*.
						level.info->leader_score=leaderLvlScore;


						leaderLvlScore = leaderLvlScore+ int(adjParTime*float(leaderLvlScore));

						//make sure the leaderLvlScore is not below zero, since no score really can be.
						leaderLvlScore=(leaderLvlScore<0)?0:leaderLvlScore;


						leaderScore=leaderWadScore + leaderLvlScore;

						//leaderScore=leaderWadScore + leaderLvlScore+ int(float((level.partime-time)/(level.partime*10.0))*float(leaderLvlScore));
						//for displaying hi scores beaten c_midprintbold below:
						//leaderLvlScore = leaderLvlScore+ int(float((level.partime-time)/(level.partime*10.0))*float(leaderLvlScore));

						//only display in multiplayer
						if (multiplayer){

							if(leaderScore-(CPlayer->wad_score+curLvlScore)==0){
									goldWadScore=1;

								///sprintf (line,"WAD/EPI SCORE:" TEXTCOLOR_GOLD " %d",CPlayer->wad_score+curLvlScore);	// Total score
								///screen->DrawText (highlight, 8, yghk, line, DTA_CleanNoMove, true, TAG_DONE);
							}else{
								goldWadScore=0;
								///sprintf (line,"WAD/EPI SCORE:" TEXTCOLOR_WHITE " %d",CPlayer->wad_score+curLvlScore);	// Total score
								///screen->DrawText (highlight, 8, yghk, line, DTA_CleanNoMove, true, TAG_DONE);

							}


							//y -= height/2;
							///y =height/2;
							if(leaderScore-(CPlayer->wad_score+curLvlScore)!=0){
								sprintf (line,"LEADER:\n" TEXTCOLOR_GOLD "%s" TEXTCOLOR_RED " WITH" TEXTCOLOR_WHITE" %d",name,leaderScore);	// Total time
								//screen->DrawText (highlight, SCREENWIDTH-105*CleanXfac, y, line, TAG_DONE);

								//y =+27;
								spread=leaderScore-(CPlayer->wad_score+curLvlScore);
								///sprintf (line,"SPREAD: " TEXTCOLOR_WHITE" -%d",leaderScore-(CPlayer->wad_score+curLvlScore));	// Total time
								///screen->DrawText (highlight, SCREENWIDTH-105*CleanXfac, y, line, TAG_DONE);
							}else
							{
								if(!tied){
									///sprintf (line,TEXTCOLOR_GOLD"YOU ARE THE LEADER\n");	// Total time
									///screen->DrawText (highlight, SCREENWIDTH-105*CleanXfac, y, line, TAG_DONE);
								}else{
									///sprintf (line,TEXTCOLOR_RED"YOU ARE TIED FOR THE LEAD\n");	// Total time
									///screen->DrawText (highlight, SCREENWIDTH-105*CleanXfac, y, line, TAG_DONE);
								}

							}


						}


						if(leadernum==consoleplayer&&lastleadernum!=consoleplayer){
							C_MidPrint("You Have Taken The Lead!");
							S_Sound (CHAN_VOICE, "announcer/q3a/youvetakenthelead", 1, ATTN_NORM);
						}
						else if(leadernum!=consoleplayer&&lastleadernum==consoleplayer){
							C_MidPrint("You Have Lost The Lead!");
							S_Sound (CHAN_VOICE, "announcer/q3a/youvelostthelead", 1, ATTN_NORM);

						}

						lastleadernum=leadernum;
					}else{

						if (multiplayer){
								goldWadScore=0;
								///sprintf (line,"WAD/EPI SCORE:" TEXTCOLOR_WHITE " %d",CPlayer->wad_score+curLvlScore);	// Total score
								///screen->DrawText (highlight, 8, yghk, line, DTA_CleanNoMove, true, TAG_DONE);
						}
					}



	if(!multiplayer){

		//predict final score based on completion of ALL bonuses for level
		//int bonusscore=0;
		//int bonusscore2=0;
		/*
		if(level.total_monsters>0&&players[0].killcount>=level.total_monsters){
						bonusscore2+=(level.total_monsters*10>=100?level.total_monsters*10:100);


					}
					if(level.total_barrels>0&&players[0].barrelcount>=level.total_barrels){
							bonusscore2+=(level.total_barrels*10>=100?level.total_barrels*10:100);


					}
					if(level.total_items>0&&players[0].itemcount>=level.total_items){
							bonusscore2+=(level.total_items*10>=100?level.total_items*10:100);


					}
					if(level.total_secrets>0&&players[0].secretcount>=level.total_secrets){
							bonusscore2+=(level.total_secrets*10>=100?level.total_secrets*10:100);

					}
				*/

				//bonusscore2=0; //***SET BACK TO ZERO FOR NOW

				//curLvlScoreHUD=CPlayer->level_score + bonusscore2 + int(adjParTime*float(CPlayer->level_score+bonusscore2));


				if(level.info->level_score==NULL)
					level.info->level_score=0;

				if(level.info->wad_score==NULL)
					level.info->wad_score=0;


				//orginals... comment out bottom 2 lines
				spread=(level.info->wad_score-leaderScore)>0?(level.info->wad_score-leaderScore):0;
				spreadLvl=(level.info->level_score-leaderLvlScore)>0?(level.info->level_score-leaderLvlScore):0;

				//spread=(level.info->wad_score-CPlayer->wad_score-curLvlScoreHUD)>0?(level.info->wad_score-CPlayer->wad_score-curLvlScoreHUD):0;
				//spreadLvl=(level.info->level_score-curLvlScoreHUD)>0?(level.info->level_score-curLvlScoreHUD):0;


				int displayLvlScore=level.info->level_score;
				int displayWadScore=level.info->wad_score;
				const char *beatHiScoremsg;
				///const char *wadhiscorename=players[consoleplayer].userinfo.netname; //for remote hi scoring
				///const char *lvlhiscorename=players[consoleplayer].userinfo.netname; //for remote hi scoring
				//bool wadHiScorebeatenHUD = true; //only show once. Though may show more than once due to par penalties
				//bool lvlHiScorebeatenHUD = true; //only show once. Though may show more than once due to par penalties

				//this gets displayed to all players, so in MP, everyone will see the
				//update, even if it is getting updated by the 'local' player's score
				if(leaderScore>level.info->wad_score){
					displayWadScore=leaderScore;
				}

				if(leaderScore>=level.info->wad_score&&level.info->wad_score>0){ //Need this to display at the beginning of the next level
					//if(!(wadHiScorebeatenHUD)&&level.info->wad_score>0){
							wadhiscorename=players[consoleplayer].userinfo.netname;
							//only display hi score beaten message locally to
							//the player who beat it
							if(consoleplayer==leadernum){
								//if (time > lastWHSDtime + 5*TICRATE || lastWHSDtime>time){//not more frequent than every 5 secs. second conditional for new levels.
									beatHiScoremsg=TEXTCOLOR_GOLD"\nWad/Episode Hi Score Beaten";
									//C_MidPrintBold (beatHiScoremsg);
									//Printf (PRINT_MEDIUM, TEXTCOLOR_LIGHTBLUE"%s\n", beatHiScoremsg);
									//S_Sound (players[consoleplayer].mo,CHAN_VOICE, "misc/wdhscr", 1, ATTN_NORM);

									//lastWHSDtime = time;
								//}

								if(leaderLvlScore>level.info->level_score){//level hi score message always on top
								//y =height/2;
									lvlhiscorename=players[consoleplayer].userinfo.netname; //for remote hi scoring
									beatHiScoremsg=TEXTCOLOR_GOLD"\nWad/Episode Hi Score Beaten";
									goldWadScore=1;
									///y= (height)/2;
									///screen->DrawText (highlight, SCREENWIDTH-105*CleanXfac, y, beatHiScoremsg, TAG_DONE);
								}else{//no level hi score message, so put the wad/epi message on top
									lvlhiscorename=lvl_hiscore_name;
									beatHiScoremsg=TEXTCOLOR_GOLD"Wad/Episode Hi Score Beaten";
									///y =height/4;
									//if (state == HUD_Fullscreen||HUD_None){
										//y =6*height/2;
									//}
									goldWadScore=1;
									///screen->DrawText (highlight, SCREENWIDTH-105*CleanXfac, y, beatHiScoremsg, TAG_DONE);


								}

							}
						//Play sounds, or a buzzer
					   //if the wad his core was just beaten

							//if(!wadHiScorebeatenHUD&&level.info->wad_score>0&&leaderScore>0){
								//DoVoiceAnnounceIntermission (true);
							//}


						//lastWHSDtime = time;
						wadHiScorebeatenHUD=true;
						wadHiScorebeatenHUDReal=true;
					//}
				}else{
					if(level.info->wad_score>=leaderScore){
						wadhiscorename=wad_hiscore_name;
						//if(wadHiScorebeatenHUDReal&&time>1){ //dont show right away, need to have time>1!!
						//show lose message
							//if (time > lastWHSDtimeL + 5*TICRATE){//not more frequent than every 5 secs.
								//Printf (PRINT_MEDIUM, TEXTCOLOR_GREEN"%s\n","Wad/Episode Hi Score Lost!");
								//S_Sound (players[consoleplayer].mo,CHAN_VOICE, "misc/sdhscr", 1, ATTN_NORM);

							//}
						//}
						//Play sounds, or a buzzer
					   //if the wad his core was just beaten
							//if(wadHiScorebeatenHUD&&level.info->wad_score>0&&leaderScore>0){
								//DoVoiceAnnounceIntermission (false);
							//}

						//lastWHSDtimeL = time;
						wadHiScorebeatenHUDReal=false;
						wadHiScorebeatenHUD=false;
					}
				}

				//ghk: set above now:leaderLvlScore = leaderLvlScore+ int(float((level.partime-time)/(level.partime*10.0))*float(leaderLvlScore));

				if(leaderLvlScore>level.info->level_score){
					lvlhiscorename=players[consoleplayer].userinfo.netname; //for remote hi scoring
					displayLvlScore=leaderLvlScore;


					//if(!(lvlHiScorebeatenHUD)&&level.info->level_score>0){

						//only display hi score beaten message locally to
						//the player who beat it
						if(consoleplayer==leadernum){
							//if (time > lastHSDtime + 5*TICRATE){//not more frequent than every 5 secs.

								beatHiScoremsg=TEXTCOLOR_GOLD"Level Hi Score Beaten";
								//C_MidPrintBold (beatHiScoremsg);
								//Printf (PRINT_MEDIUM, TEXTCOLOR_WHITE"%s\n", beatHiScoremsg);
								//S_Sound (players[consoleplayer].mo,CHAN_VOICE, "misc/ldhscr", 1, ATTN_NORM);
								//lastHSDtime = time;
								goldLvlScore=1;
								///y =height/4;
								///screen->DrawText (highlight, SCREENWIDTH-105*CleanXfac, y, beatHiScoremsg, TAG_DONE);
							//}
						}

						//Play sounds, or a buzzer
					   //if the wad his core was just beaten
							//if(!lvlHiScorebeatenHUD&&level.info->level_score>0&&leaderLvlScore>0){
								//DoVoiceAnnounceIntermission (true);
							//}
						//lastHSDtime = time;
						lvlHiScorebeatenHUD=true;
						lvlHiScorebeatenHUDReal=true;
					//}

				}else{
					if(level.info->level_score>=leaderLvlScore){
						lvlhiscorename=lvl_hiscore_name; //for remote hi scoring
						goldLvlScore=0;
						//if(lvlHiScorebeatenHUDReal&&time>1){ //dont show right away, need to have time>1!!
						//show lose message
							//if (time > lastHSDtimeL + 5*TICRATE){//not more frequent than every 5 secs.

								//Printf (PRINT_MEDIUM, TEXTCOLOR_GREEN"%s\n","Level Hi Score Lost!");
								//S_Sound (players[consoleplayer].mo,CHAN_VOICE, "misc/sdhscr", 1, ATTN_NORM);
								//lastHSDtimeL = time;
							//}
						//}
						//lastHSDtimeL = time;
						//Play sounds, or a buzzer
					   //if the wad his core was just beaten
							//if(lvlHiScorebeatenHUD&&level.info->level_score>0&&leaderLvlScore>0){
								//DoVoiceAnnounceIntermission (false);
							//}

						lvlHiScorebeatenHUDReal=false;
						lvlHiScorebeatenHUD=false;
						//lastHSDtime = 0; //reset the

					}
				}

				//GHK: reset for multiplayer co-op hi score
				//y = ::ST_Y - height;

				if(sd_global_hiscores&&sd_remote_hiscores_ok){
					//get names



					///y=yHS-5+height*(0.6);
					//y = height;
					///sprintf (line,"BY:" TEXTCOLOR_GREEN" %s",wadhiscorename);
					///screen->DrawText (highlight, SCREENWIDTH-100*CleanXfac, y, line, TAG_DONE);
					///y -= height/2;


					///sprintf (line,"WAD/EPI HI SCORE:" TEXTCOLOR_GOLD" %d",displayWadScore);
					///screen->DrawText (highlight, SCREENWIDTH-100*CleanXfac, y, line, TAG_DONE);
					//screen->DrawText (highlight, 8, y, line, DTA_CleanNoMove, true, TAG_DONE);

					///y -= height/2;

					///sprintf (line,"BY:" TEXTCOLOR_GREEN" %s",lvlhiscorename);
					///screen->DrawText (highlight, SCREENWIDTH-100*CleanXfac, y, line, TAG_DONE);
					//screen->DrawText (highlight, 8, y, line, DTA_CleanNoMove, true, TAG_DONE);


					///y -= height/2;

					///sprintf (line,"LEVEL HI SCORE:" TEXTCOLOR_GOLD" %d",displayLvlScore);
					///screen->DrawText (highlight, SCREENWIDTH-100*CleanXfac, y, line, TAG_DONE);
					//screen->DrawText (highlight, 8, y, line, DTA_CleanNoMove, true, TAG_DONE);

					if(sd_pistolstart>0){

						///y -= height/2;

						///sprintf (line,TEXTCOLOR_GOLD "PISTOL START MODE %s","");
						///screen->DrawText (highlight, SCREENWIDTH-100*CleanXfac, y, line, TAG_DONE);
						//screen->DrawText (highlight, 8, y, line, DTA_CleanNoMove, true, TAG_DONE);


					}

					if(sd_noinfight>0){

						///y -= height/2;

						///sprintf (line,TEXTCOLOR_GOLD "NO-INFIGHTING MODE %s","");
						///screen->DrawText (highlight, SCREENWIDTH-100*CleanXfac, y, line, TAG_DONE);
						//screen->DrawText (highlight, 8, y, line, DTA_CleanNoMove, true, TAG_DONE);


					}

					if(sd_global_hiscores>1){

						///y -= height/2;

						///sprintf (line,TEXTCOLOR_GOLD "HARDCORE MODE %s","");
						///screen->DrawText (highlight, SCREENWIDTH-100*CleanXfac, y, line, TAG_DONE);
						//screen->DrawText (highlight, 8, y, line, DTA_CleanNoMove, true, TAG_DONE);


					}


				}else{

					///y=yHS-5+height*(0.6);
					//y = height;
					///sprintf (line,"WAD/EPI HI SCORE:" TEXTCOLOR_GOLD" %d",displayWadScore);
					///screen->DrawText (highlight, SCREENWIDTH-100*CleanXfac, y, line, TAG_DONE);
					//screen->DrawText (highlight, 8, y, line, DTA_CleanNoMove, true, TAG_DONE);



					///y -= height/2;

					///sprintf (line,"LEVEL HI SCORE:" TEXTCOLOR_GOLD" %d",displayLvlScore);
					///screen->DrawText (highlight, SCREENWIDTH-100*CleanXfac, y, line, TAG_DONE);
					//screen->DrawText (highlight, 8, y, line, DTA_CleanNoMove, true, TAG_DONE);

					if(sd_global_hiscores&&!sd_remote_hiscores_ok){ //Show player online scoring is not enabled
						///y -= 4*height/2;
						///char lineghk[200];
						///sprintf (lineghk,TEXTCOLOR_PURPLE"*ONLINE HI SCORING DISABLED*\nCheck Console for problems\n and restart ScoreDoom\n for online hi scoring");
						///screen->DrawText (highlight, SCREENWIDTH-100*CleanXfac, y, lineghk, TAG_DONE);

					}

				}


			//if (am_showtotaltime)
			//{
				//}
			}


		space=SmallFont->StringWidth("WS: ");

		if(!multiplayer){

			if(sd_global_hiscores&&!sd_remote_hiscores_ok){
				y-=4*SmallFont->GetHeight()-1;

				sprintf (tempstr,TEXTCOLOR_PURPLE"*ONLINE HI SCORING DISABLED*\nCheck Console for problems\n and restart ScoreDoom\n for online hi scoring");
				screen->DrawText(hudcolor_statnames, x, y, tempstr,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

			}

			int i=0;
			y-=SmallFont->GetHeight()-1;
			screen->DrawText(hudcolor_statnames, x, y, "MD:",
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

			if(sd_pistolstart>0){

				i+=1;
			}

			if(sd_noinfight>0){

				i+=2;
			}

			if(sd_global_hiscores>1){

				i+=4;
			}

			switch (i){

				case 0:
					sprintf(tempstr, TEXTCOLOR_GREEN"RG");
					break;

				case 1:
					sprintf(tempstr,  TEXTCOLOR_LIGHTBLUE"PS");
					break;

				case 2:
					sprintf(tempstr,  TEXTCOLOR_ORANGE"NI");
					break;


				case 4:
					sprintf(tempstr,  TEXTCOLOR_RED"HC");
					break;
//
				case 3:
					sprintf(tempstr, TEXTCOLOR_BLUE"PS"TEXTCOLOR_WHITE" "TEXTCOLOR_ORANGE"NI");
					break;

				case 5:
					sprintf(tempstr, TEXTCOLOR_RED"HC"TEXTCOLOR_WHITE" "TEXTCOLOR_LIGHTBLUE"PS");
					break;

				case 6:
					sprintf(tempstr, TEXTCOLOR_RED"HC"TEXTCOLOR_WHITE" "TEXTCOLOR_ORANGE"NI");
					break;

				case 7:
					sprintf(tempstr, TEXTCOLOR_RED"HC"TEXTCOLOR_WHITE" "TEXTCOLOR_LIGHTBLUE"PS"TEXTCOLOR_WHITE" "TEXTCOLOR_ORANGE"NI");
					break;


			}

				screen->DrawText(hudcolor_statnames, x+space, y, tempstr,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);


		}


			y-=SmallFont->GetHeight()-1;
		screen->DrawText(hudcolor_statnames, x, y, "TL:",
			DTA_KeepRatio, true,
			DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

		if(timeleft==0)
			sprintf (tempstr, TEXTCOLOR_RED"%02d:%02d:%02d", timeleft/3600, (timeleft%3600)/60, timeleft%60);	// Time Left
		else
			sprintf (tempstr, "%02d:%02d:%02d", timeleft/3600, (timeleft%3600)/60, timeleft%60);	// Time Left

		screen->DrawText(hudcolor_stats, x+space, y, tempstr,
			DTA_KeepRatio, true,
			DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);


		if(sd_global_hiscores&&sd_remote_hiscores_ok&&spread&&!goldWadScore){
			y-=SmallFont->GetHeight()-1;

			sprintf(tempstr, TEXTCOLOR_GOLD"%s "TEXTCOLOR_GREEN"-%d ",wadhiscorename,spread);
			screen->DrawText(hudcolor_stats, x+SmallFont->StringWidth(" "), y, tempstr,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

		}else if(!multiplayer&&spread&&!goldWadScore){ //offline

			y-=SmallFont->GetHeight()-1;

			sprintf(tempstr, TEXTCOLOR_GREEN"-%d ",spread);
			screen->DrawText(hudcolor_stats, x+SmallFont->StringWidth(" "), y, tempstr,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

		}

		if(multiplayer&&spread){
			y-=SmallFont->GetHeight()-1;

			sprintf(tempstr, TEXTCOLOR_GOLD"%s "TEXTCOLOR_GREEN"-%d ",name,spread);
			screen->DrawText(hudcolor_stats, x+SmallFont->StringWidth(" "), y, tempstr,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

		}

		y-=SmallFont->GetHeight()-1;
		screen->DrawText(hudcolor_statnames, x, y, "WS:",
			DTA_KeepRatio, true,
			DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

		if(goldWadScore){
			sprintf(tempstr, TEXTCOLOR_GOLD"%i ",  CPlayer->wad_score + curLvlScore);
			screen->DrawText(hudcolor_stats, x+space, y, tempstr,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);
		}else{
			sprintf(tempstr, "%i ",  CPlayer->wad_score + curLvlScore);
			screen->DrawText(hudcolor_stats, x+space, y, tempstr,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);
		}


		if(sd_global_hiscores&&sd_remote_hiscores_ok&&spreadLvl&&!goldLvlScore){
			y-=SmallFont->GetHeight()-1;

			sprintf(tempstr, TEXTCOLOR_GOLD"%s "TEXTCOLOR_GREEN"-%d ",lvlhiscorename,spreadLvl);
			screen->DrawText(hudcolor_stats, x+SmallFont->StringWidth(" "), y, tempstr,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

		}else if(!multiplayer&&spreadLvl&&!goldLvlScore){ //offline

			y-=SmallFont->GetHeight()-1;

			sprintf(tempstr, TEXTCOLOR_GREEN"-%d ",spreadLvl);
			screen->DrawText(hudcolor_stats, x+SmallFont->StringWidth(" "), y, tempstr,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

		}


		y-=SmallFont->GetHeight()-1;
		screen->DrawText(hudcolor_statnames, x, y, "LS:",
			DTA_KeepRatio, true,
			DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

		if(goldLvlScore){
			sprintf(tempstr, TEXTCOLOR_GOLD"%i ",curLvlScore);
			screen->DrawText(hudcolor_stats, x+space, y, tempstr,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);
		}else{
			sprintf(tempstr, "%i ",curLvlScore);
			screen->DrawText(hudcolor_stats, x+space, y, tempstr,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);
		}
	}
	else
		space=SmallFont->StringWidth("K: ");

	if (!deathmatch)
	{
		// FIXME: ZDoom doesn't preserve the player's stat counters across hubs so this doesn't
		// work in cooperative hub games

		if (hud_showbarrels)
			{
				y-=SmallFont->GetHeight()-1;
				screen->DrawText(hudcolor_statnames, x, y, "B:",
					DTA_KeepRatio, true,
					DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

				sprintf(tempstr, "%i/%i ", multiplayer? CPlayer->barrelcount : level.killed_barrels, level.total_barrels);
				screen->DrawText(hudcolor_stats, x+space, y, tempstr,
					DTA_KeepRatio, true,
					DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);
			}


		if (hud_showsecrets)
		{
			y-=SmallFont->GetHeight()-1;
			screen->DrawText(hudcolor_statnames, x, y, "S:",
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

			sprintf(tempstr, "%i/%i ", multiplayer? CPlayer->secretcount : level.found_secrets, level.total_secrets);
			screen->DrawText(hudcolor_stats, x+space, y, tempstr,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);
		}

		if (hud_showitems)
		{
			y-=SmallFont->GetHeight()-1;
			screen->DrawText(hudcolor_statnames, x, y, "I:",
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

			sprintf(tempstr, "%i/%i ", multiplayer? CPlayer->itemcount : level.found_items, level.total_items);
			screen->DrawText(hudcolor_stats, x+space, y, tempstr,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);
		}

		if (hud_showmonsters)
		{
			y-=SmallFont->GetHeight()-1;
			screen->DrawText(hudcolor_statnames, x, y, "K:",
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

			sprintf(tempstr, "%i/%i ", multiplayer? CPlayer->killcount : level.killed_monsters, level.total_monsters);
			screen->DrawText(hudcolor_stats, x+space, y, tempstr,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);
		}

			if (CPlayer->lastkilltime >= 0)
			{

				if (!(CPlayer->lastkilltime < level.time - 3*TICRATE)&&CPlayer->multicount>0)
				//if (source->player->lastkilltime < level.time - 4*TICRATE)
				{
					space=SmallFont->StringWidth("WS: ");
					y-=SmallFont->GetHeight()-1;

					screen->DrawText(hudcolor_statnames, x, y, "CC:",
						DTA_KeepRatio, true,
						DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

					sprintf(tempstr, TEXTCOLOR_GOLD"%i", CPlayer->multicount);
					screen->DrawText(hudcolor_stats, x+space, y, tempstr,
						DTA_KeepRatio, true,
						DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

						//
					space=SmallFont->StringWidth("");
					int timeleft=(level.time-CPlayer->lastkilltime);
					screen->SetFont(SmallFont2);
					if(timeleft>=70&&timeleft<105){
						y-=SmallFont2->GetHeight()-1;
						sprintf(tempstr, TEXTCOLOR_RED"%s", "  I  ");
						screen->DrawText(hudcolor_stats, x+space, y, tempstr,
							DTA_KeepRatio, true,
							DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);
					}else if(timeleft>=35&&timeleft<70){
						y-=SmallFont2->GetHeight()-1;
						sprintf(tempstr, TEXTCOLOR_ORANGE"%s", " I I ");
						screen->DrawText(hudcolor_stats, x+space, y, tempstr,
							DTA_KeepRatio, true,
							DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);
					}else if(timeleft>=0&&timeleft<35){
						y-=SmallFont2->GetHeight()-1;
						sprintf(tempstr, TEXTCOLOR_GOLD"%s", "I I I");
						screen->DrawText(hudcolor_stats, x+space, y, tempstr,
							DTA_KeepRatio, true,
							DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);
					}

					if(hud_spreecenter){
						screen->SetFont(ConFont);
						char printstr[256];

						int num_digits=0;
						int myNumber=CPlayer->multicount;
						while(myNumber > 0) {
							num_digits++;
							myNumber/=10;
						}

						int length=num_digits*SmallFont->GetCharWidth('0');
						int fonth=SmallFont->GetHeight()+1;
						int bottom=hudheight-1;
						sprintf(printstr, "%d", CPlayer->multicount);
						DrawHudText(CR_GREEN, printstr, (hudwidth/2)-length/2, hudheight*0.7, 0xc000);


						if(timeleft>=70&&timeleft<105){
							int length=ConFont->GetCharWidth('|');

							DrawHudText(CR_RED, "|", (hudwidth/2)-length/2, hudheight*0.7+fonth, 0xc000);
						}else if(timeleft>=35&&timeleft<70){
							int length=2*ConFont->GetCharWidth('|');
							DrawHudText(CR_ORANGE, "||", (hudwidth/2)-length/2, hudheight*0.7+fonth, 0xc000);
						}else if(timeleft>=0&&timeleft<35){
							int length=3*ConFont->GetCharWidth('|');
							DrawHudText(CR_GOLD, "|||", (hudwidth/2)-length/2, hudheight*0.7+fonth, 0xc000);
						}




					}
				}else{

					space=SmallFont->StringWidth("WS: ");
					y-=SmallFont->GetHeight()-1;
					screen->DrawText(hudcolor_statnames, x, y, "CC:",
						DTA_KeepRatio, true,
						DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);

					sprintf(tempstr, "%i", 0);
					screen->DrawText(hudcolor_stats, x+space, y, tempstr,
						DTA_KeepRatio, true,
						DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0xc000, TAG_DONE);


				}



			}


	}
}


//===========================================================================
//
// draw health
//
//===========================================================================

static void DrawHealth(int health, int x, int y)
{
	// decide on the color first
	int fontcolor =
		health < hud_health_red ? CR_RED :
		health < hud_health_yellow ? CR_GOLD :
		health <= hud_health_green ? CR_GREEN :
		CR_BLUE;

	DrawImageToBox(healthpic, x, y, 31, 17);

	screen->SetFont(HudFont);
	DrawHudNumber(fontcolor, health, x + 33, y + 17);
}

//===========================================================================
//
// Draw Armor.
// very similar to drawhealth.
//
//===========================================================================

static void DrawArmor(AInventory * armor, int x, int y)
{
	if (armor)
	{
		int ap=armor->Amount;

		// decide on color
		int fontcolor =
			ap < hud_armor_red ? CR_RED :
			ap < hud_armor_yellow ? CR_GOLD :
			ap <= hud_armor_green ? CR_GREEN :
			CR_BLUE;


		if (ap)
		{
			DrawImageToBox(TexMan[armor->Icon], x, y, 31, 17);

			screen->SetFont(HudFont);
			DrawHudNumber(fontcolor, ap, x + 33, y + 17);
		}
	}
}

//===========================================================================
//
// KEYS
//
//===========================================================================

//---------------------------------------------------------------------------
//
// create a sorted list of the defined keys so
// this doesn't have to be done each frame
//
//---------------------------------------------------------------------------
static TArray<const PClass*> KeyTypes, UnassignedKeyTypes;

static int STACK_ARGS ktcmp(const void * a, const void * b)
{
	AKey * key1 = (AKey*)GetDefaultByType ( *(const PClass**)a );
	AKey * key2 = (AKey*)GetDefaultByType ( *(const PClass**)b );
	return key1->KeyNumber - key2->KeyNumber;
}

static void SetKeyTypes()
{
	for(unsigned int i=0;i<PClass::m_Types.Size();i++)
	{
		const PClass * ti = PClass::m_Types[i];

		if (ti->IsDescendantOf(RUNTIME_CLASS(AKey)))
		{
			AKey * key = (AKey*)GetDefaultByType(ti);

			if (key->Icon!=NULL && key->KeyNumber>0)
			{
				KeyTypes.Push(ti);
			}
			else
			{
				UnassignedKeyTypes.Push(ti);
			}
		}
	}
	if (KeyTypes.Size())
	{
		qsort(&KeyTypes[0], KeyTypes.Size(), sizeof(KeyTypes[0]), ktcmp);
	}
	else
	{
		// Don't leave the list empty
		const PClass * ti = RUNTIME_CLASS(AKey);
		KeyTypes.Push(ti);
	}
}

//---------------------------------------------------------------------------
//
// Draw one key
//
// Regarding key icons, Doom's are too small, Heretic doesn't have any,
// for Hexen the in-game sprites look better and for Strife it doesn't matter
// so always use the spawn state's sprite instead of the icon here unless an
// override is specified in ALTHUDCF.
//
//---------------------------------------------------------------------------

static void DrawOneKey(int xo, int & x, int & y, int & c, AInventory * inv)
{
	int icon=0;
	int AltIcon = inv->GetClass()->Meta.GetMetaInt(HUMETA_AltIcon, 0);

	if (AltIcon==-1) return;

	if (AltIcon>0)
	{
		icon = AltIcon;
	}
	else if (inv->SpawnState && inv->SpawnState->sprite.index!=0)
	{
		FState * state = inv->SpawnState;
		if (state &&  (unsigned)state->sprite.index < (unsigned)sprites.Size ())
		{
			spritedef_t * sprdef = &sprites[state->sprite.index];
			spriteframe_t * sprframe = &SpriteFrames[sprdef->spriteframes + state->GetFrame()];
			icon = sprframe->Texture[0];
		}
	}
	if (icon == 0) icon = inv->Icon;

	if (icon > 0)
	{
		x -= 9;
		DrawImageToBox(TexMan[icon], x, y, 8, 10);
		c++;
		if (c>=10)
		{
			x=xo;
			y-=11;
			c=0;
		}
	}
}

//---------------------------------------------------------------------------
//
// Draw all keys
//
//---------------------------------------------------------------------------

static int DrawKeys(player_t * CPlayer, int x, int y)
{
	int yo=y;
	int xo=x;
	int i;
	int c=0;
	AInventory * inv;

	if (!deathmatch)
	{
		if (KeyTypes.Size()==0) SetKeyTypes();

		// First all keys that are assigned to locks (in reverse order of definition)
		for(i=KeyTypes.Size()-1;i>=0;i--)
		{
			if (inv=CPlayer->mo->FindInventory(KeyTypes[i]))
			{
				DrawOneKey(xo, x, y, c, inv);
			}
		}
		// And now the rest
		for(i=UnassignedKeyTypes.Size()-1;i>=0;i--)
		{
			if (inv=CPlayer->mo->FindInventory(UnassignedKeyTypes[i]))
			{
				DrawOneKey(xo, x, y, c, inv);
			}
		}
	}
	if (x==xo && y!=yo) y+=11;
	return y-11;
}


//---------------------------------------------------------------------------
//
// Drawing Ammo
//
//---------------------------------------------------------------------------
static TArray<const PClass *> orderedammos;

static void AddAmmoToList(AWeapon * weapdef)
{

	for(int i=0; i<2;i++)
	{
		const PClass * ti = i==0? weapdef->AmmoType1 : weapdef->AmmoType2;
		if (ti)
		{
			AAmmo * ammodef=(AAmmo*)GetDefaultByType(ti);

			if (ammodef && !(ammodef->ItemFlags&IF_INVBAR))
			{
				unsigned int j;

				for(j=0;j<orderedammos.Size();j++)
				{
					if (ti == orderedammos[j]) break;
				}
				if (j==orderedammos.Size()) orderedammos.Push(ti);
			}
		}
	}
}

static int DrawAmmo(player_t * CPlayer, int x, int y)
{

	int i,j,k;
	char buf[256];
	AInventory * inv;

	AWeapon * wi=CPlayer->ReadyWeapon;

	orderedammos.Clear();

	// Order ammo by use of weapons in the weapon slots
	// Do not check for actual presence in the inventory!
	// We want to show all ammo types that can be used by
	// the weapons in the weapon slots.
	for (k=0;k<NUM_WEAPON_SLOTS;k++) for(j=0;j<MAX_WEAPONS_PER_SLOT;j++)
	{
		const PClass * weap = LocalWeapons.Slots[k].GetWeapon(j);

		if (weap) AddAmmoToList((AWeapon*)GetDefaultByType(weap));
	}

	// Now check for the remaining weapons that are in the inventory but not in the weapon slots
	for(inv=CPlayer->mo->Inventory;inv;inv=inv->Inventory)
	{
		if (inv->IsKindOf(RUNTIME_CLASS(AWeapon)))
		{
			AddAmmoToList((AWeapon*)inv);
		}
	}

	// ok, we got all ammo types. Now draw the list back to front (bottom to top)

	int def_width=ConFont->StringWidth("000/000");
	x-=def_width;
	screen->SetFont(ConFont);
	int yadd=ConFont->GetHeight();

	for(i=orderedammos.Size()-1;i>=0;i--)
	{

		const PClass * type = orderedammos[i];
		AAmmo * ammoitem = (AAmmo*)CPlayer->mo->FindInventory(type);

		AAmmo * inv = ammoitem? ammoitem : (AAmmo*)GetDefaultByType(orderedammos[i]);
		int AltIcon = type->Meta.GetMetaInt(HUMETA_AltIcon, 0);
		int icon = AltIcon != 0? AltIcon : inv->Icon;
		if (icon<=0) continue;

		int trans= (wi && (type==wi->AmmoType1 || type==wi->AmmoType2)) ? 0xc000:0x6000;

		int maxammo = inv->MaxAmount;
		int ammo = ammoitem? ammoitem->Amount : 0;

		sprintf(buf,"%3d/%3d", ammo,maxammo);

		int tex_width= clamp<int>(ConFont->StringWidth(buf)-def_width, 0, 1000);

		int fontcolor=( !maxammo ? CR_GRAY :
						 ammo < ( (maxammo * hud_ammo_red) / 100) ? CR_RED :
						 ammo < ( (maxammo * hud_ammo_yellow) / 100) ? CR_GOLD : CR_GREEN );

		DrawHudText(fontcolor, buf, x-tex_width, y+yadd, trans);
		DrawImageToBox(TexMan[icon], x-20, y, 16, 8, trans);
		y-=10;
	}
	return y;
}


//---------------------------------------------------------------------------
//
// Weapons List
//
//---------------------------------------------------------------------------

static void DrawOneWeapon(player_t * CPlayer, int x, int & y, AWeapon * weapon)
{
	int trans;
	int picnum=-1;

	// Powered up weapons and inherited sister weapons are not displayed.
	if (weapon->WeaponFlags & WIF_POWERED_UP) return;
	if (weapon->SisterWeapon && weapon->IsKindOf(RUNTIME_TYPE(weapon->SisterWeapon))) return;

	trans=0x6666;
	if (CPlayer->ReadyWeapon)
	{
		if (weapon==CPlayer->ReadyWeapon || weapon==CPlayer->ReadyWeapon->SisterWeapon) trans=0xd999;
	}

	FState * state=NULL, *ReadyState;

	int AltIcon = weapon->GetClass()->Meta.GetMetaInt(HUMETA_AltIcon, 0);
	picnum = AltIcon? AltIcon : weapon->Icon;

	if (picnum == 0)
	{
		if (weapon->SpawnState && weapon->SpawnState->sprite.index!=0)
		{
			state = weapon->SpawnState;
		}
		// no spawn state - now try the ready state
		else if ((ReadyState = weapon->FindState(NAME_Ready)) && ReadyState->sprite.index!=0)
		{
			state = ReadyState;
		}
		if (state &&  (unsigned)state->sprite.index < (unsigned)sprites.Size ())
		{
			spritedef_t * sprdef = &sprites[state->sprite.index];
			spriteframe_t * sprframe = &SpriteFrames[sprdef->spriteframes + state->GetFrame()];

			picnum = sprframe->Texture[0];
		}
	}

	if (picnum > 0)
	{
		FTexture * tex = TexMan[picnum];
		int w = tex->GetWidth();
		int h = tex->GetHeight();
		int rh;
		if (w>h) rh=8;
		else rh=16,y-=8;		// don't draw tall sprites too small!
		if(trans==0xd999){
			DrawImageToBox(tex, x-48, y-10, 31, 17, trans);
			y-=16;
		}else{
			DrawImageToBox(tex, x-24, y, 20, rh, trans);
			y-=10;
		}
		
	}
}


static void DrawWeapons(player_t * CPlayer, int x, int y)
{
	int k,j;
	AInventory * inv;

	// First draw all weapons in the inventory that are not assigned to a weapon slot
	for(inv=CPlayer->mo->Inventory;inv;inv=inv->Inventory)
	{
		int slot, index;
		if (inv->IsKindOf(RUNTIME_CLASS(AWeapon)) && !LocalWeapons.LocateWeapon(RUNTIME_TYPE(inv), &slot, &index))
		{
			DrawOneWeapon(CPlayer, x, y, static_cast<AWeapon*>(inv));
		}
	}

	// And now everything in the weapon slots back to front
	for (k=NUM_WEAPON_SLOTS-1;k>=0;k--) for(j=MAX_WEAPONS_PER_SLOT-1;j>=0;j--)
	{
		const PClass * weap = LocalWeapons.Slots[k].GetWeapon(j);
		if (weap)
		{
			inv=CPlayer->mo->FindInventory(weap);
			if (inv)
			{
				DrawOneWeapon(CPlayer, x, y, static_cast<AWeapon*>(inv));
			}
		}
	}
}


//---------------------------------------------------------------------------
//
// Draw the Inventory
//
//---------------------------------------------------------------------------

static void DrawInventory(player_t * CPlayer, int x,int y)
{
	AInventory * rover;
	int numitems = (hudwidth - 2*x) / 32;
	int i;

	CPlayer->mo->InvFirst = rover = StatusBar->ValidateInvFirst(numitems);
	if (rover!=NULL)
	{
		if(rover->PrevInv())
		{
			screen->DrawTexture(invgems[!!(level.time&4)], x-10, y,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0x6666, TAG_DONE);
		}

		for(i=0;i<numitems && rover;rover=rover->NextInv())
		{
			if (rover->Amount>0)
			{
				int AltIcon = rover->GetClass()->Meta.GetMetaInt(HUMETA_AltIcon, 0);

				if (AltIcon>=0 && (rover->Icon>0 || AltIcon>0) )
				{
					int trans = rover==CPlayer->mo->InvSel ? FRACUNIT : 0x6666;

					DrawImageToBox(TexMan[AltIcon? AltIcon : rover->Icon], x, y, 19, 25, trans);
					if (rover->Amount>1)
					{
						char buffer[10];
						int xx;
						sprintf(buffer,"%d",rover->Amount);
						if (rover->Amount>=1000) xx = 32 - IndexFont->StringWidth(buffer);
						else xx = 22;

						screen->SetFont(IndexFont);
						screen->DrawText(CR_GOLD, x+xx, y+20, buffer,
							DTA_KeepRatio, true,
							DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, trans, TAG_DONE);
					}

					x+=32;
					i++;
				}
			}
		}
		if(rover)
		{
			screen->DrawTexture(invgems[2 + !!(level.time&4)], x-10, y,
				DTA_KeepRatio, true,
				DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, DTA_Alpha, 0x6666, TAG_DONE);
		}
	}
}

//---------------------------------------------------------------------------
//
// Draw the Frags
//
//---------------------------------------------------------------------------

static void DrawFrags(player_t * CPlayer, int x, int y)
{
	DrawImageToBox(fragpic, x, y, 31, 17);

	screen->SetFont(HudFont);
	DrawHudNumber(CR_GRAY, CPlayer->fragcount, x + 33, y + 17);
}



//---------------------------------------------------------------------------
//
// PROC DrawCoordinates
//
//---------------------------------------------------------------------------

static void DrawCoordinates(player_t * CPlayer)
{
	fixed_t x;
	fixed_t y;
	fixed_t z;
	char coordstr[18];
	int h=SmallFont->GetHeight()+1;


	if (!map_point_coordinates || !automapactive)
	{
		x=CPlayer->mo->x;
		y=CPlayer->mo->y;
		z=CPlayer->mo->z;
	}
	else
	{
		AM_GetPosition(x,y);
		z = R_PointInSubsector(x, y)->sector->floorplane.ZatPoint(x, y);
	}

	int vwidth = con_scaletext!=2? SCREENWIDTH : SCREENWIDTH/2;
	int vheight = con_scaletext!=2? SCREENHEIGHT : SCREENHEIGHT/2;
	int xpos = vwidth - SmallFont->StringWidth("X:-99999");
	int ypos = 18;

	sprintf(coordstr, "X: %d", x>>FRACBITS);
	screen->DrawText(hudcolor_xyco, xpos, ypos, coordstr,
		DTA_KeepRatio, true,
		DTA_VirtualWidth, vwidth, DTA_VirtualHeight, vheight, TAG_DONE);

	sprintf(coordstr, "Y: %d", y>>FRACBITS);
	screen->DrawText(hudcolor_xyco, xpos, ypos+h, coordstr,
		DTA_KeepRatio, true,
		DTA_VirtualWidth, vwidth, DTA_VirtualHeight, vheight, TAG_DONE);

	sprintf(coordstr, "Z: %d", z>>FRACBITS);
	screen->DrawText(hudcolor_xyco, xpos, ypos+2*h, coordstr,
		DTA_KeepRatio, true,
		DTA_VirtualWidth, vwidth, DTA_VirtualHeight, vheight, TAG_DONE);
}


//---------------------------------------------------------------------------
//
// draw the overlay
//
//---------------------------------------------------------------------------
void HUD_ShowPop(int pop)
{
	if (pop == FBaseStatusBar::POP_Log)
	{
		showlog=!showlog;
	}
	else if (pop == FBaseStatusBar::POP_None)
	{
		showlog=false;
	}
}

void HU_InitHud();

void DrawHUD()
{
	player_t * CPlayer = StatusBar->CPlayer;

	if (HudFont==NULL) HU_InitHud();

	players[consoleplayer].inventorytics = 0;
	if (hud_althudscale && SCREENWIDTH>640)
	{
		// The HUD has been optimized for 640x400 but for the software renderer
		// just double the pixels to reduce scaling artifacts.
		// (Too bad that I don't get the math for widescreen right so I have to do
		// it for widescreen in hardware rendering, too...)
		hudwidth=SCREENWIDTH/2;
		if (currentrenderer==0 || WidescreenRatio != 0) hudheight=SCREENHEIGHT/2;
		else hudheight = hudwidth * 30 / BaseRatioSizes[WidescreenRatio][3];
	}
	else
	{
		hudwidth=SCREENWIDTH;
		hudheight=SCREENHEIGHT;
	}

	float blend[4] = {0,0,0,0};
	StatusBar->BlendView (blend);

	if (!automapactive)
	{
		int i;

		// No HUD in the title level!
		if (gamestate == GS_TITLELEVEL || !CPlayer) return;

		if (!deathmatch) DrawStatus(CPlayer, 5, hudheight-50);
		else
		{
			DrawStatus(CPlayer, 5, hudheight-75);
			DrawFrags(CPlayer, 5, hudheight-70);
		}
		DrawHealth(CPlayer->health, 5, hudheight-45);
		// Yes, that doesn't work properly for Hexen but frankly, I have no
		// idea how to make a meaningful value out of Hexen's armor system!
		DrawArmor(CPlayer->mo->FindInventory(RUNTIME_CLASS(ABasicArmor)), 5, hudheight-20);
		i=DrawKeys(CPlayer, hudwidth-4, hudheight-10);
		i=DrawAmmo(CPlayer, hudwidth-5, i);
		DrawWeapons(CPlayer, hudwidth-5, i);
		DrawInventory(CPlayer, 144, hudheight-28);
		screen->SetFont(SmallFont);
		if (CPlayer->camera && CPlayer->camera->player)
		{
			StatusBar->DrawCrosshair();
		}
		if (showlog)
		{
			if (CPlayer->LogText && *CPlayer->LogText)
			{
				int linelen = hudwidth<640? Scale(hudwidth,9,10)-40 : 560;
				FBrokenLines *lines = V_BreakLines (SmallFont, linelen, CPlayer->LogText);
				int height = 20;

				for (i = 0; lines[i].Width != -1; i++) height += SmallFont->GetHeight () + 1;

				int x,y,w;

				if (linelen<560)
				{
					x=hudwidth/20;
					y=hudheight/8;
					w=hudwidth-2*x;
				}
				else
				{
					x=(hudwidth>>1)-300;
					y=hudheight*3/10-(height>>1);
					if (y<0) y=0;
					w=600;
				}
				screen->Dim(0, 0.5f, Scale(x, SCREENWIDTH, hudwidth), Scale(y, SCREENHEIGHT, hudheight),
									 Scale(w, SCREENWIDTH, hudwidth), Scale(height, SCREENHEIGHT, hudheight));
				x+=20;
				y+=10;
				for (i = 0; lines[i].Width != -1; i++)
				{

					screen->DrawText (CR_UNTRANSLATED, x, y, lines[i].Text,
						DTA_KeepRatio, true,
						DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, TAG_DONE);
					y += SmallFont->GetHeight ()+1;
				}

				V_FreeBrokenLines (lines);
			}
		}
		if (idmypos) DrawCoordinates(CPlayer);
	}
	else
	{
		char printstr[256];
		int seconds;
		cluster_info_t *thiscluster = FindClusterInfo (level.cluster);
		bool hub = !!(thiscluster->flags&CLUSTER_HUB);
		int length=8*SmallFont->GetCharWidth('0');
		int fonth=SmallFont->GetHeight()+1;
		int bottom=hudheight-1;

		screen->SetFont(SmallFont);

		if (am_showtotaltime)
		{
			seconds = level.totaltime / TICRATE;
			sprintf(printstr, "%02i:%02i:%02i", seconds/3600, (seconds%3600)/60, seconds%60);
			DrawHudText(hudcolor_ttim, printstr, hudwidth-length, bottom, FRACUNIT);
			bottom -= fonth;
		}

		if (am_showtime)
		{
			seconds= level.time /TICRATE;
			sprintf(printstr, "%02i:%02i:%02i", seconds/3600, (seconds%3600)/60, seconds%60);
			DrawHudText(hudcolor_time, printstr, hudwidth-length, bottom, FRACUNIT);
			bottom -= fonth;

			// Single level time for hubs
			if (level.clusterflags&CLUSTER_HUB)
			{
				seconds= level.maptime /TICRATE;
				sprintf(printstr, "%02i:%02i:%02i", seconds/3600, (seconds%3600)/60, seconds%60);
				DrawHudText(hudcolor_ltim, printstr, hudwidth-length, bottom, FRACUNIT);
			}
		}

		sprintf(printstr,"%s: %s",level.mapname,level.level_name);
		screen->DrawText(hudcolor_titl, 1, hudheight-fonth-1, printstr,
			DTA_KeepRatio, true,
			DTA_VirtualWidth, hudwidth, DTA_VirtualHeight, hudheight, TAG_DONE);

		//DrawCoordinates(CPlayer);
	}
}

/////////////////////////////////////////////////////////////////////////
//
// Initialize the fonts and other data
//
/////////////////////////////////////////////////////////////////////////

static void SetIcon (const char * item, const char * sc_String)
{
	const PClass * ti = PClass::FindClass(item);
	if (ti)
	{
		AInventory * defaults = (AInventory*)GetDefaultByType(ti);
		if (defaults)
		{
			defaults->Icon = TexMan.AddPatch (sc_String);
			if (defaults->Icon <= 0) defaults->Icon = TexMan.AddPatch (sc_String, ns_sprites);
		}
	}
}


void HU_InitHud()
{
	switch (gameinfo.gametype)
	{
	case GAME_Heretic:
	case GAME_Hexen:
		healthpic=TexMan[TexMan.AddPatch("ARTIPTN2", ns_sprites)];
		HudFont=FFont::FindFont("HUDFONT_RAVEN");
		break;

	case GAME_Strife:
		healthpic=TexMan[TexMan.AddPatch("I_MDKT")];
		HudFont=BigFont;	// Strife doesn't have anything nice so use the standard font
		break;

	default:
		healthpic=TexMan[TexMan.AddPatch("MEDIA0", ns_sprites)];
		HudFont=FFont::FindFont("HUDFONT_DOOM");
		break;
	}

	IndexFont=FFont::FindFont("INDXFONT");
	if (IndexFont == NULL)
	{
		int num = Wads.CheckNumForName ("INDXFONT");
		if (num != -1)
		{
			char head[3];
			{
				FWadLump lump = Wads.OpenLumpNum (num);
				lump.Read (head, 3);
			}
			if (head[0] == 'F' && head[1] == 'O' && head[2] == 'N')
			{
				IndexFont = new FSingleLumpFont ("INDXFONT", num);
			}
		}
	}
	if (IndexFont==NULL) IndexFont=ConFont;	// Emergency fallback

	invgems[0] = TexMan[TexMan.AddPatch("INVGEML1")];
	invgems[1] = TexMan[TexMan.AddPatch("INVGEML2")];
	invgems[2] = TexMan[TexMan.AddPatch("INVGEMR1")];
	invgems[3] = TexMan[TexMan.AddPatch("INVGEMR2")];

	fragpic = TexMan[TexMan.AddPatch("HU_FRAGS")];	// Sadly, I don't have anything usable for this. :(

	KeyTypes.Clear();
	UnassignedKeyTypes.Clear();

	// Now read custom icon overrides
	int lump, lastlump = 0;

	while ((lump = Wads.FindLump ("ALTHUDCF", &lastlump)) != -1)
	{
		SC_OpenLumpNum(lump, "ALTHUDCF");
		while (SC_GetString())
		{
			if (SC_Compare("Health"))
			{
				SC_MustGetString();
				int tex = TexMan.AddPatch(sc_String);
				if (tex<=0) tex = TexMan.AddPatch(sc_String, ns_sprites);
				if (tex>0) healthpic = TexMan[tex];
			}
			else
			{
				const PClass * ti = PClass::FindClass(sc_String);
				if (!ti)
				{
					Printf("Unknown item class '%s' in ALTHUDCF\n", sc_String);
				}
				else if (!ti->IsDescendantOf(RUNTIME_CLASS(AInventory)))
				{
					Printf("Invalid item class '%s' in ALTHUDCF\n", sc_String);
					ti=NULL;
				}
				SC_MustGetString();
				int tex=0;

				if (!SC_Compare("0") && !SC_Compare("NULL") && !SC_Compare(""))
				{
					tex = TexMan.AddPatch(sc_String);
					if (tex<=0) tex = TexMan.AddPatch(sc_String, ns_sprites);
				}
				else tex=-1;

				if (ti) const_cast<PClass*>(ti)->Meta.SetMetaInt(HUMETA_AltIcon, tex);
			}
		}
		SC_Close();
	}

	SetIcon("ArmorBonus", "BON2A0");	// Just a personal preference. ;)
}


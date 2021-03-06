// Cajun bot console commands.
//
// [RH] Moved out of d_netcmd.c (in Cajun source), because they don't really
// belong there.

#include "c_cvars.h"
#include "c_dispatch.h"
#include "b_bot.h"
#include "m_argv.h"
#include "doomstat.h"
#include "p_local.h"
#include "cmdlib.h"

CVAR (Int, bot_next_color, 11, 0)
CVAR (Bool, bot_observer, false, 0)

IMPLEMENT_POINTY_CLASS (DCajunMaster)
 DECLARE_POINTER (getspawned)
 DECLARE_POINTER (botinfo)
 DECLARE_POINTER (firstthing)
 DECLARE_POINTER (body1)
 DECLARE_POINTER (body2)
END_POINTERS

CCMD (addbot)
{
	if (consoleplayer != Net_Arbitrator)
	{
		Printf ("Only player %d can add bots\n", Net_Arbitrator + 1);
		return;
	}

	if (argv.argc() > 2)
	{
		Printf ("addbot [botname] : add a bot to the game\n");
		return;
	}
/*
	if (argc == 3) //Used force colornum
	{
		color = atoi (argv[2]);
		if (color<0) color=0;
		if (color>10) color=10;
	}
*/
	if (argv.argc() > 1)
		bglobal.SpawnBot (argv[1]);
	else
		bglobal.SpawnBot (NULL);
}

void DCajunMaster::ClearPlayer (int i, bool keepTeam)
{
	if (players[i].mo)
	{
		players[i].mo->Destroy ();
		players[i].mo = NULL;
	}
	botinfo_t *bot = botinfo;
	while (bot && stricmp (players[i].userinfo.netname, bot->name))
		bot = bot->next;
	if (bot)
	{
		bot->inuse = false;
		bot->lastteam = keepTeam ? players[i].userinfo.team : TEAM_None;
	}
	players[i].~player_t();
	::new(&players[i]) player_t;
	playeringame[i] = false;
}

CCMD (removebots)
{
	Net_WriteByte (DEM_KILLBOTS);
}

extern bool CheckCheatmode ();

CCMD (freeze)
{
   if (CheckCheatmode ())
      return;

   if (netgame && consoleplayer != Net_Arbitrator)
   {
      Printf ("Only player %d can use freeze mode\n", Net_Arbitrator + 1);
      return;
   }

   Net_WriteByte (DEM_GENERICCHEAT);
   Net_WriteByte (CHT_FREEZE);
}

CCMD (listbots)
{
	botinfo_t *thebot = bglobal.botinfo;
	int count = 0;

	while (thebot)
	{
		Printf ("%s%s\n", thebot->name, thebot->inuse ? " (active)" : "");
		thebot = thebot->next;
		count++;
	}
	Printf ("> %d bots\n", count);
}

FArchive &operator<< (FArchive &arc, botskill_t &skill)
{
	return arc << skill.aiming << skill.perfection << skill.reaction << skill.isp;
}

// set the bot specific weapon information
// This is intentionally not in the weapon definition anymore.
AT_GAME_SET(BotStuff)
{
	AWeapon * w;
	AActor * a;
	
	w = (AWeapon*)GetDefaultByName ("Pistol");
	if (w != NULL)
	{
		w->MoveCombatDist=25000000;
	}
	w = (AWeapon*)GetDefaultByName ("Shotgun");
	if (w != NULL)
	{
		w->MoveCombatDist=24000000;
	}
	w = (AWeapon*)GetDefaultByName ("SuperShotgun");
	if (w != NULL)
	{
		w->MoveCombatDist=15000000;
	}
	w = (AWeapon*)GetDefaultByName ("Chaingun");
	if (w != NULL)
	{
		w->MoveCombatDist=27000000;
	}
	w = (AWeapon*)GetDefaultByName ("RocketLauncher");
	if (w != NULL)
	{
		w->MoveCombatDist=18350080;
		w->WeaponFlags|=WIF_BOT_REACTION_SKILL_THING|WIF_BOT_EXPLOSIVE;
		w->ProjectileType=PClass::FindClass("Rocket");
	}
	w = (AWeapon*)GetDefaultByName ("PlasmaRifle");
	if (w != NULL)
	{
		w->MoveCombatDist=27000000;
		w->ProjectileType=PClass::FindClass("PlasmaBall");
	}
	a = GetDefaultByName ("PlasmaBall");
	if (a != NULL)
	{
		a->flags3|=MF3_WARNBOT;
	}
	w = (AWeapon*)GetDefaultByName ("BFG9000");
	if (w != NULL)
	{
		w->MoveCombatDist=10000000;
		w->WeaponFlags|=WIF_BOT_REACTION_SKILL_THING|WIF_BOT_BFG;
		w->ProjectileType=PClass::FindClass("BFGBall");
	}
}

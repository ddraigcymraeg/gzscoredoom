//ghk new class for scoredoom
#include "info.h"
#include "c_console.h" //ghk
#include "a_pickups.h"
#include "a_artifacts.h"
#include "gstrings.h"
#include "p_local.h"
#include "p_enemy.h"
#include "s_sound.h"
#include "ravenshared.h"

void A_SentrySummon (AActor *);

//GHK

class AArtiSentryServant : public AInventory
{
	DECLARE_ACTOR (AArtiSentryServant, AInventory)
public:
	bool Use (bool pickup);
};

FState AArtiSentryServant::States[] =
{
#define S_ARTI_SSUMMON 0
	S_NORMAL (TURR, 'E',  350, NULL					    , &States[S_ARTI_SSUMMON]),
};

IMPLEMENT_ACTOR (AArtiSentryServant, Hexen, -1, 0 ) // 186, 116)
	PROP_Flags (MF_SPECIAL|MF_COUNTITEM)
	//PROP_Flags2 (MF2_FLOATBOB)
	PROP_SpawnState (S_ARTI_SSUMMON)
	PROP_Inventory_RespawnTics (30+4200)
	PROP_Inventory_DefMaxAmount
	PROP_Inventory_FlagsSet (IF_INVBAR|IF_PICKUPFLASH|IF_FANCYPICKUPSOUND)
	PROP_Inventory_Icon ("TURRZ0")
	PROP_Inventory_PickupSound ("misc/p_pkup")
	PROP_Inventory_PickupMessage("UAC Sentinel Turret")
END_DEFAULTS

// Sentry Summoning Doll -----------------------------------------------------------

class ASSummoningDoll : public AActor
{
	DECLARE_ACTOR (ASSummoningDoll, AActor)
};

FState ASSummoningDoll::States[] =
{
#define S_SSUMMON_FX1_1 0
	S_NORMAL (TURR, 'E',	4, NULL					    , &States[S_SSUMMON_FX1_1]),

#define S_SSUMMON_FX2_1 (S_SSUMMON_FX1_1+1)
	//S_NORMAL (TURR, 'E',	4, NULL					    , &States[S_SSUMMON_FX2_1+1]),
	//S_NORMAL (TURR, 'E',	4, NULL					    , &States[S_SSUMMON_FX2_1+2]),
	S_NORMAL (TURR, 'E',	1, A_SentrySummon				    , NULL),
};

IMPLEMENT_ACTOR (ASSummoningDoll, Hexen, -1, 0)
	PROP_SpeedFixed (4)
	PROP_Flags (MF_NOBLOCKMAP|MF_DROPOFF|MF_MISSILE)
	PROP_Flags2 (MF2_NOTELEPORT)

	PROP_SpawnState (S_SSUMMON_FX1_1)
	PROP_DeathState (S_SSUMMON_FX2_1)
END_DEFAULTS

//============================================================================
//
// Activate the sentry summoning artifact
//
//============================================================================

bool AArtiSentryServant::Use (bool pickup)
{
	AActor *mo = P_SpawnPlayerMissile (Owner, RUNTIME_CLASS(ASSummoningDoll));
	if (mo)
	{
		mo->target = Owner;
		mo->tracer = Owner;
		mo->momz = 5*FRACUNIT;
	}
	return true;
}

void A_SentrySummon (AActor *actor)
{
	//if ((level.flags & (LEVEL_CYBORGSPECIAL|
							//LEVEL_SPIDERSPECIAL|LEVEL_BRUISERSPECIAL)) == 0){//ghk dont spawn on 'boss' levels
		//AMinotaurFriend *mo;
		AActor *mo;

		//const PClass * mi = PClass::FindClass ("TurretA");
		//mi->ActorInfo->Class
		//mo = Spawn<AMinotaurFriend> (actor->x, actor->y, actor->z, ALLOW_REPLACE);
		mo = Spawn ("TurretA", actor->x, actor->y, actor->z, ALLOW_REPLACE);
		if (mo)
		{
			if (P_TestMobjLocation(mo) == false || !actor->tracer)
			{ // Didn't fit - change back to artifact
				mo->Destroy ();
				AActor *arti = Spawn<AArtiSentryServant> (actor->x, actor->y, actor->z, ALLOW_REPLACE);
				if (arti) arti->flags |= MF_DROPPED;
				if(actor->tracer&&actor->tracer->CheckLocalView (consoleplayer)){
					C_MidPrint ("Deployment Failed. Find a Larger Space!");
					S_Sound (actor->tracer, CHAN_VOICE, "ghk/nosumn", 1, ATTN_NORM); //GHK
				}
				return;
			}

			//mo->StartTime = level.maptime;
			if (actor->tracer->flags & MF_CORPSE)
			{	// Master dead
				mo->tracer = NULL;		// No master
			}
			else
			{
				mo->tracer = actor->tracer;		// Pointer to master
				//AInventory *power = Spawn<APowerMinotaur> (0, 0, 0, NO_REPLACE);
				//power->TryPickup (actor->tracer);
				if (actor->tracer->player != NULL)
				{
					mo->FriendPlayer = int(actor->tracer->player - players + 1);
				}
			}

			// Make smoke puff
			//Spawn<AMinotaurSmoke> (actor->x, actor->y, actor->z, ALLOW_REPLACE);
			//S_SoundID (actor, CHAN_VOICE, mo->ActiveSound, 1, ATTN_NORM);
			//S_Sound (actor, CHAN_VOICE, "Balor/Explode", 1, ATTN_NORM); //GHK
		}
	//}else{
		//if(actor->tracer&&actor->tracer->CheckLocalView (consoleplayer)){
		//	C_MidPrint ("Deployment Failed. Boss didnt allow it!");
		//	S_Sound (actor->tracer, CHAN_VOICE, "ghk/nosumn", 1, ATTN_NORM); //GHK
		//}
	//}
}



// END GHK
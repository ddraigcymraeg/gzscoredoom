/*
** a_secrettrigger.cpp
** A thing that counts toward the secret count when activated
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

#include "actor.h"
#include "g_level.h"
#include "c_console.h"
#include "info.h"
#include "s_sound.h"
#include "d_player.h"
#include "v_text.h" //ghk

EXTERN_CVAR(String, secretmessage)

class ASecretTrigger : public AActor
{
	DECLARE_STATELESS_ACTOR (ASecretTrigger, AActor)
public:
	void PostBeginPlay ();
	void Activate (AActor *activator);
};

IMPLEMENT_STATELESS_ACTOR (ASecretTrigger, Any, 9046, 0)
	PROP_Flags (MF_NOBLOCKMAP|MF_NOSECTOR|MF_NOGRAVITY)
	PROP_Flags3 (MF3_DONTSPLASH)
END_DEFAULTS

void ASecretTrigger::PostBeginPlay ()
{
	Super::PostBeginPlay ();
	level.total_secrets++;
}

void ASecretTrigger::Activate (AActor *activator)
{
	if (activator != NULL)
	{
		if (activator->CheckLocalView (consoleplayer))
		{
			if (args[0] <= 1)
			{
				//C_MidPrint (secretmessage);
				//GHK
				if(!(!multiplayer&&level.total_secrets>0&&players[0].secretcount>=level.total_secrets)){
					char ghkmessage[50];
					sprintf (ghkmessage,"Secret Found! +250 Bonus!");
					C_MidPrint (ghkmessage);
				}
			}
			if (args[0] == 0 || args[0] == 2)
			{
				S_Sound (activator, CHAN_AUTO, "misc/secret", 1, ATTN_NORM);
			}
		}
		//if (activator->player) activator->player->secretcount++;
		//GHK: add +1000 points here to?
		if (activator->player){
			char levelscore[10];
			activator->player->level_score+=250;
			activator->player->secretcount++;
			if (activator->CheckLocalView(consoleplayer))
			{

				sprintf(levelscore,"+ %d",250);
				Printf (PRINT_MEDIUM, "%s\n", levelscore);
			}

			if(!multiplayer&&level.total_secrets>0&&players[0].secretcount>=level.total_secrets){
						int bonusscore=(level.total_secrets*10>=100?level.total_secrets*10:100);
						//activator->player->level_score+=bonusscore;

						if (activator->CheckLocalView (consoleplayer))
						{
							sprintf(levelscore,"+ %d",bonusscore);
							Printf (PRINT_MEDIUM, "%s\n", levelscore);
							//C_MidPrint (secretmessage);
							char ghkmessage[80];
								sprintf (ghkmessage,"Secret Found! +250 Bonus!\nAll Secrets Found!\n+%d End Of Level Bonus!",bonusscore);
								C_MidPrintSD (ghkmessage);
							S_Sound (CHAN_AUTO, "announcer/q3a/1000PB", 1, ATTN_NORM);
						}


		}

			//GHK: Reward player 1000, nope, 250 points for finding a secret
			//activator->player->level_score+=250;

		}
	}
	level.found_secrets++;
	Destroy ();
}


#include "fmodfunc.h"

#include <map>
#include <vector>
#include <fmod.h>
#include <fmod_errors.h>
#include "errors.h"

FMOD_SYSTEM* gp_fmodSystem; //!< Système FMOD.
map<string, FMOD_SOUND*> gp_sounds_map; //!< Map liant un nom de fichier audio à un pointeur sur FMOD_SOUND.
vector<string> g_sounds_v; //!< Vecteur des noms de fichiers audio ajoutés (leurs positions dans le vecteur + 1 sont leurs indices de son).

void initFMOD()
{
	FMOD_RESULT fmodResult = FMOD_System_Create(&gp_fmodSystem);
	if (fmodResult != FMOD_OK)
		throw PfException(__LINE__, __FILE__, string("Erreur d'initialisation de FMOD : ") + FMOD_ErrorString(fmodResult));
	fmodResult = FMOD_System_Init(gp_fmodSystem, 32, FMOD_INIT_NORMAL, 0);
	if (fmodResult != FMOD_OK)
		throw PfException(__LINE__, __FILE__, string("Erreur d'initialisation de FMOD : ") + FMOD_ErrorString(fmodResult));
}

unsigned int addSound(const string& soundFileName, bool music)
{
	FMOD_RESULT fmodResult;
	unsigned int x = 0;

	map<string, FMOD_SOUND*>::iterator it = gp_sounds_map.find(soundFileName);
	if (it != gp_sounds_map.end())
	{
		if (it->second != 0)
		{
			fmodResult = FMOD_Sound_Release(it->second);
			if (fmodResult != FMOD_OK)
				throw PfException(__LINE__, __FILE__, string("Erreur lors de la suppression du son ") + it->first + " : " + FMOD_ErrorString(fmodResult) + "\nLe son ne peut pas être ajouté.");
		}
		for (unsigned int i=0, size=g_sounds_v.size();i<size;i++)
        {
            if (soundFileName == g_sounds_v[i])
                x = i + 1;
        }
	}
	else
    {
		g_sounds_v.push_back(soundFileName);
		x = g_sounds_v.size();
    }

	fmodResult = FMOD_System_CreateSound(gp_fmodSystem, soundFileName.c_str(), music?(FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL):FMOD_CREATESAMPLE, 0, &(gp_sounds_map[soundFileName]));
	if (fmodResult != FMOD_OK)
		throw PfException(__LINE__, __FILE__, string("Erreur lors de l'ajout du son ") + soundFileName + " : " + FMOD_ErrorString(fmodResult));

    return x;
}

void playSound(const string& soundFileName, bool music, int loop)
{
	map<string, FMOD_SOUND*>::iterator it = gp_sounds_map.find(soundFileName);
	try
	{
		if (it == gp_sounds_map.end())
		{
			addSound(soundFileName, music);
			it = gp_sounds_map.find(soundFileName);
		}

        FMOD_RESULT fmodResult;

		if (music)
		{
			fmodResult = FMOD_Sound_SetLoopCount(it->second, loop);
			if (fmodResult != FMOD_OK)
				throw PfException(__LINE__, __FILE__, string("Impossible de définir la répétition pour le son ") + soundFileName + " : " + FMOD_ErrorString(fmodResult));
		}
		fmodResult = FMOD_System_PlaySound(gp_fmodSystem, FMOD_CHANNEL_FREE, it->second, 0, 0);
		if (fmodResult != FMOD_OK)
			throw PfException(__LINE__, __FILE__, string("Impossible de jouer le son ") + soundFileName + " : " + FMOD_ErrorString(fmodResult));
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, string("Impossible de jouer le son ") + soundFileName + ".", e);
	}
}

void playSound(unsigned int soundIndex, bool music, int loop)
{
	if (soundIndex == 0 || soundIndex > g_sounds_v.size())
		return;
	playSound(g_sounds_v[soundIndex-1], music, loop);
}

void stopAllSounds()
{
	FMOD_CHANNELGROUP* p_channelGroup;
	FMOD_System_GetMasterChannelGroup(gp_fmodSystem, &p_channelGroup);
	FMOD_ChannelGroup_Stop(p_channelGroup);
}

void freeSounds()
{
	FMOD_RESULT fmodResult;

	for (map<string, FMOD_SOUND*>::iterator it=gp_sounds_map.begin();it!=gp_sounds_map.end();++it)
	{
		if (it->second != 0)
		{
			fmodResult = FMOD_Sound_Release(it->second);
			if (fmodResult != FMOD_OK)
				throw PfException(__LINE__, __FILE__, string("Erreur lors de la suppression du son ") + it->first + " : " + FMOD_ErrorString(fmodResult));
		}
	}

	gp_sounds_map.clear();
	g_sounds_v.clear();
}

void closeFMOD()
{
	FMOD_RESULT fmodResult;

	freeSounds();

	fmodResult = FMOD_System_Close(gp_fmodSystem);
	if (fmodResult != FMOD_OK)
		throw PfException(__LINE__, __FILE__, string("Impossible de fermer FMOD : ") + FMOD_ErrorString(fmodResult));

	fmodResult = FMOD_System_Release(gp_fmodSystem);
	if (fmodResult != FMOD_OK)
		throw PfException(__LINE__, __FILE__, string("Impossible de fermer FMOD : ") + FMOD_ErrorString(fmodResult));
}

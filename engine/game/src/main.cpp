/**
* @file
* @author Anaïs Vernet
* @brief Fichier principal du projet <em>Poufalouf Game Engine</em>.
* @date 19/06/2016
*/

/**
* @mainpage
*
* <em>Poufalouf Game Engine</em> est un moteur de jeu destiné à la réalisation de jeux Poufalouf.
* Il s'agit d'un programme en C++ utilisant OpenGL couplé à la SDL et à FMOD.
*
* Ce programme utilise les bibliothèques PfMisc, PfMedia, PfMVC et PfGLGame.
*
* Pour une description des règles générales d'écriture du code et de la documentation, voir la documentation de la bibliothèque PfMisc.
*
* @section PointsDivers Quelques points d'attention
*
* @subsection PointIndiceTextureWad La gestion des indices de texture par les PfWad
*
* Les PfWad déterminent leurs indices de texture en utilisant le champ statique PfWad::s_maxTextureIndex.
* Les PfWad, se croyant seuls au monde, considèrent donc que rien d'autre qu'eux ne peut générer de textures.
* Or, c'est faux, la texture de la police par exemple est définie dans le fichier "gen.h".
* Il faut donc faire attention à donner des indices très élevés aux autres générateurs de textures potentiels, comme pour la police par exemple,
* car les PfWad démarrent à 0.
*/

#define SDL_MAIN_HANDLED // apparemment, sans ça, le main de SDL entre en conflit avec le mien

#include "gen.h"

#include <SDL.h>
#include <string>
#include <ctime>
#include <cstdlib>
#include <map>
#include <vector>

#include "errors.h"
#include "misc.h"
#include "glfunc.h"
#include "mediahandler.h"

#include "launcher.h"
#include "menumvcsystem.h"
#include "mapmvcsystem.h"
#include "mapeditormvcsystem.h"
#include "wad.h"
#include "datapackage.h"

/**
* @brief Lance une nouvelle partie.
* @throw PfException si une erreur survient.
*
* Le modèle de sélection des niveaux est affiché, puis le jeu est lancé.
*/
void newGame()
{
	PfWad* p_wad;
	MenuMVCSystem* p_menuMVCSystem;

	try
	{
		p_wad = new PfWad("PF_map_load_menu");
		p_menuMVCSystem = new MenuMVCSystem("map_load_menu", *p_wad);
		delete p_wad;

		if (p_menuMVCSystem->run()) // si vrai alors liste donc 2 valeurs contrôleur
		{
		    DataPackage dp = p_menuMVCSystem->values();
		    dp.nextInt(); // passer la 1° valeur
		    int val = dp.nextInt();
			if (dp.isOver())
				throw PfException(__LINE__, __FILE__, "Moins de 2 valeurs dans le contrôleur lors du choix du niveau.");
			vector<string> files_v = filesInDir(MAPS_DIR, MAPS_EXT);
			if (val > 0 && (unsigned int) val <= files_v.size())
			{
				HIDE_CURSOR();
				string mapName = files_v[val-1];
				while (mapName != "")
                {
                    MapMVCSystem* p_mapMVCSystem = new MapMVCSystem(mapName);
                    p_mapMVCSystem->run();
                    DataPackage dp = p_mapMVCSystem->values();
                    mapName = dp.nextString();
                    delete p_mapMVCSystem;
                }
				SHOW_CURSOR();
			}
		}
		delete p_menuMVCSystem;
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, "Erreur dans la fonction NEW GAME.", e);
	}
}

/**
* @brief Ouvre l'éditeur avec une nouvelle map.
* @throw PfException si une erreur survient.
*
* Le modèle de sélection des propriétés de la map est affiché, puis la map est chargée.
*/
void editorNewMap()
{
	PfWad* p_wad;
	MenuMVCSystem* p_menuMVCSystem;

	try
	{
		p_wad = new PfWad("PF_map_editor_menu");
		p_menuMVCSystem = new MenuMVCSystem("map_editor_menu", *p_wad);
		delete p_wad;

		if (p_menuMVCSystem->run()) // si vrai alors 3 valeurs contrôleur
		{
		    DataPackage dp = p_menuMVCSystem->values();

		    delete p_menuMVCSystem;
		    p_wad = new PfWad("PF_map_editor_load_menu");
            p_menuMVCSystem = new MenuMVCSystem("map_editor_wad_menu", *p_wad);
            delete p_wad;

		    if (p_menuMVCSystem->run()) // si vrai alors 2 valeurs contrôleur car liste
            {
                DataPackage dpWad = p_menuMVCSystem->values();
                dpWad.nextInt(); // passer la 1° valeur
                int val = dpWad.nextInt();
                if (dpWad.isOver())
                    throw PfException(__LINE__, __FILE__, "Moins de 2 valeurs dans le contrôleur lors du choix de wad pour une map.");
                vector<string> files_v = filesInDir(WAD_DIR, WAD_EXT, SYSTEM_FILE_PREFIX);
                if (val > 0 && (unsigned int) val <= files_v.size())
                {
                    p_wad = new PfWad(files_v[val-1].substr(0, files_v[val-1].find_last_of(".")), "launcher_load_menu", "PF_launcher_menu"); // détruit par le modèle
                    dp.nextInt(); // passer la 1° valeur
                    int val1 = dp.nextInt();
                    int val2 = dp.nextInt();
                    if (dp.isOver())
                        throw PfException(__LINE__, __FILE__, "Moins de 3 valeurs dans le contrôleur lors de la création d'une nouvelle map.");

                    MapEditorMVCSystem* p_mapEditorMVCSystem = new MapEditorMVCSystem(val1, val2, p_wad, "lake");
                    p_mapEditorMVCSystem->run();
                    delete p_mapEditorMVCSystem;
                }
            }
		}
		delete p_menuMVCSystem;
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, "Erreur dans la fonction EDITOR NEW MAP.", e);
	}
}

/**
* @brief Ouvre l'éditeur après sélection d'une map existante.
* @throw PfException si une erreur survient.
*
* Le modèle de sélection des maps est affiché, puis la map est chargée.
*/
void editorLoadMap()
{
	PfWad* p_wad;
	MenuMVCSystem* p_menuMVCSystem;

	try
	{
		p_wad = new PfWad("PF_map_load_menu");
		p_menuMVCSystem = new MenuMVCSystem("map_load_menu", *p_wad);
		delete p_wad;

		if (p_menuMVCSystem->run()) // si vrai alors liste donc 2 valeurs contrôleur
		{
		    DataPackage dp = p_menuMVCSystem->values();
		    dp.nextInt(); // passer la 1° valeur
		    int val = dp.nextInt();
			if (dp.isOver())
				throw PfException(__LINE__, __FILE__, "Moins de 2 valeurs dans le contrôleur lors de l'ouverture d'une map.");
			vector<string> files_v = filesInDir(MAPS_DIR, MAPS_EXT);
			if (val > 0 && (unsigned int) val <= files_v.size())
			{
				MapEditorMVCSystem* p_mapEditorMVCSystem = new MapEditorMVCSystem(files_v[val-1]);
				p_mapEditorMVCSystem->run();
				delete p_mapEditorMVCSystem;
			}
		}
		delete p_menuMVCSystem;
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, "Erreur dans la fonction EDITOR LOAD MAP.", e);
	}
}

/**
* @brief Boucle principale de jeu.
* @throw PfException si une erreur survient dans la boucle.
*/
void mainLoop()
{
	PfWad* p_wad = 0;
	MenuMVCSystem* p_menuMVCSystem = 0;

	try
	{
		p_wad = new PfWad("PF_main_menu", "launcher_load_menu", "PF_launcher_menu"); // chargement avec barre de progression
		p_menuMVCSystem = new MenuMVCSystem("main_menu", *p_wad);
		delete p_wad;

		bool cnt = true;

		while (cnt)
		{
			cnt = p_menuMVCSystem->run();
			if (cnt)
			{
			    DataPackage dp = p_menuMVCSystem->values();
			    int val = dp.nextInt();
				if (dp.isOver())
					throw PfException(__LINE__, __FILE__, "Aucune valeur dans le contrôleur du menu principal lors du choix.");
				switch (val)
				{
					case 1:
						newGame();
						break;
					case 2:
						editorNewMap();
						break;
					case 3:
						editorLoadMap();
						break;
					default:
						break;
				}
			}
		}
		delete p_menuMVCSystem;
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, "Erreur dans la boucle de jeu principale.", e);
	}
}

/**
* @brief Fonction principale du projet <em>Poufalouf Game Engine</em>.
* @return 1 si une erreur survient, 0 sinon.
*
* La fonction principale réalise les actions suivantes :
* <ul><li>initialisation du fichier LOG (macro du fichier "gen.h"),</li>
* <li>lecture du fichier de configuration (fonction <em>readConfig</em> du fichier "misc.h"),</li>
* <li>initialisation de SDL, FMOD et OpenGL, (fonction <em>initEverything</em> de la bibliothèque PfMedia),</li>
* <li>exécution du launcher (fonction <em>executeLauncher</em> du fichier "launcher.h") et fermeture éventuelle du programme,</li>
* <li>lancement de la boucle de jeu avec gestion des différents modèles (fonction <em>mainLoop</em>).</li></ul>
*/
int main(int argc, char* argv[])
{
	LOG(__DATE__ << " " << __TIME__ << "\nPOUFALOUF GAME ENGINE log\n\n");

	srand(time(0));
	rand();

	try
	{
		readConfig();
		initEverything("Poufalouf Game Engine");

		addTexture(FONT_DEFAULT, FONT_TEXTURE_INDEX);
		addTexture(FONT_DEFAULT_2, FONT_TEXTURE_INDEX_2);

		if (executeLauncher("PF_launcher_menu", "launcher_menu"))
			mainLoop();
	}
	catch (PfException& e)
	{
		LOG("Une erreur est survenue:\n" << e.what() << "\n");
		try
		{
			closeEverything();
		}
		catch (PfException& e2)
		{
			LOG("Erreur à la fermeture : " << e2.what() << "\n");
			#ifndef NDEBUG
			if (g_log.is_open())
				g_log.close();
			#endif
		}
		return 1;
	}

	try
	{
		closeEverything();
	}
	catch (PfException& e)
	{
		LOG("Erreur à la fermeture : " << e.what() << "\n");
		#ifndef NDEBUG
		if (g_log.is_open())
			g_log.close();
		#endif
		return 1;
	}

	return 0;
}


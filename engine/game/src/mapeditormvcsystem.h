/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe MapEditorMVCSystem.
* @date 17/06/2015
*/

#ifndef MAPEDITORMVCSYSTEM_H_INCLUDED
#define MAPEDITORMVCSYSTEM_H_INCLUDED

#include "gen.h"
#include "mvcsystem.h"
#include "wad.h"

/**
* @brief Système MVC dédié à la gestion d'un éditeur de map vue de haut.
*/
class MapEditorMVCSystem : public MVCSystem
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur MapEditorMVCSystem 1.
		* @param rows le nombre de lignes de la map.
		* @param columns le nombre de colonnes de la map.
		* @param p_wad le wad à utiliser.
		* @param texName le nom du fichier texture à utiliser.
		*/
		MapEditorMVCSystem(unsigned int rows, unsigned int columns, PfWad* p_wad, const string& texName);
		/**
		* @brief Constructeur MapEditorMVCSystem 2.
		* @param fileName le nom du fichier map à charger (sans l'extension).
		*/
		explicit MapEditorMVCSystem(const string& fileName);
};

#endif // MAPEDITORMVCSYSTEM_H_INCLUDED

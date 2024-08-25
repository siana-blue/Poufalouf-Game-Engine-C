/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe MapMVCSystem.
* @date 17/06/2015
*/

#ifndef MAPMVCSYSTEM_H_INCLUDED
#define MAPMVCSYSTEM_H_INCLUDED

#include "gen.h"
#include "mvcsystem.h"

/**
* @brief Système MVC dédié à la gestion d'une map 2D vue de haut.
*/
class MapMVCSystem : public MVCSystem
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur MapMVCSystem.
		* @param fileName le nom du fichier de la map à ouvrir.
		*/
		MapMVCSystem(const string& fileName);
};

#endif // MAPMVCSYSTEM_H_INCLUDED

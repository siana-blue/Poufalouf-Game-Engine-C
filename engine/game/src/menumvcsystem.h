/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe MenuMVCSystem.
* @date 17/06/2015
*/

#ifndef MAINMENUMVCSYSTEM_H_INCLUDED
#define MAINMENUMVCSYSTEM_H_INCLUDED

#include "gen.h"
#include <string>
#include "mvcsystem.h"

class PfWad;

/**
* @brief Système MVC dédié à la gestion d'un menu.
*/
class MenuMVCSystem : public MVCSystem
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur MenuMVCSystem.
		* @param name le nom du fichier menu à utiliser (sans extension et sans chemin).
		* @param r_wad le wad à utiliser.
		*/
		MenuMVCSystem(const string& name, PfWad& r_wad);
		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Ajoute un texte dynamique au modèle de ce système.
		* @param text le texte à ajouter.
		*/
		void addText(const string& text);
		/**
		* @brief Supprime les textes dynamiques du modèle de ce système.
		* @param update vrai si les widgets concernés doivent être mis à jour.
		*/
		void clearTexts(bool update = false);
		/**
		* @brief Ajoute une valeur dynamique au modèle de ce système.
		* @param value la valeur à ajouter.
		*/
		void addValue(int value);
		/**
		* @brief Supprime les valeurs dynamiques du modèle de ce système.
		* @param update vrai si les widgets concernés doivent être mis à jour.
		*/
		void clearValues(bool update = false);
};

#endif // MAINMENUMVCSYSTEM_H_INCLUDED


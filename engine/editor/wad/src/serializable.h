/**
* @file
* @author Poufalouf
* @brief Fichier contenant la classe Serializable.
* @date 08/02/2014
* @version 0.0
*/

#ifndef SERIALIZABLE_H_INCLUDED
#define SERIALIZABLE_H_INCLUDED

#include "gen.h"
#include "datapackage.h"

/**
* @brief Interface d'objet sérialisable.
*
* Un objet sérialisable dispose d'une méthode Serializable::saveData permettant sa sérialisation.
*
* La méthode Serializable::loadData permet d'utiliser les données sauvegardées pour mettre à jour cet objet.
*
* Certaines classes pourront définir un constructeur utilisant directement ces données et ne pas redéfinir Serializable::loadData.
*/
class Serializable
{
	public:
		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Enregistre le contenu de cet objet.
		* @param r_ofs le flux en écriture.
		*
		* Les données devraient être stockées de façon à pouvoir être affectées à un DataPackage (indication du type de valeur avant chaque valeur).
		*/
		virtual void saveData(ofstream& r_ofs) const = 0;
		/**
		* @brief Utilise les données sauvegardées.
		*
		* Par défaut, ne fait rien.
		*/
		virtual void loadData(ifstream&) {}
		/**
		* @brief Utilise les données sauvegardées.
		*
		* Par défaut, ne fait rien.
		*/
		virtual void loadData(DataPackage&) {}
};

#endif // SERIALIZABLE_H_INCLUDED

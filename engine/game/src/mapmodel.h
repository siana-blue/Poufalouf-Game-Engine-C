/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe MapModel.
* @date 26/06/2016
*/

#ifndef MAPMODEL_H_INCLUDED
#define MAPMODEL_H_INCLUDED

#include "gen.h"

#include <string>

#include "instructionreader.h"

#include "glmodel.h"
#include "map.h"
#include "enum.h"
#include "mapobject.h"
#include "wad.h"

/**
* @brief Modèle MVC dédié à la gestion d'une map vue de haut.
*/
class MapModel : public GLModel, public InstructionReader
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur MapModel.
		* @param fileName le nom du fichier de la map à ouvrir.
		* @throw ConstructorException si une erreur survient lors de la création de la map.
		*
		* Les textures de terrain sont ajoutées ici, aux indices TERRAIN_TEXTURE_INDEX et TERRAIN_TEXTURE_INDEX_2 (fichier "gen.h").
		*
		* La caméra est installée :
		* <ul><li>elle suit le mob contrôlé (nommé MOB_1),</li>
		* <li>ses limites sont fixées aux limites de la map.</li></ul>
		*/
		MapModel(const string& fileName);
		/**
		* @brief Destructeur MapModel.
		*/
		virtual ~MapModel() {}
		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Déplace les objets de cette map.
		* @throw PfException si un objet n'est pas trouvé lors des tests de collision.
		*
		* Les collisions sont testées, et les objets sont déplacés jusqu'à la première collision.
		* Cette méthode met à jour les plans de perspective des objets déplacés.
		* Les cases de la map se voient réaffecter les objets déplacés.
		*
		* Les actions suivantes sont réalisées sur chaque objet :
		* <ul><li>appel de la méthode MapModel::moveXY,</li>
		* <li>appel de la méthode MapModel::moveZ,</li>
		* <li>appel de la méthode MapModel::updateLayer,</li>
		* <li>mise à jour de la caméra selon le processus décrit au paragraphe suivant.</li></ul>
		*
		* Par défaut, la caméra est centrée sur le polygone du mob contrôlé :
		* <ul><li>si ce mob a l'état OBJSTAT_JUMPING, alors la caméra est décalée de façon à rester centrée sur sa projection au niveau du sol
		* au départ de son saut,</li>
		* <li>si ce mob a l'état OBJSTAT_LANDING, alors la caméra est à nouveau centrée sur lui avec une vitesse transitoire de 10%.</li></ul>
		*/
		void moveObjects();
		/**
		* @brief Réalise les actions dépendant des effets en cours.
		*/
		void applyEffects();
		/**
		* @brief Gère les intéractions entre objets via leurs zones.
		*
		* Les actions suivantes sont réalisées :
		* <ul><li>les objets dont la zone ACTION pénètrent la zone DOOR d'un autre objet sont supprimés de la carte ou, s'il s'agit du mob
		* contrôlé, entraînent le changement de map pour le joueur,</li>
		* <li>les objets activés dont la zone TRIGGER n'est plus pénétrée par aucune zone ACTION d'un autre objet sont désactivés,</li>
		* <li>les objets dont la zone TRIGGER est pénétrée par une zone ACTION d'un autre objet sont activés.</li></ul>
		*/
		void processInteractions();
		/**
		* @brief Indique si la map doit être quittée et de quelle façon.
		* @return un code indiquant si la map doit être quittée, basé sur l'énumération PfReturnCode (fichier "enum.h").
		* @throw PfException si le mob contrôlé ne peut pas être trouvé.
		*
		* Le code retourné est le suivant en fonction de la situation :
		* <ul><li>RETURN_NOTHING si la map ne doit pas être quittée,</li>
		* <li>RETURN_FAIL si la map doit être quittée du fait de l'échec du joueur,</li>
		* <li>RETURN_OK si la map doit être quittée pour en charger une autre, avec pour valeur associée ( & 255) le lien de map.</li></ul>
		*/
		pfflag32 endStatus() const;
		/**
		* @brief Retourne le nom de la map en lien à l'indice spécifié.
		* @param index l'indice spécifié, sur une liste commençant à 1.
		* @return le nom de la map liée à cet indice.
		*
		* Appelle la méthode Map::mapLink si l'indice est valide, retourne "" sinon.
		*/
		string mapLink(unsigned int index) const;
		/*
		* Redéfinitions
		* -------------
		*/
		/**
		* @brief Traite l'instruction passée en paramètre.
		* @return RETURN_OK si l'instruction a été traitée, RETURN_NOTHING sinon.
		* @throw PfException si l'exception ne peut pas être traitée.
		*
		* Les instructions sont transférées au mob contrôlé.
		* Si l'instruction est INSTRUCTION_CHECK, la valeur du type de sol Map::MapGroundType est envoyée en valeur complémentaire pour permettre au mob
		* de tester sa réaction à l'environnement.
		*/
		virtual PfReturnCode processInstruction();

	private:
		/**
		* @brief Déplace un objet horizontalement sur la map.
		* @param r_obj l'objet à déplacer.
		* @return le nombre de pas parcourus.
		*
		* Cette méthode est appelée par MapModel::moveObjects.
		*
		* Les actions suivantes sont réalisées :
		* <ul><li>si la vitesse de cet objet est nulle, alors toutes les étapes suivantes sont sautées,</li>
		* <li>le nombre de pas à parcourir par l'objet est déterminé à partir de sa vitesse et des collisions éventuelles avec la map,
		* ses bords ainsi que les autres objets, qui pourraient avoir lieu sur le chemin défini à l'étape précédente,</li>
		* <li>si le nombre de pas est différent de zéro, alors la méthode MapObject::move est appelée avec pour paramètre le nombre de pas,</li>
		* <li>si, après ce calcul, le nombre de pas à parcourir est différent de la vitesse de cet objet, alors l'instruction INSTRUCTION_STOP
		* est envoyée à cet objet.</li></ul>
		*/
		unsigned int moveXY(MapObject& r_obj);
		/**
		* @brief Déplace un objet verticalement sur la map.
		* @param r_obj l'objet à déplacer.
		* @param xySteps le nombre de pas parcourus en XY (nécessaire pour vérifier si des pentes ont été parcourues).
		* @return le nombre de pas Z parcourus, négatifs vers le bas.
		*
		* Cette méthode est appelée par MapModel::moveObjects.
		*
		* Les actions suivantes sont réalisées :
		* <ul><li>si l'objet a une vitesse Z non nulle, alors un déplacement Z est calculé pour la frame en cours,</li>
		* <li>si l'objet n'est pas au sol, alors son altitude est modifiée en fonction de sa vitesse Z et sa vitesse Z est affectée de la
		* constante MAP_GRAVITY (fichier "gen.h") pour la prochaine frame,</li>
		* <li>si l'objet n'est pas au sol mais que sa vitesse Z l'amène au sol avant déplacement complet, l'espace nécessaire pour le ramener au sol
		* est parcouru,</li>
		* <li>quand l'objet touche le sol, sa vitesse Z passe à 0, son statut démarque le flag OBJSTAT_JUMPING et marque le flag OBJSTAT_LANDING,</li>
		* <li>si l'objet est au sol est possède le flag OBJSTAT_LANDING depuis le tour précedent, alors ce flag est démarqué.</li></ul>
		*
		* Le déplacement Z de l'objet en une frame est calculé comme l'entier le plus proche de zéro de sa vitesse Z.
		* Une vitesse Z de -1.25 donne une déplacement de -1, tandis qu'une vitesse de 3.75 donne un déplacement de 3.
		*/
		int moveZ(MapObject& r_obj, unsigned int xySteps);
		/**
		* @brief Calcule le nombre de pas pouvant être effectués par un objet dans sa direction actuelle avant collision avec une case ou un autre objet.
		* @param rc_obj l'objet à tester.
		* @return le nombre de pas pouvant être réalisés.
		* @throw PfException si un objet n'est pas trouvé.
		*/
		unsigned int stepsBeforeCollision(const MapObject& rc_obj) const;
		/**
		* @brief Calcule le nombre de pas Z pouvant être effectués vers le bas par un objet avant collision avec une case.
		* @param rc_obj l'objet à tester.
		* @return le nombre de pas vers le bas pouvant être réalisés.
		*
		* Si, par un effet quelconque (bug très certainement), l'objet se trouve en-dessous d'une case, alors il tombera
		* jusqu'à 0, du moins vu par cette méthode.
		*/
		unsigned int zStepsBeforeCellCollision(const MapObject& rc_obj) const;
		/**
		* @brief Calcule le nombre de pas Z pouvant être effectués vers le haut par un objet avant collision avec les autres.
		* @param rc_obj l'objet à tester.
		* @return le nombre pas vers le haut pouvant être réalisés.
		* @throw PfException si un objet n'est pas trouvé.
		*/
		unsigned int zStepsBeforeObjectUpCollision(const MapObject& rc_obj) const;
		/**
		* @brief Calcule le nombre de pas Z pouvant être effectués vers le bas par un objet avant collision avec les autres.
		* @param rc_obj l'objet à tester.
		* @return le nombre pas vers le bas pouvant être réalisés.
		* @throw PfException si un objet n'est pas trouvé.
		*/
		unsigned int zStepsBeforeObjectDownCollision(const MapObject& rc_obj) const;
		/**
		* @brief Retourne l'altitude maximale du sol sous un objet (case + autres objets).
		* @param rc_obj l'objet à tester.
		* @return l'altitude maximale.
		* @throw PfException si un objet n'est pas trouvé.
		*
		* Si l'objet est en-dessous de l'altitude à laquelle il devrait être sur une case en pente, mais qu'il est
		* au-dessus de la base, alors la valeur retournée sera celle de la position à laquelle il devrait être.
		*
		* Si l'objet est sous une case, la valeur retournée est celle de la case.
		*/
		int maxZUnderObject(const MapObject& rc_obj) const;
		/**
		* @brief Met à jour le plan de perspective d'un objet en fonction de son déplacement.
		* @param r_obj l'objet à déplacer.
		*
		* Cette méthode est appelée par MapModel::moveObjects.
		*
		* Appelle la méthode statique Map::layerAt pour faire la mise à jour.
		*/
		void updateLayer(MapObject& r_obj);
		/**
		* @brief Méthode utilisée par les constructeurs pour créer les composants GUI.
		* @param p_wad le wad utilisé pour créer la map.
		* @throw PfException si une erreur survient lors de la création d'un objet.
		*/
		void createGUI(PfWad* p_wad);

		Map* mp_map; //!< La map de ce modèle.
		string m_controlledMobName; //!< Le nom du mob contrôlé.
		pfflag32 m_effects; //!< Les effets à prendre en compte.
		bool m_userActivation; //!< Indique si une activation par l'utilisateur est en cours.
};

#endif // MAPMODEL_H_INCLUDED

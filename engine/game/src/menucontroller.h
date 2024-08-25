/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe MenuController.
* @date 28/05/2016
*/

#ifndef MENUCONTROLLER_H_INCLUDED
#define MENUCONTROLLER_H_INCLUDED

#include "gen.h"
#include "glcontroller.h"

class MenuModel;

/**
* @brief Contrôleur MVC dédié à la gestion d'un menu sous OpenGL.
*
* Cette classe stocke des effets sous forme de flags correspondant à l'énumération PfEffect (fichier "enum.h").
*/
class MenuController : public GLController
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur MenuController.
		* @param r_model le modèle géré par ce contrôleur.
		*/
		explicit MenuController(MenuModel& r_model);
		/*
		* Redéfinitions
		* -------------
		*/
		/**
		* @brief Cycle de mise à jour du contrôleur : le modèle est mis à jour par appel à ses différentes méthodes.
		*
		* Les informations de l'EventHandler sont utilisées comme suit, sous forme notamment d'instructions au modèle :
		* <ul><li>Appui sur une touche directionnelle : instruction directionnelle correspondante envoyée au modèle,</li>
		* <li>appui sur ENTRE : instruction ACTIVATE,</li>
		* <li>appui sur BACKSPACE : instruction DELETE,</li>
		* <li>appui sur une touche de caractère : instruction MODIFY, code ASCII,</li>
		* <li>glissement de souris, si g_showCursor (fichier "gen.h") est vrai :
		* MenuModel::changeSelection aux coordonnées de la souris,</li>
		* <li>clic gauche, si g_showCursor est vrai :
		* MenuModel::changeSelection aux coordonnées de la souris ;
		* si le résultat est différent de RETURN_EMPTY (fichier "enum.h"),
		* instruction ACTIVATE.</li></ul>
		*
		* Puis, les actions suivantes sont réalisées:
		* <ul><li>La méthode GLController::update est appelée,</li>
		* <li>les effets du modèle sont traités.</li></ul>
		*
		* L'activation d'un effet se traduit par :
		* <ul><li>Le lancement d'un fondu de transition, sauf pour les effets EFFECT_SWITCH et EFFECT_SLEEP directement activés,</li>
		* <li>pour les autres effets, l'enregistrement des effets dans la variable MenuController::m_effects,</li>
		* <li>une fois le rendu de la transition terminé, s'il y a lieu, application de l'effet ;
		* MenuController::m_effects repasse à EFFECT_NONE.</li></ul>
		*
		* Ci-dessous la liste des effets :
		* <ul><li>EFFECT_NEXT : sélection du layout dont la valeur d'effet indique l'identifiant,</li>
		* <li>EFFECT_QUIT : passe le statut de ce contrôleur à DEAD,</li>
		* <li>EFFECT_NEW ou EFFECT_SELECT : passe le statut de ce contrôleur à ASLEEP,</li>
		* <li>EFFECT_SWITCH (sans transition) : passe le statut de ce contrôleur à ASLEEP,</li>
		* <li>EFFECT_SLEEP (sans transition) : émet l'instruction INSTRUCTION_STOP au modèle (désactive les widgets).</li></ul>
		*/
		virtual void update();
		/**
		* @brief Indique si ce contrôleur accepte les entrées utilisateurs.
		* @return vrai si les entrées utilisateur sont admises.
		*
		* Retourne vrai sauf si MenuController::m_effects est différent de EFFECT_NONE.
		*/
		virtual bool isAvailable() const;
		/*
		* Accesseurs
		* ----------
		*/
		pfflag32 getEffects() const {return m_effects;}

	private:
		pfflag32 m_effects; //!< Les effets à appliquer.
};

#endif // MENUCONTROLLER_H_INCLUDED


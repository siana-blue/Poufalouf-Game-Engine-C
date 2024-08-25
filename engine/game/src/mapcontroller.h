/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe MapController.
* @date 19/06/2016
*/

#ifndef MAPCONTROLLER_H_INCLUDED
#define MAPCONTROLLER_H_INCLUDED

#include "gen.h"
#include <vector>
#include "glcontroller.h"
#include "enum.h"

class MapModel;

/**
* @brief Contrôleur MVC dédié à la gestion d'une map 2D vue de haut.
*/
class MapController : public GLController
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur MapController.
		* @param r_model le modèle géré par ce contrôleur.
		*/
		explicit MapController(MapModel& r_model);
		/*
		* Redéfinitions
		* -------------
		*/
		/**
		* @brief Cycle de mise à jour du contrôleur : le modèle est mis à jour par appel à ses différentes méthodes.
		*
		* Tout d'abord, l'instruction INSTRUCTION_CHECK est envoyée au modèle, afin qu'il vérifie que la map ne doive pas être quittée.
		* Puis, la méthode MapModel::endStatus est appelée.
		* Si son code de retour est RETURN_FAIL (fichier "enum.h"), alors le statut de ce système MVC passe à DEAD.
		*
		* Si le statut n'est pas DEAD, alors
		* les méthodes MapModel::moveObjects et MapModel::processInteractions sont ensuite appelées, puis
		* les informations de l'EventHandler sont ensuite utilisées comme suit, sous forme notamment d'instructions au modèle :
		* <ul><li>touche directionnelle enfoncée ou relâchée : la méthode MapController::arrowKeyInstruction est appelée pour déterminer la direction à prendre,</li>
		* <li>touche LALT enfoncée : instruction INSTRUCTION_JUMP,</li>
		* <li>touche LCTRL enfoncée : instruction INSTRUCTION_ACTIVATE, 1,</li>
		* <li>touche LCTRL relâchée : instruction INSTRUCTION_ACTIVATE, 0,</li>
		* <li>touche F1 : enfoncée, <em>g_debug</em> (fichier "gen.h") est vrai, faux sinon.</li></ul>
		*
		* Enfin, quelque soit le statut, les actions suivantes sont réalisées :
		* <ul><li>appel de la méthode GLController::update,</li>
		* <li>appel de la méthode GLModel::showDebugInfo.</li></ul>
		*/
		virtual void update();

	private:
		/**
		* @brief Retourne une instruction en fonction d'une valeur de touche fléchée.
		* @param arrowKey l'instruction correspondant à une orientation.
		* @param down vrai si la touche est enfoncée, faux pour une touche relâchée.
		*
		* Cette méthode gère la liste MapController::m_arrowKeys_v et retourne une instruction dépendant de l'ordre de celle-ci,
		* dans le cas d'une touche relâchée.
		*
		* L'instruction retournée peut être une orientation ou INSTRUCTION_STOP.
		*
		* @bug
		* Manifestement, trois touches directionnelles enfoncées simultanément ne sont pas gérées.
		* Ce n'est vraiment pas gênant. Cela vient soit de ce code, soit de SDL, je ne sais pas...
		*/
		PfInstruction arrowKeyInstruction(PfInstruction arrowKey, bool down);

		vector<PfInstruction> m_arrowKeys_v; //!< La liste des touches fléchées enfoncées.
};

#endif // MAPCONTROLLER_H_INCLUDED


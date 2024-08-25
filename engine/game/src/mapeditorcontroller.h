/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe MapEditorController.
* @date 28/05/2016
*/

#ifndef MAPEDITORCONTROLLER_H_INCLUDED
#define MAPEDITORCONTROLLER_H_INCLUDED

#include "gen.h"
#include "glcontroller.h"

class MapEditorModel;

/**
* @brief Contrôleur MVC dédié à la gestion d'un éditeur de map vue de haut.
*/
class MapEditorController : public GLController
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur MapEditorController.
		* @param r_model le modèle géré par ce contrôleur.
		*/
		explicit MapEditorController(MapEditorModel& r_model);
		/*
		* Redéfinitions
		* -------------
		*/
		/**
		* @brief Cycle de mise à jour du contrôleur : le modèle est mis à jour par appel à ses différentes méthodes.
		*
		* Les informations de l'EventHandler sont utilisées comme suit, sous forme notamment d'instructions au modèle :
		* <ul><li>appui sur une touche directionnelle : instruction directionnelle correspondante envoyée au modèle,</li>
		* <li>appui d'une touche +, - ou PageUp/PageDown : instruction MODIFY affectée d'une valeur traduisant ALT ou CTRL enfoncées,</li>
		* <li>appui sur F4 : instruction SWITCH, 0,</li>
		* <li>appui sur F9 : instruction SWITCH, 1,</li>
		* <li>appui sur o : instruction SWITCH, 2,</li>
		* <li>appui sur ENTREE : instruction VALIDATE,</li>
		* <li>appui sur BACKSPACE : instruction DELETE, 0,</li>
		* <li>appui sur SUPPR : instruction DELETE, 1,</li>
		* <li>appui sur une touche caractère : instruction MODIFY accompagnée d'un DataPackage {Uint,1 ; Int,code ASCII},</li>
		* <li>clic gauche : instruction SELECT accompagnée d'un DataPackage "souris" (voir MapEditorModel::processInstruction),</li>
		* <li>relâchement souris gauche : instruction VALIDATE, 1,</li>
		* <li>glissement de souris : instruction SELECT, DataPackage "souris".</li></ul>
		*
		* Puis, les actions suivantes sont réalisées:
		* <ul><li>la méthode GLController::update est appelée,</li>
		* <li>la méthode MapEditorModel::playCurrentPhase est appelée,</li>
		* <li>la méthode MapEditorModel::applyEffects est appelée.</li></ul>
		*/
		virtual void update();
};

#endif // MAPEDITORCONTROLLER_H_INCLUDED


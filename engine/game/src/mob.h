/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe Mob.
* @date 03/03/2016
*/

#ifndef MOB_H_INCLUDED
#define MOB_H_INCLUDED

#include "gen.h"
#include <string>
#include "mapobject.h"

/**
* @brief Personnage ou entité animée du jeu, sur une map.
*/
class Mob : public MapObject
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur Mob.
		* @param name le nom de ce mob.
		* @param rect le rectangle des coordonnées de cet objet.
		*/
		Mob(const string& name = "", const PfRectangle& rect = PfRectangle());
		/*
		* Redéfinitions
		* -------------
		*/
		/**
		* @brief Met à jour ce ModelItem.
		*
		* Les actions suivantes sont réalisées :
		* <ul><li>appel de la méthode MapObject::update,</li>
		* <li>si cet objet a l'état STANDBY, passer sa vitesse à 0.</li></ul>
		*/
		virtual void update();
		/**
		* @brief Traite l'instruction passée en paramètre.
		* @return RETURN_OK si l'instruction a été traitée, RETURN_NOTHING sinon.
		* @throw PfException si l'instruction ne peut pas être traitée.
		*
		* Si cet objet a l'état STANDBY, alors rien n'est fait.
		*
		* Sinon, les instructions sont traitées comme suit :
		* <ul><li>instruction directionnelle :
		* changement d'orientation correspondant et passage à une vitesse de 2,</li>
		* <li>STOP : passage à une vitesse nulle,</li>
		* <li>JUMP : si cet objet n'a pas l'état ANIM_JUMPING,
		* alors prendre une vitesse Z de 1.</li>
		* <li>CHECK : si la valeur associée est MAP_GROUND_WATER (fichier "map.h"), alors si l'altitude de ce mob est 0,
		* prendre l'état OBJSTAT_ONWATER et OBJSTAT_STANDBY si ce n'est pas déjà fait.</li></ul>
		*/
		virtual PfReturnCode processInstruction();

	protected:
		/*
		* Redéfinitions
		* -------------
		*/
		/**
		* @brief Met à jour les statuts de cet objet.
		*
		* Les actions suivantes sont effectuées :
		* <ul><li>mise à jour des états,</li>
		* <li>appel de la méthode MapObject::updateObjStat.</li></ul>
		*
		* La phase de mise à jour des états comprend les actions suivantes :
		* <ul><li>si ce mob a l'état ONWATER et n'a pas l'état STANDBY, alors prendre l'état DEAD,</li>
		* <li>si la vitesse Z de ce mob est supérieure à 0, alors prendre l'état ANIM_JUMPING,</li>
		* <li>si la vitesse Z de ce mob est inférieure à -2, alors perdre l'état ANIM_JUMPING.</li></ul>
		*/
		virtual void updateObjStat();
		/**
		* @brief Met à jour l'animation de cet objet.
		*
		* Les actions suivantes sont réalisées :
		* <ul><li>appel la méthode MapObject::updateAnim,</li>
		* <li>si l'animation ANIM_GETUP1, ANIM_GETUP2 ou ANIM_DROWNING vient de se terminer, perdre l'état OBJSTAT_STANDBY ; pour les animations
		* ANIM_GETUP1 et ANIM_GETUP2, passer également à l'animation ANIM_IDLE,</li>
		* <li>si cet objet a l'état OBJSTAT_STANDBY, quitter cette méthode,</li>
		* <li>si la vitesse Z de ce mob est inférieure à -2, passer à l'animation ANIM_FALLING2,</li>
		* <li>sinon si cet objet a l'état OBJSTAT_JUMPING, passer à l'animation ANIM_JUMPING,</li>
		* <li>sinon si cet objet a l'état OBJSTAT_FALLING, passer à l'animation ANIM_FALLING1,</li>
		* <li>sinon si cet objet a l'état OBJSTAT_LANDING, passer à l'animation ANIM_GETUP1 depuis ANIM_FALLING1, ANIM_GETUP2 depuis ANIM_FALLING2 ou ANIM_IDLE par défaut ;
		* pour les animations ANIM_GETUP1 et ANIM_GETUP2, l'état STANDBY est pris,</li>
		* <li>sinon si la vitesse de cet objet est non nulle, passer à l'animation ANIM_WALKING,</li>
		* <li>sinon, passer à l'animation ANIM_IDLE, sauf si l'animation en cours est une animation à ne pas interrompre.</li></ul>
		*/
		virtual void updateAnim();
};

#endif // MOB_H_INCLUDED

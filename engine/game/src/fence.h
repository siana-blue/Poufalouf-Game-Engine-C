/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe Fence.
* @date 24/04/2016
*/

#ifndef FENCE_H_INCLUDED
#define FENCE_H_INCLUDED

#include "gen.h"
#include <string>
#include "mapobject.h"
#include "geometry.h"

/**
* @brief Objet Fence, représentant une barrière ou clôture.
*
* Cet objet placé seul sur la map aura la forme d'un piquet.
*
* La forme de cet objet varie ensuite en fonction de ses voisins, pour relier deux objets Fence contigus.
*
* Chaque fichier image est dédié à une animation. Ses dimensions sont de 4 cases horizontales et de 2 cases verticales.
* Le piquet se trouve dans le coin supérieur gauche de l'image, la barrière horizontale sur la même ligne et les deux cases suivantes,
* et enfin la barrière verticale occupe la dernière case de la ligne et la case du dessous.
*
* La zone MAIN définie dans le wad n'est pas très utile pour cet objet, car elle est constamment redéfinie par la méthode Fence::update
* en fonction de son code objet.
* Trois zones collision doivent être définies dans le wad, la première pour le piquet, la deuxième pour la barrière horizontale
* et la troisième pour la barrière verticale.
*
* Trois codes objets sont disponibles pour cet objet : 0 pour un simple piquet, 1 pour une barrière horizontale et 2 pour une barrières verticale.
* La méthode Fence::updateAnim tient compte de ce code pour sélectionner l'animation correspondante, à savoir ANIM_MISC1 pour l'horizontale et
* ANIM_MISC2 pour la verticale.
*/
class Fence : public MapObject
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur Fence.
    * @param name le nom de cet objet.
    * @param rect le rectangle des coordonnées de cet objet.
    */
    Fence(const string& name, const PfRectangle& rect);
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Met à jour ce ModelItem.
    *
    * Appelle la méthode MapObject::update.
    *
    * Puis, redimensionne le rectangle image en fonction du code objet :
    * <ul><li>0 : MAP_CELL_SIZE x MAP_CELL_SIZE (fichier "gen.h"),</li>
    * <li>1 : 2*MAP_CELL_SIZE x MAP_CELL_SIZE,</li>
    * <li>2 : MAP_CELL_SIZE x 2*MAP_CELL_SIZE.</li></ul>
    *
    * La zone de collision est également redimensionnée, de sorte à relier deux piquets.
    * Pour cela, les box définies dans le wad sont utilisées, normalement au nombre de trois pour une barrière.
    */
    virtual void update();

protected:
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Met à jour l'animation de cet objet.
    *
    * Puis, change l'animation de cet objet selon son code objet :
    * <ul><li>0 : ANIM_IDLE,</li>
    * <li>1 : ANIM_MISC1,</li>
    * <li>2 : ANIM_MISC2.</li></ul>
    */
    virtual void updateAnim();
};

#endif // FENCE_H_INCLUDED

/**
* @file
* @author Ana�s Vernet
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
* @brief Objet Fence, repr�sentant une barri�re ou cl�ture.
*
* Cet objet plac� seul sur la map aura la forme d'un piquet.
*
* La forme de cet objet varie ensuite en fonction de ses voisins, pour relier deux objets Fence contigus.
*
* Chaque fichier image est d�di� � une animation. Ses dimensions sont de 4 cases horizontales et de 2 cases verticales.
* Le piquet se trouve dans le coin sup�rieur gauche de l'image, la barri�re horizontale sur la m�me ligne et les deux cases suivantes,
* et enfin la barri�re verticale occupe la derni�re case de la ligne et la case du dessous.
*
* La zone MAIN d�finie dans le wad n'est pas tr�s utile pour cet objet, car elle est constamment red�finie par la m�thode Fence::update
* en fonction de son code objet.
* Trois zones collision doivent �tre d�finies dans le wad, la premi�re pour le piquet, la deuxi�me pour la barri�re horizontale
* et la troisi�me pour la barri�re verticale.
*
* Trois codes objets sont disponibles pour cet objet : 0 pour un simple piquet, 1 pour une barri�re horizontale et 2 pour une barri�res verticale.
* La m�thode Fence::updateAnim tient compte de ce code pour s�lectionner l'animation correspondante, � savoir ANIM_MISC1 pour l'horizontale et
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
    * @param rect le rectangle des coordonn�es de cet objet.
    */
    Fence(const string& name, const PfRectangle& rect);
    /*
    * Red�finitions
    * -------------
    */
    /**
    * @brief Met � jour ce ModelItem.
    *
    * Appelle la m�thode MapObject::update.
    *
    * Puis, redimensionne le rectangle image en fonction du code objet :
    * <ul><li>0 : MAP_CELL_SIZE x MAP_CELL_SIZE (fichier "gen.h"),</li>
    * <li>1 : 2*MAP_CELL_SIZE x MAP_CELL_SIZE,</li>
    * <li>2 : MAP_CELL_SIZE x 2*MAP_CELL_SIZE.</li></ul>
    *
    * La zone de collision est �galement redimensionn�e, de sorte � relier deux piquets.
    * Pour cela, les box d�finies dans le wad sont utilis�es, normalement au nombre de trois pour une barri�re.
    */
    virtual void update();

protected:
    /*
    * Red�finitions
    * -------------
    */
    /**
    * @brief Met � jour l'animation de cet objet.
    *
    * Puis, change l'animation de cet objet selon son code objet :
    * <ul><li>0 : ANIM_IDLE,</li>
    * <li>1 : ANIM_MISC1,</li>
    * <li>2 : ANIM_MISC2.</li></ul>
    */
    virtual void updateAnim();
};

#endif // FENCE_H_INCLUDED

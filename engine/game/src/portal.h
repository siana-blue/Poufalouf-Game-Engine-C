/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe Portal
* @date 19/06/2016
*/

#ifndef PORTAL_H
#define PORTAL_H

#include "gen.h"
#include "mapobject.h"

class Portal : public MapObject
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur Portal.
    * @param name le nom de cet objet.
    * @param rect le rectangle de coordonnées de cet objet.
    * @param orientation l'orientation de cet objet.
    */
    Portal(const string& name = "", const PfRectangle& rect = PfRectangle(), PfOrientation::PfOrientationValue orientation = PfOrientation::NO_ORIENTATION);
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Active cet objet.
    * @param code un code d'activation PfActivationCode indiquant le type d'activation effectuée. Ce paramètre n'est pas utilisé par la classe Portal.
    * @return EFFECT_NONE.
    *
    * Appelle la méthode MapObject::activate.
    *
    * Inhibe la zone de collision et rend cet objet flottant.
    */
    virtual pfflag32 activate(pfflag32 code = ACTIVCODE_ANY);
    /**
    * @brief Désactive cet objet.
    * @return EFFECT_NONE.
    *
    * Appelle la méthode MapObject::deactivate.
    *
    * Désinhibe la zone de collision et retire le caractère flottant de cet objet.
    */
    virtual pfflag32 deactivate();
};

#endif // PORTAL_H

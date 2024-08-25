/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe Fence.
* @date 19/06/2016
*/

#ifndef SIGN_H_INCLUDED
#define SIGN_H_INCLUDED

#include "gen.h"
#include <string>
#include "mapobject.h"
#include "geometry.h"

/**
* @brief Pancarte, panneau affichant une boîte de texte sur activation.
*/
class Sign : public MapObject
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur Sign.
    * @param name le nom de cet objet.
    * @param rect le rectangle des coordonnées de cet objet.
    */
    Sign(const string& name, const PfRectangle& rect);

    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Active cet objet.
    * @param code un code d'activation PfActivationCode indiquant le type d'activation effectuée.
    * @return EFFECT_PRINT.
    *
    * Si le code passé en paramètre ne contient pas le flag ACTIVCODE_USER, cet objet n'est pas activé.
    * De même, si ACTIVCODE_NORTH ou ACTIVCODE_SOUTH n'est pas trouvé, cet objet n'est pas activé.
    */
    virtual pfflag32 activate(pfflag32 code = ACTIVCODE_ANY);
    /**
    * @brief Désactive cet objet.
    * @return EFFECT_PRINT.
    */
    virtual pfflag32 deactivate();
};

#endif // SIGN_H_INCLUDED

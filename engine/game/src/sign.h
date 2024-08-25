/**
* @file
* @author Ana�s Vernet
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
* @brief Pancarte, panneau affichant une bo�te de texte sur activation.
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
    * @param rect le rectangle des coordonn�es de cet objet.
    */
    Sign(const string& name, const PfRectangle& rect);

    /*
    * Red�finitions
    * -------------
    */
    /**
    * @brief Active cet objet.
    * @param code un code d'activation PfActivationCode indiquant le type d'activation effectu�e.
    * @return EFFECT_PRINT.
    *
    * Si le code pass� en param�tre ne contient pas le flag ACTIVCODE_USER, cet objet n'est pas activ�.
    * De m�me, si ACTIVCODE_NORTH ou ACTIVCODE_SOUTH n'est pas trouv�, cet objet n'est pas activ�.
    */
    virtual pfflag32 activate(pfflag32 code = ACTIVCODE_ANY);
    /**
    * @brief D�sactive cet objet.
    * @return EFFECT_PRINT.
    */
    virtual pfflag32 deactivate();
};

#endif // SIGN_H_INCLUDED

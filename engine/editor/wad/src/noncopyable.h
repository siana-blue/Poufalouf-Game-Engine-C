/**
* @file
* @author Poufalouf
* @brief Contient la classe NonCopyable.
* @version 0.0
* @date 11/05/2013
*/

#ifndef NONCOPYABLE_H_INCLUDED
#define NONCOPYABLE_H_INCLUDED

#include "gen.h"

/**
* @brief Interface à utiliser pour interdire l'utilisation du constructeur de copie et de l'opérateur d'affectation pour une classe.
*
* Cette classe doit être héritée de manière privée par les classes non copiables.
*/
class NonCopyable
{
    protected:
        NonCopyable() {} //!< Constructeur nécessaire pour l'héritage.
        ~NonCopyable() {} //!< Destructeur nécessaire pour l'héritage.

    private:
        NonCopyable(const NonCopyable&); //!< Constructeur par copie interdit.
        NonCopyable& operator=(const NonCopyable&); //!< Opérateur interdit.
};

#endif // NONCOPYABLE_H_INCLUDED

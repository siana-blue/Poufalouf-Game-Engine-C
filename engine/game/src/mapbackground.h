/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe MapBackground.
* @date 19/12/2015
*/

#ifndef MAPWAD_BACKGROUND_H_INCLUDED
#define MAPWAD_BACKGROUND_H_INCLUDED

#include "gen.h"
#include <string>
#include "viewable.h"
#include "glitem.h"

/**
* @brief Décor de fond sur une map.
*/
class MapBackground : public AnimatedGLItem
{
    public:
        /*
        * Constructeurs et destructeur
        * ----------------------------
        */
        /**
        * @brief Constructeur MapBackground.
        * @param name le nom de cet objet.
        * @param mapRowsCount le nombre de lignes de la map à laquelle ce décor est lié.
        * @param mapColumnsCount le nombre de colonnes de la map à laquelle ce décor est lié.
        */
        MapBackground(const string& name, unsigned int mapRowsCount, unsigned int mapColumnsCount);

        /*
        * Redéfinitions
        * -------------
        */
        /**
        * @brief Génère un Viewable à partir de ce ModelItem.
        * @return le Viewable créé.
        *
        * Une Viewable différent est créé pour chaque case de la mosaïque que constitue le décor afin de n'avoir à afficher que ce qui est nécessaire.
        *
        * @warning
        * De la mémoire est allouée pour le pointeur retourné.
        */
        virtual Viewable* generateViewable() const;

    private:
        unsigned int m_rows; //!< le nombre de fois qu'il faut répéter le décor verticalement
        unsigned int m_columns; //!< le nombre de fois qu'il faut répéter le décor horizontalement
};

#endif // MAPWAD_BACKGROUND_H_INCLUDED

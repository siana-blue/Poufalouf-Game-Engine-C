/**
* @file
* @author Ana�s Vernet
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
* @brief D�cor de fond sur une map.
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
        * @param mapRowsCount le nombre de lignes de la map � laquelle ce d�cor est li�.
        * @param mapColumnsCount le nombre de colonnes de la map � laquelle ce d�cor est li�.
        */
        MapBackground(const string& name, unsigned int mapRowsCount, unsigned int mapColumnsCount);

        /*
        * Red�finitions
        * -------------
        */
        /**
        * @brief G�n�re un Viewable � partir de ce ModelItem.
        * @return le Viewable cr��.
        *
        * Une Viewable diff�rent est cr�� pour chaque case de la mosa�que que constitue le d�cor afin de n'avoir � afficher que ce qui est n�cessaire.
        *
        * @warning
        * De la m�moire est allou�e pour le pointeur retourn�.
        */
        virtual Viewable* generateViewable() const;

    private:
        unsigned int m_rows; //!< le nombre de fois qu'il faut r�p�ter le d�cor verticalement
        unsigned int m_columns; //!< le nombre de fois qu'il faut r�p�ter le d�cor horizontalement
};

#endif // MAPWAD_BACKGROUND_H_INCLUDED

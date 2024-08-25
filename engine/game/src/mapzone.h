/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe MapZone.
* @date 24/03/2016
*/

#ifndef MAPZONE_H
#define MAPZONE_H

#include "gen.h"
#include "geometry.h"

/**
* @brief Zone d'intéraction sur une map.
*
* Une zone est définie par un rectangle représentant sa géométrie horizontale, complétée par un entier spécifiant son altitude ("paroi basse")
* et un entier positif spécifiant sa hauteur d'effet.
*
* Les coordonnées du rectangle d'une zone sont des fractions de l'image principale de l'objet.
*/
class MapZone
{
    public:

        /*
        * Constructeurs et destructeur
        * ----------------------------
        */
        /**
        * @brief Constructeur MapZone.
        * @param rect le rectangle de cette zone.
        * @param z l'altitude de cette zone ("paroi basse").
        * @param height la hauteur de cette zone en pas Z d'une map.
        */
        MapZone(const PfRectangle& rect, int z, unsigned int height);

        /*
        * Méthodes
        * --------
        */
        /**
        * @brief Déplace cette zone sur le plan horizontal.
        * @param orientation l'orientation de déplacement.
        * @param scale l'échelle de déplacement.
        *
        * Appelle la méthode PfRectangle::shift sur le rectangle de cette zone.
        */
        void shift(const PfOrientation& orientation, double scale = 1);
        /*
        * Accesseurs
        * ----------
        */
        const PfRectangle& getRect() const {return m_rect;}
        void setRect(const PfRectangle& rect) {m_rect = rect;}
        int getZ() const {return m_z;}
        void setZ(int z) {m_z = z;}
        unsigned int getHeight() const {return m_height;}
        void setHeight(unsigned int height) {m_height = height;}
        bool isInhibited() const {return m_inhibited;}
        void setInhibited(bool inhibited) {m_inhibited = inhibited;}

    private:
        PfRectangle m_rect; //!< Le rectangle de cette zone.
        int m_z; //!< L'altitude de cette zone ("paroi basse").
        unsigned int m_height; //!< La hauteur de cette zone en pas Z d'une map.
        bool m_inhibited; //!< Indique si cette zone est inhibée.
};

#endif // MAPZONE_H

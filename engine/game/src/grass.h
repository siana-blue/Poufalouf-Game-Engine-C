/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe Jungle et ses éléments.
* @date 17/04/2016
*/

#ifndef GRASS_H_INCLUDED
#define GRASS_H_INCLUDED

#include "gen.h"
#include <string>
#include <vector>
#include <cstdlib>
#include "mapobject.h"
#include "geometry.h"
#include "viewable.h"

/**
* @brief Objet Grass, permettant l'affichage d'une étendue d'herbe au sol.
*
* Cet objet est composé de plusieurs rangées d'herbe.
*
* Pour déterminer le nombre d'images différentes pouvant représenter chaque rangée d'herbe,
* la première frame de l'animation ANIM_IDLE est utilisée.
* La case indiquée par le wad permet de découper le reste du fichier en lignes de hauteurs égales, et de piocher aléatoirement dans cette liste
* pour générer chaque rangée.
* Un fichier image doit donc être composé de lignes de tailles égales, chacune représentant un graphisme d'herbes.
* Chaque ligne doit être composée de trois cases, une pour la bordure gauche, l'autre pour la bordure droite.
* L'animation ANIM_IDLE est centrée sur la case du milieu.
*
* Il est possible de définir une configuration "bordures" pour cet objet, au moyen du code objet.
* Ce champ prend la forme d'un flag respectant l'énumération PfOrientation::PfOrientationValue.
* Une valeur de 0 indique un objet entier. Toute autre valeur indique qu'il ne s'agit que de bordures aux directions indiquées.
* Par exemple, si la valeur contient les bits EAST et SOUTH, alors l'objet est une bordure est et sud.
* Pour les coins, il faut utiliser les bits au-dessus de 16, dans le même ordre.
* Ainsi, pour (SOUTH | EAST) >> 4, on a un coin sud-est. Ceci peut bien entendu être combiné à des orientations simples.
* Tout ceci est géré lors du rendu dans la méthode Grass::generateViewable.
*/
class Grass : public MapObject
{
    public:
        /**
		* @brief Enumération des sections de sauvegarde d'un objet Grass.
		*/
		enum SaveGrass {SAVE_ROWS, //!< section de sauvegarde des lignes d'herbe (vecteur d'indices).
                        SAVE_END = SAVE_END_VALUE //!< fin de sauvegarde
                        };

        /*
        * Constructeurs et destructeur
        * ----------------------------
        */
        /**
        * @brief Constructeur Grass.
        * @param name le nom de cet objet.
        * @param rect le rectangle des coordonnées de cet objet.
        */
        Grass(const string& name, const PfRectangle& rect);

        /*
        * Méthodes
        * --------
        */
        /**
        * @brief Génère les images de cet objet, sous forme de rangées d'herbe.
        * @param rows le nombre de rangées d'herbe à générer.
        * @throw PfException si l'animation ANIM_IDLE n'est pas trouvée.
        *
        * Efface la liste Grass::m_rows_v et la remplit ensuite d'indices de cases de textures.
        * Ce vecteur, en fin de méthode, à la taille indiquée par le paramètre <em>rows</em>.
        */
        void generateElements(unsigned int rows);
        /**
        * @brief Génère les images de cet objet, sous forme de rangées d'herbe.
        * @param rc_grass l'objet Grass servant à initialiser celui-ci.
        *
        * En fin de méthode, le vecteur Grass::m_rows_v est égal à celui du paramètre.
        */
        void generateElements(const Grass& rc_grass);

        /*
        * Redéfinitions
        * -------------
        */
        /**
        * @brief Génère un Viewable à partir de ce ModelItem.
        * @return le Viewable créé.
        *
        * @warning
        * De la mémoire est allouée pour le pointeur retourné.
        */
        virtual Viewable* generateViewable() const;
        /**
		* @brief Sérialise cet objet.
		* @param r_ofs le flux en écriture.
		*
		* Le flux doit être positionné à l'endroit désiré pour l'écriture, il n'est pas rembobiné en fin de méthode.
		*
		* Les données sont stockées de façon à pouvoir être affectées à un DataPackage (indication du type de valeur avant chaque valeur).
		*
		* Cette méthode appelle MapObject::saveData puis sauvegarde ensuite les données liées aux lignes générées.
		*/
		virtual void saveData(ofstream& r_ofs) const;
		/**
		* @brief Utilise les données sauvegardées pour placer cet objet sur la map.
		* @param r_data les données.
		* @throw PfException si les données ne sont pas valides.
		*
		* Cette méthode appelle MapObject::loadData puis charge les données propres à l'objet Grass.
		*
		* @remarks
		* Cet objet est marqué modifié.
		*/
		virtual void loadData(DataPackage& r_data);

    private:
        vector<unsigned int> m_rows_v; //!< Les rangées d'herbe, chaque élément représentant une case de texture.
};

#endif // GRASS_H_INCLUDED

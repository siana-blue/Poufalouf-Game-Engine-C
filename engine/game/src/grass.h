/**
* @file
* @author Ana�s Vernet
* @brief Fichier contenant la classe Jungle et ses �l�ments.
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
* @brief Objet Grass, permettant l'affichage d'une �tendue d'herbe au sol.
*
* Cet objet est compos� de plusieurs rang�es d'herbe.
*
* Pour d�terminer le nombre d'images diff�rentes pouvant repr�senter chaque rang�e d'herbe,
* la premi�re frame de l'animation ANIM_IDLE est utilis�e.
* La case indiqu�e par le wad permet de d�couper le reste du fichier en lignes de hauteurs �gales, et de piocher al�atoirement dans cette liste
* pour g�n�rer chaque rang�e.
* Un fichier image doit donc �tre compos� de lignes de tailles �gales, chacune repr�sentant un graphisme d'herbes.
* Chaque ligne doit �tre compos�e de trois cases, une pour la bordure gauche, l'autre pour la bordure droite.
* L'animation ANIM_IDLE est centr�e sur la case du milieu.
*
* Il est possible de d�finir une configuration "bordures" pour cet objet, au moyen du code objet.
* Ce champ prend la forme d'un flag respectant l'�num�ration PfOrientation::PfOrientationValue.
* Une valeur de 0 indique un objet entier. Toute autre valeur indique qu'il ne s'agit que de bordures aux directions indiqu�es.
* Par exemple, si la valeur contient les bits EAST et SOUTH, alors l'objet est une bordure est et sud.
* Pour les coins, il faut utiliser les bits au-dessus de 16, dans le m�me ordre.
* Ainsi, pour (SOUTH | EAST) >> 4, on a un coin sud-est. Ceci peut bien entendu �tre combin� � des orientations simples.
* Tout ceci est g�r� lors du rendu dans la m�thode Grass::generateViewable.
*/
class Grass : public MapObject
{
    public:
        /**
		* @brief Enum�ration des sections de sauvegarde d'un objet Grass.
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
        * @param rect le rectangle des coordonn�es de cet objet.
        */
        Grass(const string& name, const PfRectangle& rect);

        /*
        * M�thodes
        * --------
        */
        /**
        * @brief G�n�re les images de cet objet, sous forme de rang�es d'herbe.
        * @param rows le nombre de rang�es d'herbe � g�n�rer.
        * @throw PfException si l'animation ANIM_IDLE n'est pas trouv�e.
        *
        * Efface la liste Grass::m_rows_v et la remplit ensuite d'indices de cases de textures.
        * Ce vecteur, en fin de m�thode, � la taille indiqu�e par le param�tre <em>rows</em>.
        */
        void generateElements(unsigned int rows);
        /**
        * @brief G�n�re les images de cet objet, sous forme de rang�es d'herbe.
        * @param rc_grass l'objet Grass servant � initialiser celui-ci.
        *
        * En fin de m�thode, le vecteur Grass::m_rows_v est �gal � celui du param�tre.
        */
        void generateElements(const Grass& rc_grass);

        /*
        * Red�finitions
        * -------------
        */
        /**
        * @brief G�n�re un Viewable � partir de ce ModelItem.
        * @return le Viewable cr��.
        *
        * @warning
        * De la m�moire est allou�e pour le pointeur retourn�.
        */
        virtual Viewable* generateViewable() const;
        /**
		* @brief S�rialise cet objet.
		* @param r_ofs le flux en �criture.
		*
		* Le flux doit �tre positionn� � l'endroit d�sir� pour l'�criture, il n'est pas rembobin� en fin de m�thode.
		*
		* Les donn�es sont stock�es de fa�on � pouvoir �tre affect�es � un DataPackage (indication du type de valeur avant chaque valeur).
		*
		* Cette m�thode appelle MapObject::saveData puis sauvegarde ensuite les donn�es li�es aux lignes g�n�r�es.
		*/
		virtual void saveData(ofstream& r_ofs) const;
		/**
		* @brief Utilise les donn�es sauvegard�es pour placer cet objet sur la map.
		* @param r_data les donn�es.
		* @throw PfException si les donn�es ne sont pas valides.
		*
		* Cette m�thode appelle MapObject::loadData puis charge les donn�es propres � l'objet Grass.
		*
		* @remarks
		* Cet objet est marqu� modifi�.
		*/
		virtual void loadData(DataPackage& r_data);

    private:
        vector<unsigned int> m_rows_v; //!< Les rang�es d'herbe, chaque �l�ment repr�sentant une case de texture.
};

#endif // GRASS_H_INCLUDED

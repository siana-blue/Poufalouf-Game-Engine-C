/**
* @file
* @author Ana�s Vernet
* @brief Fichier contenant la classe Jungle et ses �l�ments.
* @date 26/07/2016
*/

#ifndef JUNGLE_H_INCLUDED
#define JUNGLE_H_INCLUDED

#include "gen.h"
#include <vector>
#include "mapobject.h"
#include "viewable.h"

class PfRectangle;

/**
* @brief Structure repr�sentant un �l�ment affichable dans un objet Jungle.
*
* Un objet Jungle est forc�ment d�fini avec une animation ANIM_IDLE. Cette animation d�termine le nombre de lignes et de colonnes dans le fichier texture
* de l'objet Jungle (voir documentation de la classe Jungle). En fonction de cela, l'indice de texture d�termine quelle image choisir pour ce JungleElement.
* L'indice sup�rieur gauche est le num�ro 1, puis les indices s'encha�nent de gauche � droite de ligne en ligne.
*/
struct JungleElement
{
    /**
    * @brief Constructeur JungleElement.
    * @param r le rectangle des coordonn�es de cet �l�ment.
    * @param t l'indice de texture de cet �l�ment.
    */
    JungleElement(PfRectangle r, unsigned int t) : rect(r), textureIndex(t) {}

    PfRectangle rect; //!< Le rectangle des coordonn�es de cet �l�ment en fraction du rectangle de l'objet Jungle qui l'utilise.
    unsigned int textureIndex; //!< L'indice de texture de cet �l�ment.
};

/**
* @brief Objet Jungle, constitu� de diff�rent JungleElement pla��s al�atoirement par le constructeur.
*
* La gestion des frames d'une Jungle est particuli�re car les �l�ments sont g�n�r�s al�atoirement � partir du fichier image source.
*
* Pour une image statique : l'animation ANIM_IDLE (la seule actuellement utilis�e) sera constitu�e d'une seule frame d�finissant l'�l�ment du fichier image
* situ� dans le coin sup�rieur gauche. Tous les autres �l�ments al�atoires seront d�termin�s par le parcours du fichier image selon la taille du premier
* �l�ment d�fini par cette frame.
*
* Par exemple, si la frame d�termin�e dans le wad est 1;1;2;4, cela signifique que les diff�rents �l�ments de cette Jungle sont r�partis sur le fichier
* image sur deux lignes et quatre colonnes. La frame pointe sur l'�l�ment sup�rieur gauche et les autres seront parcourus selon cette taille.
*
* Pour une image anim�e, il sera donc ici n�cessaire d'avoir un fichier image diff�rent par frame.
*/
class Jungle : public MapObject
{
    public:
        /**
        * @brief Enum�ration des diff�rentes positions d'un �l�ment de jungle dans l'objet principal.
        */
        enum JungleElementPosition {JUNGLE_POS_ANYWHERE, //!< peut �tre plac� n'importe o�
                                    JUNGLE_POS_ALWAYS_FRONT, //!< ne peut �tre plac� qu'avec un Y �gal � l'Y de l'objet Jungle
                                    JUNGLE_POS_ALWAYS_BACK //!< ne peut �tre plac� qu'avec un Y+H �gal � l'Y+H de l'objet Jungle
                                    };
        /**
		* @brief Enum�ration des sections de sauvegarde d'un objet Grass.
		*/
		enum SaveJungle {SAVE_COUNT, //!< section de sauvegarde du nombre de JungleElement.
                         SAVE_RECTS, //!< section de sauvegarde des rectangles des �l�ments.
                         SAVE_INDEX, //!< section de sauvegarde des indices de texture des �l�ments
                         SAVE_END = SAVE_END_VALUE //!< fin de sauvegarde
                         };

        /*
        * Constructeurs et destructeur
        * ----------------------------
        */
        /**
        * @brief Constructeur Jungle.
        * @param name le nom de cet objet.
        * @param rect le rectangle des coordonn�es de cet objet.
        */
        Jungle(const string& name, const PfRectangle& rect);

        /*
        * M�thodes
        * --------
        */
        /**
        * @brief Cr�e des JungleElement pour cet objet Jungle.
        * @param minEleCount le nombre minimal d'�l�ments � g�n�rer.
        * @param maxEleCount le nombre maximal d'�l�ments � g�n�rer.
        * @param margin la marge servant au placement des �l�ments les uns par rapport aux autres.
        * @throw PfException si l'animation ANIM_IDLE n'est pas trouv�e ou n'a pas de frame.
        *
        * Cette m�thode est d�tach�e du constructeur car il faut une animation ANIM_IDLE pour d�terminer comment seront lus les indices de texture pour
        * g�n�rer chaque �l�ment.
        *
        * Si cette m�thode est appel�e une seconde fois, le vecteur de JungleElement est pr�alablement vid�.
        *
        * La marge entre objets est d�finie en pourcentage des dimensions de chaque �l�ment g�n�r�.
        * Si la marge est de 0,25, un autre �l�ment ne pourra pas �tre pos� de sorte que son centre soit plus proche de 0,25/2 du centre d'un autre �l�ment.
        */
        void generateJungleElements(int minEleCount, int maxEleCount, float margin);

        /*
        * Red�finitions
        * -------------
        */
        /**
        * @brief G�n�re un Viewable � partir de ce ModelItem.
        * @return le Viewable cr��.
        *
        * Une image est g�n�r�e pour chaque JungleElement de cet objet.
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
		* Cette m�thode appelle MapObject::saveData puis sauvegarde ensuite les donn�es li�es aux �l�ments g�n�r�s.
		*/
		virtual void saveData(ofstream& r_ofs) const;
		/**
		* @brief Utilise les donn�es sauvegard�es pour placer cet objet sur la map.
		* @param r_data les donn�es.
		* @throw PfException si les donn�es ne sont pas valides.
		*
		* Cette m�thode appelle MapObject::loadData puis charge les donn�es propres � l'objet Jungle.
		*
		* @remarks
		* Cet objet est marqu� modifi�.
		*/
		virtual void loadData(DataPackage& r_data);

    private:
        vector<JungleElement> m_elements_v; //!< La liste des �l�ments de cette jungle.
};

#endif // JUNGLE_H_INCLUDED

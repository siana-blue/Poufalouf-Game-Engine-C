/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe Jungle et ses éléments.
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
* @brief Structure représentant un élément affichable dans un objet Jungle.
*
* Un objet Jungle est forcément défini avec une animation ANIM_IDLE. Cette animation détermine le nombre de lignes et de colonnes dans le fichier texture
* de l'objet Jungle (voir documentation de la classe Jungle). En fonction de cela, l'indice de texture détermine quelle image choisir pour ce JungleElement.
* L'indice supérieur gauche est le numéro 1, puis les indices s'enchaînent de gauche à droite de ligne en ligne.
*/
struct JungleElement
{
    /**
    * @brief Constructeur JungleElement.
    * @param r le rectangle des coordonnées de cet élément.
    * @param t l'indice de texture de cet élément.
    */
    JungleElement(PfRectangle r, unsigned int t) : rect(r), textureIndex(t) {}

    PfRectangle rect; //!< Le rectangle des coordonnées de cet élément en fraction du rectangle de l'objet Jungle qui l'utilise.
    unsigned int textureIndex; //!< L'indice de texture de cet élément.
};

/**
* @brief Objet Jungle, constitué de différent JungleElement plaçés aléatoirement par le constructeur.
*
* La gestion des frames d'une Jungle est particulière car les éléments sont générés aléatoirement à partir du fichier image source.
*
* Pour une image statique : l'animation ANIM_IDLE (la seule actuellement utilisée) sera constituée d'une seule frame définissant l'élément du fichier image
* situé dans le coin supérieur gauche. Tous les autres éléments aléatoires seront déterminés par le parcours du fichier image selon la taille du premier
* élément défini par cette frame.
*
* Par exemple, si la frame déterminée dans le wad est 1;1;2;4, cela signifique que les différents éléments de cette Jungle sont répartis sur le fichier
* image sur deux lignes et quatre colonnes. La frame pointe sur l'élément supérieur gauche et les autres seront parcourus selon cette taille.
*
* Pour une image animée, il sera donc ici nécessaire d'avoir un fichier image différent par frame.
*/
class Jungle : public MapObject
{
    public:
        /**
        * @brief Enumération des différentes positions d'un élément de jungle dans l'objet principal.
        */
        enum JungleElementPosition {JUNGLE_POS_ANYWHERE, //!< peut être placé n'importe où
                                    JUNGLE_POS_ALWAYS_FRONT, //!< ne peut être placé qu'avec un Y égal à l'Y de l'objet Jungle
                                    JUNGLE_POS_ALWAYS_BACK //!< ne peut être placé qu'avec un Y+H égal à l'Y+H de l'objet Jungle
                                    };
        /**
		* @brief Enumération des sections de sauvegarde d'un objet Grass.
		*/
		enum SaveJungle {SAVE_COUNT, //!< section de sauvegarde du nombre de JungleElement.
                         SAVE_RECTS, //!< section de sauvegarde des rectangles des éléments.
                         SAVE_INDEX, //!< section de sauvegarde des indices de texture des éléments
                         SAVE_END = SAVE_END_VALUE //!< fin de sauvegarde
                         };

        /*
        * Constructeurs et destructeur
        * ----------------------------
        */
        /**
        * @brief Constructeur Jungle.
        * @param name le nom de cet objet.
        * @param rect le rectangle des coordonnées de cet objet.
        */
        Jungle(const string& name, const PfRectangle& rect);

        /*
        * Méthodes
        * --------
        */
        /**
        * @brief Crée des JungleElement pour cet objet Jungle.
        * @param minEleCount le nombre minimal d'éléments à générer.
        * @param maxEleCount le nombre maximal d'éléments à générer.
        * @param margin la marge servant au placement des éléments les uns par rapport aux autres.
        * @throw PfException si l'animation ANIM_IDLE n'est pas trouvée ou n'a pas de frame.
        *
        * Cette méthode est détachée du constructeur car il faut une animation ANIM_IDLE pour déterminer comment seront lus les indices de texture pour
        * générer chaque élément.
        *
        * Si cette méthode est appelée une seconde fois, le vecteur de JungleElement est préalablement vidé.
        *
        * La marge entre objets est définie en pourcentage des dimensions de chaque élément généré.
        * Si la marge est de 0,25, un autre élément ne pourra pas être posé de sorte que son centre soit plus proche de 0,25/2 du centre d'un autre élément.
        */
        void generateJungleElements(int minEleCount, int maxEleCount, float margin);

        /*
        * Redéfinitions
        * -------------
        */
        /**
        * @brief Génère un Viewable à partir de ce ModelItem.
        * @return le Viewable créé.
        *
        * Une image est générée pour chaque JungleElement de cet objet.
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
		* Cette méthode appelle MapObject::saveData puis sauvegarde ensuite les données liées aux éléments générés.
		*/
		virtual void saveData(ofstream& r_ofs) const;
		/**
		* @brief Utilise les données sauvegardées pour placer cet objet sur la map.
		* @param r_data les données.
		* @throw PfException si les données ne sont pas valides.
		*
		* Cette méthode appelle MapObject::loadData puis charge les données propres à l'objet Jungle.
		*
		* @remarks
		* Cet objet est marqué modifié.
		*/
		virtual void loadData(DataPackage& r_data);

    private:
        vector<JungleElement> m_elements_v; //!< La liste des éléments de cette jungle.
};

#endif // JUNGLE_H_INCLUDED

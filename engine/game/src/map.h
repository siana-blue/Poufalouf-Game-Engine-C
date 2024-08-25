/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe Map et les classes de certains objets constitutifs d'une map : Cell.
* @date 31/07/2016
*/

#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "gen.h"
#include <vector>
#include <map>
#include <string>
#include "glitem.h"
#include "serializable.h"
#include "datapackage.h"
#include "mapobject.h"
#include "textures.h"
#include "geometry.h"

class Viewable;

/**
* @brief Case, élément de terrain.
*
* Une case est repérée par la ligne et la colonne à laquelle elle se trouve dans la map.
* Sa texture est lue dans le fichier de terrain, l'indice de terrain indique la ligne et la colonne de la case à lire.
* Les indices s'enchaînent du haut à gauche vers le bas à droite, en commençant par 0, dans le fichier de texture terrain.
*
* Le polygone d'une case est un triangle dans le cas où la pente est cassée, l'autre triangle étant généré par la méthode Cell::generateViewable.
*
* @remarks
* Par rapport au fichier "textures.h", ici indice de terrain indique le numéro de case dans le fichier image...
* un jour j'harmoniserai peut-être le vocabulaire.
*
* Plusieurs fichiers terrains sont utilisés : le fichier terrain de base et un ou plusieurs terrains annexes
* Le terrain de base contient les textures principales des cases.
* Les terrains annexes contiennent les textures des falaises, des bordures etc...
* Seul l'indice du terrain de base est stocké en tant que membre d'une case.
* Les éléments de terrain annexe sont déterminés en fonction des attributs correspondants de cette case
* et du jeu de texture pointé par le champ Cell::mq_textureSet.
*
* Les données sauvegardées d'une case ne comprennent pas le jeu de textures,
* le but étant de pouvoir charger une map avec n'importe quel jeu de textures.
* Seuls les indices de terrain principal sont sauvegardés.
*
* La hauteur d'une case (coordonnée z) est un entier positif, 0 pour le vide.
* Une hauteur "carrée" est un multiple de MAP_CELL_SQUARE_HEIGHT (fichier "gen.h").
*
* Une case peut se voir affectée une pente. La pente est définie par une orientation représentée par un point cardinal PfOrientation::PfCardinalPoint
* ainsi que par une pair d'int.
* Le premier int indique la pente dans la direction spécifiée, le deuxième la pente dans la direction opposée.
* Cela permet de séparer la pente en deux triangles d'inclinaisons différentes.
*
* Une case ne gère pas son plan de perspective, c'est la map qui génère un Viewable composé de toutes les cases.
* La map génère ses Viewables dans l'ordre afin d'avoir les cases les plus au sud les plus en avant (gestion du relief).
*
* Le nom d'une case est "Case (r;c)" où r et c sont ses coordonnées.
*
* Une case connait ses voisines pour pouvoir générer l'ensemble de ses textures. C'est la map qui assigne ses voisines à chaque case.
*/
class Cell : public PolygonGLItem, public Serializable
{
	public:
		/**
		* @brief Enumération des sections de sauvegarde d'une case.
		*/
		enum SaveCell {SAVE_COORD, //!< section de sauvegarde des coordonnées
					   SAVE_TERRAIN, //!< section de sauvegarde de l'indice de terrain
					   SAVE_Z, //!< section de sauvegarde de l'altitude
					   SAVE_SLOPE, //!< section de sauvegarde des informations de pente
					   SAVE_END = SAVE_END_VALUE //!< fin de sauvegarde
					  };

		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur Cell 1.
		* @param row la ligne de cette case dans la map.
		* @param col la colonne de cette case dans la map.
		* @param terrainIndex l'indice de la case dans le fichier terrain.
		* @param rc_textureSet le jeu de textures à utiliser.
		* @param z la hauteur de cette case.
		*/
		Cell(unsigned int row, unsigned int col, unsigned int terrainIndex, const PfMapTextureSet& rc_textureSet, int z = MAP_CELL_SQUARE_HEIGHT);
		/**
		* @brief Constructeur Cell 2.
		* @param r_data les données pour construire cet objet.
		* @param rc_textureSet le jeu de textures à utiliser.
		* @throw ConstructorException si les données sont invalides.
		*
		* La variable globale g_maxSpreadingLayer prend la valeur trouvée dans le fichier si celle-ci est supérieure à sa valeur actuelle.
		*/
		Cell(DataPackage& r_data, const PfMapTextureSet& rc_textureSet);
		/**
		* @brief Destructeur Cell.
		*
		* Détruit le tableau des cases voisines, sans détruire les cases elles-mêmes.
		* Le jeu de textures lié n'est pas détruit non plus.
		*/
		~Cell();
		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Assigne une case voisine à cette case.
		* @param q_cell un pointeur constant vers la case voisine.
		* @param ori l'orientation de voisinage.
		*/
		void assignNeighbour(const Cell* q_cell, PfOrientation::PfCardinalPoint ori);
		/**
		* @brief Modifie les propriétés de cette case.
		* @param terrainIndex le nouvel indice de case de texture.
		* @param z la nouvelle hauteur.
		* @param spreadingLayer la priorité de recouvrement de cette case, négatif pour que cela soit géré automatiquement.
		*
		* Les propriétés de PolygonGLItem sont également mises à jour pour assurer une représentation adéquate de cette case après modification.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*/
		void modify(unsigned int terrainIndex, int z, int spreadingLayer = -1);
		/**
		* @brief Modifie la pente de cette case.
		* @param slopeOri la direction de modification de la pente.
		* @param deltaSlope la valeur à ajouter à la nouvelle pente actuelle.
		* @param forceOri indique si la valeur de pente doit être forcée si elle pouvait ne pas être modifiée.
		*
		* Si l'orientation passée en paramètre est égale à l'orientation actuelle de la pente de cette case,
		* ou s'il s'agit de l'orientation opposée à celle-ci,
		* alors la valeur de pente concernée est simplement modifiée sans changer la valeur Cell::m_slopeOri, à moins que <em>forceOri</em> ne soit vrai.
		*
		* Sinon, la valeur Cell::m_slopeOri est modifiée.
		*
		* Une pente dans une direction N,S,W,E n'a qu'une valeur de pente dans son orientation, la deuxième valeur de Cell::m_slopeValues est toujours nulle.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*/
		void changeSlope(PfOrientation::PfCardinalPoint slopeOri, int deltaSlope, bool forceOri = false);
		/**
		* @brief Retourne la hauteur entre cette case est la case voisine dans la direction spécifiée.
		* @param ori la direction à considérer.
		* @return la différence de hauteur, positive si cette case est plus haute que sa voisine.
		*
		* Retourne 0 si la case voisine n'existe pas.
		*/
		int cliffHeightWith(PfOrientation::PfCardinalPoint ori) const;
		/**
		* @brief Indique si la priorité de la texture de cette case en ce qui concerne les débordements est supérieure à celle d'une autre case.
		* @param rc_cell la case dont la priorité de texture est à comparer.
		* @return vrai si la priorité de la texture de cette case est supérieure strictement.
		*
		* La priorité est tout d'abord vérifiée dans le champ Cell::mq_textureSet.
		* Si les priorités définies dans ce jeu de textures sont les mêmes pour les textures des deux cases,
		* alors vrai est retourné si l'indice de case de texture de cette case est supérieur à celui de l'autre case.
		*/
		bool isSpreadingLayerHigher(const Cell& rc_cell) const;
		/**
		* @brief Retourne l'altitude minimale de cette case compte tenu de sa pente.
		* @return l'altitude minimale.
		*/
		int minZ() const;
		/**
		* @brief Retourne l'altitude maximale de cette case compte tenu de sa pente.
		* @return l'altitude maximale.
		*/
		int maxZ() const;
		/**
		* @brief Retourne l'altitude minimale de cette case dans un rectangle donné.
		* @param rect le rectangle à considérer.
		* @param z l'altitude pour correction.
		* @param roundUp vrai si l'arrondi en cas de fraction doit se faire à l'entier supérieur.
		*/
		int minZIn(const PfRectangle& rect, int z = MAP_CELL_SQUARE_HEIGHT, bool roundUp = false) const;
		/**
		* @brief Retourne l'altitude maximale de cette case dans un rectangle donné.
		* @param rect le rectangle à considérer.
		* @param z l'altitude pour correction.
		* @param roundUp vrai si l'arrondi en cas de fraction doit se faire à l'entier supérieur.
		*/
		int maxZIn(const PfRectangle& rect, int z = MAP_CELL_SQUARE_HEIGHT, bool roundUp = false) const;
		/**
		* @brief Retourne l'altitude en un point cardinal donné de cette case compte tenu de sa pente.
		* @param ori l'orientation de calcul d'altitude.
		* @param roundUp vrai si l'arrondi en cas de fraction doit se faire à l'entier supérieur.
		* @return l'altitude à l'orientation spécifiée.
		*/
		int zAt(PfOrientation::PfCardinalPoint ori, bool roundUp = false) const;
		/**
		* @brief Retourne l'altitude en un point donné de cette case compte tenu de sa pente.
		* @param xStep le nombre de pas en X (compris entre 0 et MAP_STEPS_PER_CELL-1).
		* @param yStep le nombre de pas en Y (compris entre 0 et MAP_STEPS_PER_CELL-1).
		* @param roundUp vrai si l'arrondi en cas de fraction doit se faire à l'entier supérieur.
		* @throw ArgumentException si le nombre de pas n'est pas valide.
		*
		* Par exemple, si les deux paramètres fournis valent MAP_STEPS_PER_CELL/2, et si MAP_STEPS_PER_CELL est pair,
		* alors l'altitude au centre de la case est retournée.
		*
		* Si <em>xStep</em> vaut 0 et <em>yStep</em> vaut MAP_STEPS_PER_CELL, l'altitude au point nord-ouest est retournée.
		*/
		int zAt(unsigned int xStep, unsigned int yStep, bool roundUp = false) const;
		/**
		* @brief Retourne l'altitude minimale parmi les directions voisines de l'orientation passée en paramètre.
		* @param ori l'orientation spécifiée.
		* @return l'altitude minimale.
		*
		* Par exemple, si <em>ori</em> est égal à PfOrientation::NORTH, les altitudes seront testées pour le nord-est, le nord et le nord-ouest.
		*/
		int minZAt(PfOrientation::PfCardinalPoint ori) const;
		/**
		* @brief Retourne l'altitude maximale parmi les directions voisines de l'orientation passée en paramètre.
		* @param ori l'orientation spécifiée.
		* @return l'altitude maximale.
		*
		* Par exemple, si <em>ori</em> est égal à PfOrientation::NORTH, les altitudes seront testées pour le nord-est, le nord et le nord-ouest.
		*/
		int maxZAt(PfOrientation::PfCardinalPoint ori) const;
		/**
		* @brief Construit un polygone représentant l'ensemble de la case.
		* @return le polygone.
		*
		* Utile dans le cas où PolygonGLItem::m_polygon est un triangle (cas des pentes).
		* Un rectangle complet est alors retourné.
		*/
		PfPolygon reconstructedPolygon() const;
		/**
		* @brief Indique si cette case est plate.
		* @return vrai si aucune des deux valeurs de pente n'est différente de 0.
		*/
		bool isFlat() const;
		/*
		* Redéfinitions
		* -------------
		*/
		/**
		* @brief Génère un Viewable à partir de ce ModelItem.
		* @return le Viewable créé.
		* @throw ViewableGenerationException si le Viewable ne peut être généré.
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
		* Les données sont stockées de façon à pouvoir être affectées à un DataPackage.
		*/
		virtual void saveData(ofstream& r_ofs) const;
		/*
		* Accesseurs
		* ----------
		*/
		unsigned int getRow() const {return m_row;}
		unsigned int getCol() const {return m_col;}
		unsigned int getTerrainIndex() const {return m_terrainIndex;}
		int getZ() const {return m_z;}

	private:
		unsigned int m_row; //!< La ligne où cette case se trouve dans la map.
		unsigned int m_col; //!< La colonne où cette case se trouve dans la map.
		unsigned int m_terrainIndex; //!< L'indice de la case du fichier terrain.
		int m_z; //!< La hauteur de cette case.
		PfOrientation::PfCardinalPoint m_slopeOri; //!< L'orientation de la pente de cette case.
		pair<int, int> m_slopeValues; //!< Les valeurs de la pente de cette case [direction Cell::m_slopeOri ; direction opposée].
		const PfMapTextureSet* mq_textureSet; //!< Le jeu de textures à utiliser.
		const Cell** mq_nextCells_t; //!< La liste des 8 voisines de cette case, dans l'ordre des points cardinaux retournés par PfOrientation::cardinalPoints.
};

/**
* @brief Map, ensemble des objets constitutifs d'une map 2D vue de haut.
*
* Une map est constituée de cases réparties en lignes et colonnes.
* La coordonnée (1;1) correspond à la case à l'extrême Sud-Ouest.
*
* A chaque fois qu'une case de la map est modifiée, c'est toute la map qui est reconstruite.
* Les cases ne doivent jamais être détruites et reconstruites, sous peine de perdre les liens entre cases voisines.
*
* Le nom d'une map est toujours MAP_NAME (fichier "gen.h").
*
* Chaque case a pour plan de perspective la valeur MAP_LAYER (fichier "gen.h") auquel s'ajoute le terme
* (MAP_MAX_LINES_COUNT-[la ligne de la case, partant de 1])*MAP_MAX_HEIGHT.
* Chaque objet ajouté à cette map ou déplacé prendra pour plan de perspective celui de la case la plus au sud sur laquelle il se trouve + 1.
* Le modèle pourra modifier ce plan de perspective pour placer deux objets l'un au-dessus d'un autre par exemple, sur la même case.
*
* Le niveau 0 d'une map peut se comporter différement d'un niveau à un autre. Il peut s'agir d'un simple sol (par défaut), mais également d'eau ou de vide.
* Le champ Map::m_groundType spécifie ce comportement au moyen de l'énumération Map::MapGroundType.
*
* Une map possède une liste de liens vers d'autres maps. Il s'agit d'une liste contenant les noms des autres maps. Pour réaliser les connexions,
* les objets les réalisant auront comme propriété le numéro correspondant à l'indice de la map dans cette liste, commençant à 1.
*/
class Map : public GLItem, public Serializable
{
	public:
		/**
		* @brief Enumération des sections de sauvegarde d'une map.
		*/
		enum SaveMap {SAVE_DIM, //!< section de sauvegarde des dimensions
					  SAVE_SEED, //!< section de sauvegarde de la graine de hasard
					  SAVE_TEXTURE, //!< section de sauvegarde du jeu de texture
					  SAVE_CELLS, //!< section de sauvegarde des cases
					  SAVE_GROUNDTYPE, //!< section de sauvegarde du comportement du niveau 0
					  SAVE_MAPLINKS, //!< section de sauvegarde des liens vers d'autres maps
					  SAVE_SCRIPT, //!< section de sauvegarde du script
					  SAVE_END = SAVE_END_VALUE //!< fin de sauvegarde
					  };
        /**
        * @brief Enumération des types de comportement du niveau 0 d'une map.
        */
        enum MapGroundType {MAP_GROUND_FLOOR, //!< niveau 0 identique à tout autre niveau
                            MAP_GROUND_WATER //!< niveau 0 correspondant à de l'eau (ou autre liquide)
                            };

        /**
		* @brief Retourne le plan de perspective associé à un objet.
		* @param rc_rect le rectangle de l'objet (collision, image ou autre).
		* @param z l'altitude de l'objet.
		* @return le plan de perspective à associer à l'objet.
		*/
		static int layerAt(const PfRectangle& rc_rect, int z);

		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur Map 1.
		* @param rows le nombre de lignes de cette map.
		* @param columns le nombre de colonnes de cette map.
		* @param texName le nom du fichier de textures de cette map.
		* @throw ConstructorException si les dimensions ne sont pas valides (nulles ou trop élevées).
		*
		* La graine de hasard de cette map est attribuée ici, aléatoirement.
		*/
		Map(unsigned int rows, unsigned int columns, const string& texName);
		/**
		* @brief Constructeur Map 2.
		* @param r_data les données pour construire cet objet.
		* @throw ConstructorException si les données sont invalides.
		*/
		explicit Map(DataPackage& r_data);
		/**
		* @brief Destructeur Map.
		*
		* Détruit les cases de cette map.
		*/
		~Map();
		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Retourne la case aux coordonnées spécifiées.
		* @param row la ligne de la case.
		* @param col la colonne de la case.
		* @return un pointeur constant vers la case.
		* @throw ArgumentException si les coordonnées ne sont pas valides.
		*/
		const Cell* cell(unsigned int row, unsigned int col) const;
		/**
		* @brief Retourne la case aux coordonnées spécifiées.
		* @param coord les coordonnées de la case.
		* @return un pointeur constant vers la case.
		* @throw ArgumentException si les coordonnées ne sont pas valides.
		*/
		const Cell* cell(pair<unsigned int, unsigned int> coord) const;
		/**
		* @brief Retourne la liste des coordonnées des cases contenues dans le rectangle passé en paramètre.
		* @param rect le rectangle à considérer.
		* @param z la hauteur du rectangle à considérer.
		*
		* Les cases partiellement contenues sont comptées.
		*
		* Une correction des coordonnées est effectuée en fonction de la hauteur <em>z</em> passée en paramètre.
		*
		* Le rectangle peut se situer, entièrement ou en partie, hors des limites de la map.
		* Seules les cases réellement existantes seront retournées.
		*/
		vector<pair<unsigned int, unsigned int> > cellsCoord(const PfRectangle& rect, int z);
		/**
		* @brief Retourne la case voisine de celle aux coordonnées spécifiées.
		* @param row la ligne de la case principale.
		* @param col la colonne de la case principale.
		* @param orientation l'orientation dans laquelle chercher la voisine.
		* @return la case voisine ou 0 si la case principale n'a pas de voisine dans l'orientation spécifiée.
		* @throw ArgumentException si les coordonnées de la case principale ne sont pas valides.
		*
		* L'orientation est une série de flags (8 bits).
		* Il est possible de demander la voisine dans la direction NORD, SUD mais également SUD-OUEST etc...
		* Demander l'orientation NORD_SUD ou OUEST_EST pointera vers la case principale (les mouvements s'annulent).
		*/
		Cell* nextCell(unsigned int row, unsigned int col, pfflag orientation);
		/**
		* @brief Retourne la case voisine de celle aux coordonnées spécifiées.
		* @param row la ligne de la case principale.
		* @param col la colonne de la case principale.
		* @param orientation l'orientation dans laquelle chercher la voisine.
		* @return la case voisine ou 0 si la case principale n'a pas de voisine dans l'orientation spécifiée.
		* @throw ArgumentException si les coordonnées de la case principale ne sont pas valides.
		*
		* L'orientation est une série de flags (8 bits).
		* Il est possible de demander la voisine dans la direction NORD, SUD mais également SUD-OUEST etc...
		* Demander l'orientation NORD_SUD ou OUEST_EST pointera vers la case principale (les mouvements s'annulent).
		*/
		const Cell* nextConstCell(unsigned int row, unsigned int col, pfflag orientation) const;
		/**
		* @brief Remplace une case par une autre.
		* @param row la ligne à laquelle modifier la case.
		* @param col la colonne à laquelle modifier la case.
		* @param terrainIndex l'indice de terrain de la nouvelle case.
		* @param z la hauteur de la nouvelle case.
		* @throw ArgumentException si les coordonnées ne sont pas valides.
		*
		* Si <em>z</em> est négatif, alors la hauteur de la case n'est pas modifiée.
		*
		* La case n'est pas vraiment remplacée, ce sont ses propriétés qui sont modifiées au moyen de la méthode Cell::modify.
		*
		* @remarks
		* Cette map est marquée modifiée.
		*/
		void changeCell(unsigned int row, unsigned int col, int terrainIndex, int z = -1);
		/**
		* @brief Modifie la pente de la case spécifiée.
		* @param row la ligne à laquelle modifier la case.
		* @param col la colonne à laquelle modifier la case.
		* @param slopeOri la nouvelle orientation de pente.
		* @param deltaSlope la valeur à ajouter à la pente actuelle.
		* @param forceOri indique si l'orientation de pente doit être forcée.
		* @throw ArgumentException si les coordonnées ne sont pas valides.
		*
		* Appelle Cell::changeSlope.
		*
		* @remarks
		* Cette map est marquée modifiée.
		*/
		void changeCellSlope(unsigned int row, unsigned int col, PfOrientation::PfCardinalPoint slopeOri, int deltaSlope, bool forceOri = false);
		/**
		* @brief Retourne la liste des noms des objets présents sur la case aux coordonnées spécifiées.
		* @param coord_pair les coordonnées de la case.
		* @return la liste de noms.
		* @throw ArgumentException si les coordonnées ne sont pas valides.
		*/
		vector<string> objectsOnCell(pair<unsigned int, unsigned int> coord_pair) const;
		/**
		* @brief Ajoute un objet aux coordonnées spécifiées.
		* @param r_object l'objet à placer.
		* @param row la ligne de la case.
		* @param col la colonne de la case
		* @throw ArgumentException si l'objet doit être placé à des coordonnées non valides.
		*
		* Si les coordonnées sont 0, alors l'objet est placé selon son rectangle.
		* Sinon, les coordonnées x et y de l'objet sont mises à jour, ainsi que sa hauteur.
		*
		* Le nom de l'objet est ajouté à chaque case recouverte.
		* L'objet est marqué modifié.
		*/
		void addObject(MapObject& r_object, unsigned int row = 0, unsigned int col = 0);
		/**
		* @brief Supprime les objets contenus dans un rectangle.
		* @param rect le rectangle contenant les objets.
		* @param z la hauteur du rectangle de suppression.
		*
		* @warning
		* Les objets ne sont pas réellement détruits, ils sont juste retirés de cette map.
		*/
		void removeObjects(const PfRectangle& rect, int z);
		/**
		* @brief Supprime les objets d'une case.
		* @param coord_pair les coordonnées de la case.
		*
		* @warning
		* Les objets ne sont pas réellement détruits, ils sont juste retirés de cette map.
		*/
		void removeObjects(const pair<unsigned int, unsigned int>& coord_pair);
		/**
		* @brief Supprime les objets dont les noms sont passés en paramètre.
		* @param names_v la liste des noms à supprimer.
		*
		* @remarks
		* La liste passée en paramètre n'est pas en référence car il est généralement probable que ce soit Map::m_objectsNames_v_t2 qui soit utilisé.
		* Comme les listes de cette variable sont modifiées, cela perturbe les itérateurs.
		*
		* @warning
		* Les objets ne sont pas réellement détruits, ils sont juste retirés de cette map.
		*/
		void removeObjects(const vector<string> names_v);
		/**
		* @brief Supprime l'objet dont le nom est passé en paramètre.
		* @param name le nom de l'objet.
		*
		* @warning
		* Les objets ne sont pas réellement détruits, ils sont juste retirés de cette map.
		*/
		void removeObject(const string& name);
		/**
		* @brief Met à jour la position d'un objet sur les cases de cette Map.
		* @param r_object l'objet.
		*
		* Met à jour les champs Map::m_objectsNames_v_t2 et Map::m_cellsUnderObjects_v_map.
		*/
		void updateObjectPosition(MapObject& r_object);
		/**
		* @brief Retourne le nombre de pas réalisables avant la première collision avec une case ou un bord de cette map, depuis une position spécifiée.
		* @param pathRect le rectangle de déplacement.
		* @param orientation l'orientation du déplacement.
		* @param z la hauteur à considérer.
		* @return le nombre de pas.
		*
		* Ce nombre est positif, mais un int signé est retourné, pour compatibilité avec la méthode GLItem::shift qui admet un nombre de pas négatif.
		*
		* Le rectangle de déplacement sert à déterminer le nombre de cases impliquées dans le calcul.
		* Sur un déplacement vertical par exemple, seule la coordonnée h ne sera pas utilisée car calculée indépendamment du rectangle.
		* La coordonnée y servira à déterminer le début du déplacement, et les coordonnées x et w au nombre de colonnes à prendre en compte.
		*
		* Le nombre de pas est calculé en considérant que l'objet peut passer au-dessus de toute case dont la hauteur est inférieure à <em>z</em> mais également
		* sur une case de hauteur supérieure si elle est reliée à la précédente par une pente douce.
		*/
		int stepsBeforeCollision(const PfRectangle& pathRect, PfOrientation::PfOrientationValue orientation, int z) const;
		/**
		* @brief Retourne le nombre de pas Z à parcourir pour atteindre une case voisine à une autre.
		* @param row la ligne de la case de départ.
		* @param col la colonne de la case de départ.
		* @param ori l'orientation vers la case voisine.
		* @return le nombre de pas Z, 0 si la case voisine est inexistante.
		* @throw ArgumentException si les coordonnées ne sont pas valides.
		*
		* Si la case voisine est plus haute, un nombre positif est retourné.
		* Si elle est plus basse, ce sera un nombre négatif.
		*
		* Permet de détecter la présence d'une falaise entre deux cases.
		*/
		int zStepsForNextCell(unsigned int row, unsigned int col, pfflag32 ori) const;
		/**
		* @brief Retourne le nombre de liens vers d'autres maps sur cette map.
		* @return le nombre de liens.
		*/
		unsigned int mapLinksCount() const;
		/**
		* @brief Retourne le nom de la map en lien à l'indice spécifié.
		* @param index l'indice spécifié, sur une liste commençant à 1.
		* @return le nom de la map liée à cet indice.
		* @throw ArgumentException si l'indice n'est pas valide.
		*/
        const string& mapLink(unsigned int index) const;
        /**
        * @brief Ajoute un lien de map à cette map.
        * @param mapName le nom de la map à lier.
        * @return l'index de la map ajoutée, commençant à 1.
        *
        * Si ce nom existe déjà, alors rien n'est fait.
        *
        * La valeur retournée sera donc la taille du vecteur si la map est bien ajoutée, ou sa position dans le vecteur (+1) si le lien existe déjà.
        */
        unsigned int addMapLink(const string& mapName);
        /**
        * @brief Supprime un lien de map.
        * @param index l'indice du lien à supprimer, en base 1.
        * @throw ArgumentException si l'indice n'est pas valide.
        */
        void removeMapLink(unsigned int index);
        /**
        * @brief Ajoute une entrée de script à cette map.
        * @param text l'entrée de script.
        */
        void addScriptEntry(const string& text);
        /**
        * @brief Modifie l'entrée de script à l'indice spécifié.
        * @param text le nouveau texte.
        * @param index l'indice de l'entrée de script à modifier.
        * @throw ArgumentException si l'indice n'est pas valide.
        */
        void editScriptEntry(const string& text, unsigned int index);
        /**
        * @brief Retourne l'entrée de script à l'indice spécifié.
        * @param index l'indice de l'entrée de script à retourner, en base 0.
        * @return le texte de l'entrée de script.
        * @throw ArgumentException si l'indice n'est pas valide.
        */
        const string& scriptEntry(unsigned int index) const;
        /**
        * @brief Retourne le nombre d'entrées de script de cette map.
        * @return le nombre d'entrées de script.
        */
        unsigned int scriptEntriesCount() const;
		/*
		* Redéfinitions
		* -------------
		*/
		/**
		* @brief Met à jour ce ModelItem.
		*
		* Passe le booléen ModelItem::m_modified à l'état faux.
		*/
		virtual void update();
		/**
		* @brief Génère un Viewable à partir de ce ModelItem.
		* @return le Viewable créé.
		* @throw ViewableGenerationException si la coordonnée Z d'une case est négative.
		*
		* Génère une image pour chaque case.
		* Les cases génèrent les images du Nord vers le Sud afin d'avoir les cases les plus au sud en avant (gestion du relief et des plans de perspective).
		* Pour chaque case, cette map ajoute des GLImage pour le relief et les bordures des cases.
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
		* Les données sont stockées de façon à pouvoir être affectées à un DataPackage.
		*/
		virtual void saveData(ofstream& r_ofs) const;
		/*
		* Accesseurs
		* ----------
		*/
		unsigned int getRowsCount() const {return m_rowsCount;}
		unsigned int getColumnsCount() const {return m_columnsCount;}
		unsigned int getSeed() const {return m_seed;}
		MapGroundType getGroundType() const {return m_groundType;}
		void setGroundType(MapGroundType type) {m_groundType = type;}

	private:
		/**
		* @brief Indique si une case est cachée par une case au sud.
		* @param row la ligne de la case concernée.
		* @param col la colonne de la case concernée.
		* @return vrai si la case n'est pas visible.
		* @throw ArgumentException si les coordonnées ne sont pas valides.
		*/
		bool isCellHidden(unsigned int row, unsigned int col) const;
		/**
		* @brief Indique si la falaise d'une case est visible.
		* @param row la ligne de la case concernée.
		* @param col la colonne de la case concernée.
		* @return vrai si la falaise est visible.
		* @throw ArgumentException si les coordonnées ne sont pas valides.
		*/
		bool isCliffVisible(unsigned int row, unsigned int col) const;
		/**
		* @brief Retourne la hauteur de falaise visible pour la case spécifiée.
		* @param row la ligne de la case concernée.
		* @param col la colonne de la case concernée.
		* @return la hauteur visible.
		* @throw ArgumentException si les coordonnées ne sont pas valides.
		*/
		int heightOfVisibleCliff(unsigned int row, unsigned int col) const;

		unsigned int m_rowsCount; //!< Le nombre de lignes de cette map.
		unsigned int m_columnsCount; //!< Le nombre de colonnes de cette map.
		unsigned int m_seed; //!< La graine de hasard (comme c'est poétique...).
		Cell*** mp_cells_t2; //!< Les cases de cette map.
		vector<string>** m_objectsNames_v_t2; //!< Les listes de noms d'objets par case de cette map.
		map<string, vector<pair<unsigned int, unsigned int> > > m_cellsUnderObjects_v_map; //!< La map des cellules contenant l'objet dont le nom est la clé (permet le parcours inverse par rapport à Map::m_objectsNames_v_t2).
		PfMapTextureSet m_textureSet; //!< Le jeu de textures de cette map.
		MapGroundType m_groundType; //!< Le comportement du niveau 0 de cette map.
		vector<string> m_mapLinks_v; //!< La liste des liens vers d'autres maps.
		vector<string> m_scriptEntries_v; //!< La liste des textes associés à cette map.
};

#endif // MAP_H_INCLUDED


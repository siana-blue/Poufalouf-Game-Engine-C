/**
* @file
* @author Ana�s Vernet
* @brief Fichier contenant la classe MapEditorModel.
* @date 19/06/2016
*/

#ifndef MAPEDITORMODEL_H_INCLUDED
#define MAPEDITORMODEL_H_INCLUDED

#include "gen.h"
#include <vector>
#include <string>
#include "graphics.h"
#include "map.h"
#include "wad.h"
#include "glmodel.h"
#include "multiphases.h"

/**
* @brief Mod�le MVC d�di� � la gestion d'un �diteur de map vue de haut.
*
* La s�lection de cases sur la map peut se faire case par case ou au moyen d'une s�lection rectangulaire.
* Pour r�aliser une s�lection rectangulaire, une case pivot est sp�cifi�e au moyen d'une ligne et d'une colonne,
* mises � jour � chaque s�lection d'une case unique.
*
* Les cases s�lectionn�es sont stock�es dans la liste MapEditorModel::m_selCoord_v, indiquant les cases s�lectionn�es.
* Une autre liste indique les cases en cours de s�lection non envore valid�e, par exemple lors d'une s�lection rectangulaire.
*
* Ce mod�le utilise un pointeur PfWad pour initialiser son champ MapEditorModel::mp_wad.
* Ce pointeur sera d�truit par le destructeur de cette classe.
*/
class MapEditorModel : public GLModel, public MultiPhases, public InstructionReader
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur MapEditorModel 1.
		* @param rows le nombre de lignes de la map.
		* @param columns le nombre de colonnes de la map.
		* @param p_wad le wad � utiliser.
		* @param texName le nom du fichier de textures de la map (sans l'extension).
		* @throw ConstructorException si une erreur survient lors de la construction de la map ou de l'interface graphique.
		*
		* Le wad sera d�truit par le destructeur de cette classe.
		*/
		MapEditorModel(unsigned int rows, unsigned int columns, PfWad* p_wad, const string& texName);
		/**
		* @brief Constructeur MapEditorModel 2.
		* @param fileName le nom du fichier map � charger (avec l'extension).
		* @throw ConstructorException si une erreur survient lors de la lecture du fichier ou la construction d'un objet.
		*
		* Le premier octet du fichier est la version PFGAME_VERSION (fichier "gen.h").
		* S'il n'y a pas correspondance, une exception est lev�e.
		*/
		explicit MapEditorModel(const string& fileName);
		/**
		* @brief Destructeur MapEditorModel.
		*
		* D�truit le wad.
		*/
		virtual ~MapEditorModel();
		/*
		* M�thodes
		* --------
		*/
		/**
		* @brief Applique les effets des widgets, en fonction de la phase actuelle.
		* @throw PfException si un objet n'est pas trouv�.
		*
		* La m�thode MapEditorModel::resetEffects est ensuite appel�e.
		*/
		void applyEffects();
		/**
		* @brief Affiche une petite ic�ne � c�t� de la souris repr�sentant le slot de wad en cours de s�lection.
		*
		* Si le slot de wad est WAD_NULL_OBJECT, rien n'est affich�.
		*/
		void displayMouseObjectIcon();
		/*
		* Red�finitions
		* -------------
		*/
		/**
		* @brief Traite l'instruction pass�e en param�tre.
		* @return RETURN_OK si l'instruction a �t� trait�e, RETURN_NOTHING sinon.
		* @throw PfException si une erreur survient.
		*
		* Certaines instructions doivent �tre accompagn�es d'un DataPackage "souris" structur� de la fa�on suivante,
		* l'ordre des char devant �tre respect�, 1 indiquant "oui" et 0 "non" :
		* <ul><li>deux float pour les coordonn�es d'un point (li� � la souris),</li>
		* <li>un char pour indiquer si ALT est enfonc�,</li>
		* <li>un char pour indiquer si CTRL est enfonc�,</li>
		* <li>un char pour indiquer si SHIFT est enfonc�,</li>
		* <li>un char pour indiquer s'il y a glissement de souris (si "non", alors il s'agit d'un clic),</li>
		* <li>un char pour indiquer si le bouton gauche est enfonc� pendant un glissement,</li>
		* <li>un char pour indiquer si le bouton droit est enfonc� pendant un glissement,</li>
		* <li>un char pour indiquer si l'action a lieu dans les bordures.</li></ul>
		*
		* Appelle la m�thode MapEditorModel::playCurrentPhase.
		*/
		virtual PfReturnCode processInstruction();
		/**
		* @brief R�alise les actions associ�es � la phase en cours.
		*/
		virtual void processCurrentPhase();
		/**
		* @brief R�alise les actions associ�es � la sortie de la phase en cours.
		*/
		virtual void exitCurrentPhase();
		/*
		* Accesseurs
		* ----------
		*/
		pfflag32 getWidgetEffects() const {return m_widgetEffects;}
		bool isTextureMode() const {return m_textureMode;}
		int getCurrentTerrainIndex() const {return m_currentTerrainIndex;}
		void setCurrentTerrainIndex(int index) {m_currentTerrainIndex = index;}

	private:
		/*
		* M�thodes
		* --------
		*/
		/**
		* @brief Met � jour la s�lection d'un layout de ce mod�le.
		* @param rc_point le point de s�lection.
		* @param name le nom du layout.
		* @param justHighlight vrai si la s�lection ne doit �tre qu'une emphase.
		* @return RETURN_EMPTY si aucun widget n'est sous la s�lection, RETURN_NOTHING si le widget est d�j� s�lecitonn�, ou RETURN_OK sinon.
		* @throw PfException si l'objet "layout" n'est pas trouv� ou si une erreur survient lors de la s�lection.
		*/
		PfReturnCode changeSelection(const PfPoint& rc_point, const string& name, bool justHighlight = false);
		/**
		* @brief Retourne les coordonn�es d'une case en fonction d'un point de l'�cran.
		* @param rc_point le point � consid�rer.
		* @return les coordonn�es correspondantes, (1;1) �tant la case inf�rieure gauche et (0;0) signifiant l'absence de case au point sp�cifi�.
		*
		* Cette m�thode prend en compte un point de l'�cran et fait ses calculs en consid�rant la position de la cam�ra.
		*
		* Si le point est dans une bordure, (0;0) est retourn�.
		*/
		pair<unsigned int, unsigned int> mapCoordAt(const PfPoint& rc_point) const;
		/**
		* @brief S�lectionne la case aux coordonn�es sp�cifi�es.
		* @param row la ligne de la case.
		* @param col la colonne de la case.
		*/
		void selectCell(unsigned int row, unsigned int col);
		/**
		* @brief S�lectionne la case aux coordonn�es sp�cifi�es.
		* @param coord_pair la paire de coordonn�es.
		*/
		void selectCell(pair<unsigned int, unsigned int> coord_pair);
		/**
		* @brief R�alise une s�lection rectangulaire de la case pivot jusqu'� la case dont les coordonn�es sont sp�cifi�es.
		* @param coord_pair les coordonn�es sp�cifi�es.
		*
		* Si la case pivot n'est pas valide, alors une s�lection unique est r�alis�e.
		*/
		void squareSelect(pair<unsigned int, unsigned int> coord_pair);
		/**
		* @brief D�s�lectionne l'ensemble des cases.
		*/
		void unselectAll();
		/**
		* @brief Change la s�lection de la liste dont le nom est pass� en param�tre.
		* @param name le nom de la liste.
		* @param rc_point le point de s�lection.
		* @param activate vrai si la liste doit �tre activ�e.
		* @return vrai si un bouton est activ�.
		* @throw PfException si la liste n'est pas trouvable.
		*/
		bool selectInList(const string& name, const PfPoint& rc_point, bool activate);
		/**
		* @brief Active ou d�sactive la bo�te de texte en fonction du point.
		* @param rc_point le point d'activation.
		* @throw PfException si la bo�te de texte n'est pas trouvable.
		*/
		void clickOnTextBox(const PfPoint& rc_point);
		/**
		* @brief Valide la s�lection temporaire.
		*
		* Les cases du tableau tampon sont transf�r�s dans le tableau de s�lection. Le tableau tampon est vid�.
		*
		* Cette m�thode doit typiquement �tre appel�e pour valider une s�lection rectangulaire.
		*/
		void validateBufferSelection();
		/**
		* @brief Met � jour la position des curseurs en fonction des cases s�lectionn�es.
		*
		* Pour chaque case de la map, si la case est s�lectionn�e,
		* alors un curseur est g�n�r� portant le nom "MapCursor(x;y)" o� x et y sont les coordonn�es de la case (en partant de (1;1) en bas � gauche).
		* Chaque curseur correspondant aux coordonn�es de la liste de suppression est supprim�.
		*
		* L'affichage du curseur prend en compte les caract�ristiques de la case, comme sa hauteur par exemple.
		*/
		void updateCursors();
		/**
		* @brief Modifie la topologie locale aux curseurs.
		* @param z la hauteur relative de modification.
		* @param radius le rayon de modification.
		* @param mode le mode de g�n�ration de la texture de topologie.
		* @param rel vrai si la valeur de hauteur est relative � la hauteur des cases.
		* @param behavior 0 : modification syst�matique, <0 : modification uniquement si la nouvelle hauteur est inf�rieure � l'ancienne, >0 : modification uniquement si la nouvelle hauteur est sup�rieure � l'ancienne.
		* @param minValue la hauteur minimale des cases.
		* @param slope la pente � utiliser.
		*/
		void changeTopology(int z, unsigned int radius, TextureGenerationMode mode, bool rel = false, int behavior = 0, unsigned int minValue = 4, unsigned int slope = 1);
		/**
		* @brief Modifie la hauteur des cases aux coordonn�es sp�cifi�es.
		* @param rc_cells_v la liste des cases concern�es.
		* @param z la hauteur des cases.
		* @param rel vrai si la valeur de hauteur est relative � la hauteur des cases.
		* @param behavior 0 : modification syst�matique, <0 : modification uniquement si la nouvelle hauteur est inf�rieure � l'ancienne, >0 : modification uniquement si la nouvelle hauteur est sup�rieure � l'ancienne.
		* @throw PfException si un probl�me survient lors de la mise � jour de la hauteur d'un objet.
		*
		* Les objets pr�sents sur la case sont retir�s de la map puis ajout�s � nouveau pour �tre mis � jour (en hauteur).
		*
		* Les cases sont limit�es en hauteur entre les valeurs 0 et MAP_MAX_HEIGHT.
		*/
		void changeCellsHeight(const vector<pair<unsigned int, unsigned int> >& rc_cells_v, int z, bool rel = false, int behavior = 0);
		/**
		* @brief Modifie la hauteur des cases s�lectionn�es.
		* @param z la hauteur des cases.
		* @param rel vrai si la valeur de hauteur est relative � la hauteur des cases.
		* @throw PfException si un probl�me survient lors de la mise � jour de la hauteur d'un objet.
		*
		* Les objets pr�sents sur la case sont retir�s de la map puis ajout�s � nouveau pour �tre mis � jour (en hauteur).
		*/
		void changeSelCellsHeight(int z, bool rel = false);
		/**
		* @brief Modifie l'indice de terrain des cases s�lectionn�es.
		* @param index le nouvel indice de terrain.
		*/
		void changeSelCellTerrain(int index);
		/**
		* @brief Modifie la pente des cases s�lectionn�es.
		* @param deltaSlope la variation de pente � appliquer.
		*
		* L'orientation est d�termin�e par le champ MapEditorModel::m_selOr.
		*
		* Si cette orientation est nulle, alors la hauteur de la case est simplement modifi�e.
		*/
		void changeSelCellSlope(int deltaSlope);
		/**
		* @brief Ex�cute une instruction par le layout.
		* @param instruction l'instruction � ex�cuter.
		* @param value la valeur associ�e.
		* @return RETURN_OK si l'instruction a �t� trait�e, RETURN_NOTHING sinon.
		* @throw PfException si le layout n'est pas trouv�.
		*
		* Si l'instruction est INSTRUCTION_ACTIVATE, alors le champ MapEditorModel::m_widgetEffects est mis � jour.
		*/
		PfReturnCode executeLayoutInstruction(PfInstruction instruction, int value = 0);
		/**
		* @brief Fait d�filer la grille de s�lection.
		* @param forward vrai si le d�filement est vers le bas ou la droite.
		* @param wadGrid vrai si la grille de wad doit d�filer, faux pour la grille de textures.
		* @param steps le nombre de pas.
		* @throw PfException si le layout n'est pas trouv�.
		*/
		void scrollGrid(bool forward, bool wadGrid, int steps = 1);
		/**
		* @brief Appelle la m�thode PfWidget::slide sur la s�lection.
		* @param layout le nom du layout concern�.
		* @throw PfException si l'objet "layout" n'est pas trouv� ou si une erreur survient lors de la s�lection.
		*/
		void slideSelection(const string& layout);
		/**
		* @brief Change le mode d'�dition.
		* @throw PfException si le layout n'est pas trouv�.
		*
		* Le bouton de changement de mode change de texte.
		*/
		void changeMode();
		/**
		* @brief Change le mode d'�dition de topologie.
		* @throw PfException si le layout n'est pas trouv�.
		*
		* Le changement se fait en fonction de la position du curseur de topologie.
		*/
		void changeTopoMode();
		/**
		* @brief Lit la valeur du rayon topologique.
		* @return le rayon topologique.
		* @throw PfException si le layout n'est pas trouv�.
		*
		* La valeur est lue directement dans la bo�te num�rique.
		*/
		unsigned int topoRadius();
		/**
		* @brief Lit la valeur de hauteur minimale pour l'�dition topologique.
		* @return la hauteur minimale.
		* @throw PfException si le layout n'est pas trouv�.
		*
		* La valeur est lue directement dans la bo�te num�rique.
		*/
		unsigned int minFloorValue();
		/**
		* @brief Lit la valeur de pente pour l'�dition topologique.
		* @return la pente.
		* @throw PfException si le layout n'est pas trouv�.
		*
		* La valeur est lue directement dans la bo�te num�rique.
		*/
		unsigned int topoSlope();
		/**
		* @brief S�lectionne l'objet au point sp�cifi�, s'il y en a un.
		* @param rc_point le point de s�lection.
		* @throw PfException si un objet n'est pas trouv�.
		*
		* Si deux objets sont superpos�s, cette m�thode s�lectionne le premier.
		*/
		void selectObject(const PfPoint& rc_point);
		/**
		* @brief D�s�lectionne tous les objets.
		*/
		void unselectAllObjects();
		/**
		* @brief Ajoute un objet � la map de ce mod�le.
		* @param slot le slot de wad � ajouter.
		* @param rc_point le point d'ajout.
		* @throw PfException si un objet n'est pas trouv�.
		*
		* Si le point n'est pas valide, rien n'est fait.
		*
		* Pour les slots de wad suivants, un traitement sp�cial est ensuite effectu� :
		* <ul><li>WAD_GRASS : tout objet Grass d�j� pr�sent sur la m�me case est supprim�, puis la m�thode MapEditorModel::placeGrass est appel�e pour
		* g�n�rer les bordures,</li>
		* <li>FENCEx : tout objet Fence d�j� pr�sent sur la m�me case est supprim�, puis la m�thode MapEditorModel::connectFences est appel�e.</li></ul>
		*/
		void addObject(PfWadSlot slot, const PfPoint& rc_point);
		/**
		* @brief Suprrime les objets s�lectionn�s.
		*
		* Supprime l'objet dont le nom est MapEditorModel::m_ANIM_SELECTEDObj puis vide ce champ.
		* Si ce champ est vide, alors rien n'est fait.
		*
		* @remarks
		* Pour l'instant seul un objet peut �tre s�lectionn� � la fois.
		*/
		void removeObjects();
		/**
		* @brief Remet les effets stock�s dans MapEditorModel::m_widgetEffects � EFFECT_NONE.
		*/
		void resetEffects();
		/**
		* @brief Sauvegarde la map.
		* @throw PfException si la map n'est pas trouv�e.
		*
		* Le nom du fichier est d�fini par le champ MapEditorModel::m_currentMapName.
		* Si cette valeur est "", alors rien n'est fait.
		*
		* Le nom du wad de la map ouverte est enregistr� dans le fichier,
		* puis la m�thode Map::saveData est appel�e.
		* La sauvegarde des objets est ensuite effectu�e.
		*/
		void saveMap();
		/**
		* @brief Place les �l�ments d'herbe n�cessaires autour d'une case.
		* @param cellCoord la case centrale sur laquelle est suppos�e avoir �t� ajout�e un �l�ment d'herbe (slot WAD_GRASS).
		* @throw PfException si aucun objet Grass n'est trouv� sur la case dont les coordonn�es sont pass�es en param�tre.
		*
		* Cette m�thode v�rifie les cases situ�es autour de celle aux coordonn�es pass�es en param�tre.
		* En fonction de la pr�sence ou non d'un objet WAD_GRASS sur chacune des cases, des bordures d'herbes sont ajout�es
		* (objet Grass avec le code objet appropri�).
		*/
		void placeGrass(pair<unsigned int, unsigned int> cellCoord);
		/**
		* @brief Connecte les barri�res entourant une case.
		* @param cellCoord la case centrale sur laquelle est suppos�e avoir �t� ajout�e un piquet (slot FENCEx).
		* @param slot le slot de wad de la barri�re � traiter.
		* @throw PfException si aucun objet Fence n'est trouv� sur la case dont les coordonn�es sont pass�es en param�tre.
		*
		* La connexion entre barri�res se fait par le changement du code objet de la barri�re pr�sente sur la case dont les coordonn�es sont pass�es
		* en param�tres.
		*
		* Chaque piquet se transforme selon ces animations, pour joindre un autre piquet situ�e sur la case suivante au Nord ou � l'Est.
		*
		* Si les directions Nord et Est ou Sud et Ouest doivent �tre connect�es en m�me temps, un deuxi�me objet Fence est cr��.
		*/
		void connectFences(pair<unsigned int, unsigned int> cellCoord, PfWadSlot slot);
		/**
		* @brief M�thode utilis�e par les constructeurs pour cr�er les composants GUI.
		* @throw PfException si une erreur survient lors de la cr�ation d'un objet.
		*
		* @warning
		* La map doit �tre construite avant appel � cette m�thode.
		*/
		void createGUI();

		Map* mp_map; //!< La map de ce mod�le (ajout�e en tant que ModelItem).
		vector<pair<unsigned int, unsigned int> > m_selCoord_v; //!< La liste des cases s�lectionn�es (lignes/colonnes).
		vector<pair<unsigned int, unsigned int> > m_bufSelCoord_v; //!< La liste des cases en cours de s�lection (lignes/colonnes).
		vector<pair<unsigned int, unsigned int> > m_dltSelCoord_v; //!< La liste des curseurs � supprimer.
		vector<pair<unsigned int, unsigned int> > m_objOnCells_v; //!< La liste des cases sur lesquelles des objets ont �t� pos�s durant le glisser en cours.
		unsigned int m_pinRow; //!< La ligne du pivot de s�lection.
		unsigned int m_pinCol; //!< La colonne du pivot de s�lection.
		pfflag32 m_widgetEffects; //!< Les effets de widgets � prendre en compte.
		bool m_textureMode; //!< Indique si l'on se trouve en mode texture.
		int m_currentTerrainIndex; //!< La texture actuellement s�lectionn�e.
		string m_currentMapName; //!< Le nom actuel de la map.
		PfWad* mp_wad; //!< Le wad ouvert dans ce mod�le.
		vector<PfWadSlot> m_wadSlotsInGrid_v; //!< La liste des slots de wads dans l'ordre de la grille (utile si des slots sont saut�s).
		PfWadSlot m_currentWadSlot; //!< Le slot actuellement s�lectionn�.
		TextureGenerationMode m_topoMode; //!< Le mode de topologie s�lectionn�.
		pfflag32 m_selOr; //!< L'orientation d'inclinaison � appliquer aux cases s�lectionn�es (sous forme de flag).
		string m_ANIM_SELECTEDObj; //!< Le nom de l'objet actuellement s�lectionn�, vide si aucune s�lection.
		unsigned int m_ANIM_SELECTEDObjIndex; //!< L'indice de s�lection d'objet (utilis� par la m�thode MapEditorModel::selectObject).
		PfRectangle m_mouseCursorRect; //!< Le rectangle de position de l'objet accompagnant le curseur de la souris.
		unsigned int m_currentScriptEntry; //!< L'indice de l'entr�e de script actuellement �dit�e, base 1.
};

#endif // MAPEDITORMODEL_H_INCLUDED

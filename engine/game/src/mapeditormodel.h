/**
* @file
* @author Anaïs Vernet
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
* @brief Modèle MVC dédié à la gestion d'un éditeur de map vue de haut.
*
* La sélection de cases sur la map peut se faire case par case ou au moyen d'une sélection rectangulaire.
* Pour réaliser une sélection rectangulaire, une case pivot est spécifiée au moyen d'une ligne et d'une colonne,
* mises à jour à chaque sélection d'une case unique.
*
* Les cases sélectionnées sont stockées dans la liste MapEditorModel::m_selCoord_v, indiquant les cases sélectionnées.
* Une autre liste indique les cases en cours de sélection non envore validée, par exemple lors d'une sélection rectangulaire.
*
* Ce modèle utilise un pointeur PfWad pour initialiser son champ MapEditorModel::mp_wad.
* Ce pointeur sera détruit par le destructeur de cette classe.
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
		* @param p_wad le wad à utiliser.
		* @param texName le nom du fichier de textures de la map (sans l'extension).
		* @throw ConstructorException si une erreur survient lors de la construction de la map ou de l'interface graphique.
		*
		* Le wad sera détruit par le destructeur de cette classe.
		*/
		MapEditorModel(unsigned int rows, unsigned int columns, PfWad* p_wad, const string& texName);
		/**
		* @brief Constructeur MapEditorModel 2.
		* @param fileName le nom du fichier map à charger (avec l'extension).
		* @throw ConstructorException si une erreur survient lors de la lecture du fichier ou la construction d'un objet.
		*
		* Le premier octet du fichier est la version PFGAME_VERSION (fichier "gen.h").
		* S'il n'y a pas correspondance, une exception est levée.
		*/
		explicit MapEditorModel(const string& fileName);
		/**
		* @brief Destructeur MapEditorModel.
		*
		* Détruit le wad.
		*/
		virtual ~MapEditorModel();
		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Applique les effets des widgets, en fonction de la phase actuelle.
		* @throw PfException si un objet n'est pas trouvé.
		*
		* La méthode MapEditorModel::resetEffects est ensuite appelée.
		*/
		void applyEffects();
		/**
		* @brief Affiche une petite icône à côté de la souris représentant le slot de wad en cours de sélection.
		*
		* Si le slot de wad est WAD_NULL_OBJECT, rien n'est affiché.
		*/
		void displayMouseObjectIcon();
		/*
		* Redéfinitions
		* -------------
		*/
		/**
		* @brief Traite l'instruction passée en paramètre.
		* @return RETURN_OK si l'instruction a été traitée, RETURN_NOTHING sinon.
		* @throw PfException si une erreur survient.
		*
		* Certaines instructions doivent être accompagnées d'un DataPackage "souris" structuré de la façon suivante,
		* l'ordre des char devant être respecté, 1 indiquant "oui" et 0 "non" :
		* <ul><li>deux float pour les coordonnées d'un point (lié à la souris),</li>
		* <li>un char pour indiquer si ALT est enfoncé,</li>
		* <li>un char pour indiquer si CTRL est enfoncé,</li>
		* <li>un char pour indiquer si SHIFT est enfoncé,</li>
		* <li>un char pour indiquer s'il y a glissement de souris (si "non", alors il s'agit d'un clic),</li>
		* <li>un char pour indiquer si le bouton gauche est enfoncé pendant un glissement,</li>
		* <li>un char pour indiquer si le bouton droit est enfoncé pendant un glissement,</li>
		* <li>un char pour indiquer si l'action a lieu dans les bordures.</li></ul>
		*
		* Appelle la méthode MapEditorModel::playCurrentPhase.
		*/
		virtual PfReturnCode processInstruction();
		/**
		* @brief Réalise les actions associées à la phase en cours.
		*/
		virtual void processCurrentPhase();
		/**
		* @brief Réalise les actions associées à la sortie de la phase en cours.
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
		* Méthodes
		* --------
		*/
		/**
		* @brief Met à jour la sélection d'un layout de ce modèle.
		* @param rc_point le point de sélection.
		* @param name le nom du layout.
		* @param justHighlight vrai si la sélection ne doit être qu'une emphase.
		* @return RETURN_EMPTY si aucun widget n'est sous la sélection, RETURN_NOTHING si le widget est déjà sélecitonné, ou RETURN_OK sinon.
		* @throw PfException si l'objet "layout" n'est pas trouvé ou si une erreur survient lors de la sélection.
		*/
		PfReturnCode changeSelection(const PfPoint& rc_point, const string& name, bool justHighlight = false);
		/**
		* @brief Retourne les coordonnées d'une case en fonction d'un point de l'écran.
		* @param rc_point le point à considérer.
		* @return les coordonnées correspondantes, (1;1) étant la case inférieure gauche et (0;0) signifiant l'absence de case au point spécifié.
		*
		* Cette méthode prend en compte un point de l'écran et fait ses calculs en considérant la position de la caméra.
		*
		* Si le point est dans une bordure, (0;0) est retourné.
		*/
		pair<unsigned int, unsigned int> mapCoordAt(const PfPoint& rc_point) const;
		/**
		* @brief Sélectionne la case aux coordonnées spécifiées.
		* @param row la ligne de la case.
		* @param col la colonne de la case.
		*/
		void selectCell(unsigned int row, unsigned int col);
		/**
		* @brief Sélectionne la case aux coordonnées spécifiées.
		* @param coord_pair la paire de coordonnées.
		*/
		void selectCell(pair<unsigned int, unsigned int> coord_pair);
		/**
		* @brief Réalise une sélection rectangulaire de la case pivot jusqu'à la case dont les coordonnées sont spécifiées.
		* @param coord_pair les coordonnées spécifiées.
		*
		* Si la case pivot n'est pas valide, alors une sélection unique est réalisée.
		*/
		void squareSelect(pair<unsigned int, unsigned int> coord_pair);
		/**
		* @brief Désélectionne l'ensemble des cases.
		*/
		void unselectAll();
		/**
		* @brief Change la sélection de la liste dont le nom est passé en paramètre.
		* @param name le nom de la liste.
		* @param rc_point le point de sélection.
		* @param activate vrai si la liste doit être activée.
		* @return vrai si un bouton est activé.
		* @throw PfException si la liste n'est pas trouvable.
		*/
		bool selectInList(const string& name, const PfPoint& rc_point, bool activate);
		/**
		* @brief Active ou désactive la boîte de texte en fonction du point.
		* @param rc_point le point d'activation.
		* @throw PfException si la boîte de texte n'est pas trouvable.
		*/
		void clickOnTextBox(const PfPoint& rc_point);
		/**
		* @brief Valide la sélection temporaire.
		*
		* Les cases du tableau tampon sont transférés dans le tableau de sélection. Le tableau tampon est vidé.
		*
		* Cette méthode doit typiquement être appelée pour valider une sélection rectangulaire.
		*/
		void validateBufferSelection();
		/**
		* @brief Met à jour la position des curseurs en fonction des cases sélectionnées.
		*
		* Pour chaque case de la map, si la case est sélectionnée,
		* alors un curseur est généré portant le nom "MapCursor(x;y)" où x et y sont les coordonnées de la case (en partant de (1;1) en bas à gauche).
		* Chaque curseur correspondant aux coordonnées de la liste de suppression est supprimé.
		*
		* L'affichage du curseur prend en compte les caractéristiques de la case, comme sa hauteur par exemple.
		*/
		void updateCursors();
		/**
		* @brief Modifie la topologie locale aux curseurs.
		* @param z la hauteur relative de modification.
		* @param radius le rayon de modification.
		* @param mode le mode de génération de la texture de topologie.
		* @param rel vrai si la valeur de hauteur est relative à la hauteur des cases.
		* @param behavior 0 : modification systématique, <0 : modification uniquement si la nouvelle hauteur est inférieure à l'ancienne, >0 : modification uniquement si la nouvelle hauteur est supérieure à l'ancienne.
		* @param minValue la hauteur minimale des cases.
		* @param slope la pente à utiliser.
		*/
		void changeTopology(int z, unsigned int radius, TextureGenerationMode mode, bool rel = false, int behavior = 0, unsigned int minValue = 4, unsigned int slope = 1);
		/**
		* @brief Modifie la hauteur des cases aux coordonnées spécifiées.
		* @param rc_cells_v la liste des cases concernées.
		* @param z la hauteur des cases.
		* @param rel vrai si la valeur de hauteur est relative à la hauteur des cases.
		* @param behavior 0 : modification systématique, <0 : modification uniquement si la nouvelle hauteur est inférieure à l'ancienne, >0 : modification uniquement si la nouvelle hauteur est supérieure à l'ancienne.
		* @throw PfException si un problème survient lors de la mise à jour de la hauteur d'un objet.
		*
		* Les objets présents sur la case sont retirés de la map puis ajoutés à nouveau pour être mis à jour (en hauteur).
		*
		* Les cases sont limitées en hauteur entre les valeurs 0 et MAP_MAX_HEIGHT.
		*/
		void changeCellsHeight(const vector<pair<unsigned int, unsigned int> >& rc_cells_v, int z, bool rel = false, int behavior = 0);
		/**
		* @brief Modifie la hauteur des cases sélectionnées.
		* @param z la hauteur des cases.
		* @param rel vrai si la valeur de hauteur est relative à la hauteur des cases.
		* @throw PfException si un problème survient lors de la mise à jour de la hauteur d'un objet.
		*
		* Les objets présents sur la case sont retirés de la map puis ajoutés à nouveau pour être mis à jour (en hauteur).
		*/
		void changeSelCellsHeight(int z, bool rel = false);
		/**
		* @brief Modifie l'indice de terrain des cases sélectionnées.
		* @param index le nouvel indice de terrain.
		*/
		void changeSelCellTerrain(int index);
		/**
		* @brief Modifie la pente des cases sélectionnées.
		* @param deltaSlope la variation de pente à appliquer.
		*
		* L'orientation est déterminée par le champ MapEditorModel::m_selOr.
		*
		* Si cette orientation est nulle, alors la hauteur de la case est simplement modifiée.
		*/
		void changeSelCellSlope(int deltaSlope);
		/**
		* @brief Exécute une instruction par le layout.
		* @param instruction l'instruction à exécuter.
		* @param value la valeur associée.
		* @return RETURN_OK si l'instruction a été traitée, RETURN_NOTHING sinon.
		* @throw PfException si le layout n'est pas trouvé.
		*
		* Si l'instruction est INSTRUCTION_ACTIVATE, alors le champ MapEditorModel::m_widgetEffects est mis à jour.
		*/
		PfReturnCode executeLayoutInstruction(PfInstruction instruction, int value = 0);
		/**
		* @brief Fait défiler la grille de sélection.
		* @param forward vrai si le défilement est vers le bas ou la droite.
		* @param wadGrid vrai si la grille de wad doit défiler, faux pour la grille de textures.
		* @param steps le nombre de pas.
		* @throw PfException si le layout n'est pas trouvé.
		*/
		void scrollGrid(bool forward, bool wadGrid, int steps = 1);
		/**
		* @brief Appelle la méthode PfWidget::slide sur la sélection.
		* @param layout le nom du layout concerné.
		* @throw PfException si l'objet "layout" n'est pas trouvé ou si une erreur survient lors de la sélection.
		*/
		void slideSelection(const string& layout);
		/**
		* @brief Change le mode d'édition.
		* @throw PfException si le layout n'est pas trouvé.
		*
		* Le bouton de changement de mode change de texte.
		*/
		void changeMode();
		/**
		* @brief Change le mode d'édition de topologie.
		* @throw PfException si le layout n'est pas trouvé.
		*
		* Le changement se fait en fonction de la position du curseur de topologie.
		*/
		void changeTopoMode();
		/**
		* @brief Lit la valeur du rayon topologique.
		* @return le rayon topologique.
		* @throw PfException si le layout n'est pas trouvé.
		*
		* La valeur est lue directement dans la boîte numérique.
		*/
		unsigned int topoRadius();
		/**
		* @brief Lit la valeur de hauteur minimale pour l'édition topologique.
		* @return la hauteur minimale.
		* @throw PfException si le layout n'est pas trouvé.
		*
		* La valeur est lue directement dans la boîte numérique.
		*/
		unsigned int minFloorValue();
		/**
		* @brief Lit la valeur de pente pour l'édition topologique.
		* @return la pente.
		* @throw PfException si le layout n'est pas trouvé.
		*
		* La valeur est lue directement dans la boîte numérique.
		*/
		unsigned int topoSlope();
		/**
		* @brief Sélectionne l'objet au point spécifié, s'il y en a un.
		* @param rc_point le point de sélection.
		* @throw PfException si un objet n'est pas trouvé.
		*
		* Si deux objets sont superposés, cette méthode sélectionne le premier.
		*/
		void selectObject(const PfPoint& rc_point);
		/**
		* @brief Désélectionne tous les objets.
		*/
		void unselectAllObjects();
		/**
		* @brief Ajoute un objet à la map de ce modèle.
		* @param slot le slot de wad à ajouter.
		* @param rc_point le point d'ajout.
		* @throw PfException si un objet n'est pas trouvé.
		*
		* Si le point n'est pas valide, rien n'est fait.
		*
		* Pour les slots de wad suivants, un traitement spécial est ensuite effectué :
		* <ul><li>WAD_GRASS : tout objet Grass déjà présent sur la même case est supprimé, puis la méthode MapEditorModel::placeGrass est appelée pour
		* générer les bordures,</li>
		* <li>FENCEx : tout objet Fence déjà présent sur la même case est supprimé, puis la méthode MapEditorModel::connectFences est appelée.</li></ul>
		*/
		void addObject(PfWadSlot slot, const PfPoint& rc_point);
		/**
		* @brief Suprrime les objets sélectionnés.
		*
		* Supprime l'objet dont le nom est MapEditorModel::m_ANIM_SELECTEDObj puis vide ce champ.
		* Si ce champ est vide, alors rien n'est fait.
		*
		* @remarks
		* Pour l'instant seul un objet peut être sélectionné à la fois.
		*/
		void removeObjects();
		/**
		* @brief Remet les effets stockés dans MapEditorModel::m_widgetEffects à EFFECT_NONE.
		*/
		void resetEffects();
		/**
		* @brief Sauvegarde la map.
		* @throw PfException si la map n'est pas trouvée.
		*
		* Le nom du fichier est défini par le champ MapEditorModel::m_currentMapName.
		* Si cette valeur est "", alors rien n'est fait.
		*
		* Le nom du wad de la map ouverte est enregistré dans le fichier,
		* puis la méthode Map::saveData est appelée.
		* La sauvegarde des objets est ensuite effectuée.
		*/
		void saveMap();
		/**
		* @brief Place les éléments d'herbe nécessaires autour d'une case.
		* @param cellCoord la case centrale sur laquelle est supposée avoir été ajoutée un élément d'herbe (slot WAD_GRASS).
		* @throw PfException si aucun objet Grass n'est trouvé sur la case dont les coordonnées sont passées en paramètre.
		*
		* Cette méthode vérifie les cases situées autour de celle aux coordonnées passées en paramètre.
		* En fonction de la présence ou non d'un objet WAD_GRASS sur chacune des cases, des bordures d'herbes sont ajoutées
		* (objet Grass avec le code objet approprié).
		*/
		void placeGrass(pair<unsigned int, unsigned int> cellCoord);
		/**
		* @brief Connecte les barrières entourant une case.
		* @param cellCoord la case centrale sur laquelle est supposée avoir été ajoutée un piquet (slot FENCEx).
		* @param slot le slot de wad de la barrière à traiter.
		* @throw PfException si aucun objet Fence n'est trouvé sur la case dont les coordonnées sont passées en paramètre.
		*
		* La connexion entre barrières se fait par le changement du code objet de la barrière présente sur la case dont les coordonnées sont passées
		* en paramètres.
		*
		* Chaque piquet se transforme selon ces animations, pour joindre un autre piquet située sur la case suivante au Nord ou à l'Est.
		*
		* Si les directions Nord et Est ou Sud et Ouest doivent être connectées en même temps, un deuxième objet Fence est créé.
		*/
		void connectFences(pair<unsigned int, unsigned int> cellCoord, PfWadSlot slot);
		/**
		* @brief Méthode utilisée par les constructeurs pour créer les composants GUI.
		* @throw PfException si une erreur survient lors de la création d'un objet.
		*
		* @warning
		* La map doit être construite avant appel à cette méthode.
		*/
		void createGUI();

		Map* mp_map; //!< La map de ce modèle (ajoutée en tant que ModelItem).
		vector<pair<unsigned int, unsigned int> > m_selCoord_v; //!< La liste des cases sélectionnées (lignes/colonnes).
		vector<pair<unsigned int, unsigned int> > m_bufSelCoord_v; //!< La liste des cases en cours de sélection (lignes/colonnes).
		vector<pair<unsigned int, unsigned int> > m_dltSelCoord_v; //!< La liste des curseurs à supprimer.
		vector<pair<unsigned int, unsigned int> > m_objOnCells_v; //!< La liste des cases sur lesquelles des objets ont été posés durant le glisser en cours.
		unsigned int m_pinRow; //!< La ligne du pivot de sélection.
		unsigned int m_pinCol; //!< La colonne du pivot de sélection.
		pfflag32 m_widgetEffects; //!< Les effets de widgets à prendre en compte.
		bool m_textureMode; //!< Indique si l'on se trouve en mode texture.
		int m_currentTerrainIndex; //!< La texture actuellement sélectionnée.
		string m_currentMapName; //!< Le nom actuel de la map.
		PfWad* mp_wad; //!< Le wad ouvert dans ce modèle.
		vector<PfWadSlot> m_wadSlotsInGrid_v; //!< La liste des slots de wads dans l'ordre de la grille (utile si des slots sont sautés).
		PfWadSlot m_currentWadSlot; //!< Le slot actuellement sélectionné.
		TextureGenerationMode m_topoMode; //!< Le mode de topologie sélectionné.
		pfflag32 m_selOr; //!< L'orientation d'inclinaison à appliquer aux cases sélectionnées (sous forme de flag).
		string m_ANIM_SELECTEDObj; //!< Le nom de l'objet actuellement sélectionné, vide si aucune sélection.
		unsigned int m_ANIM_SELECTEDObjIndex; //!< L'indice de sélection d'objet (utilisé par la méthode MapEditorModel::selectObject).
		PfRectangle m_mouseCursorRect; //!< Le rectangle de position de l'objet accompagnant le curseur de la souris.
		unsigned int m_currentScriptEntry; //!< L'indice de l'entrée de script actuellement éditée, base 1.
};

#endif // MAPEDITORMODEL_H_INCLUDED

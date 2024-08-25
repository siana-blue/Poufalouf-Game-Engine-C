/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe PfWad.
* @date 31/07/2016
*/

#ifndef WAD_H_INCLUDED
#define WAD_H_INCLUDED

#include "gen.h"
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <string>
#include "animation.h"
#include "enum.h"
#include "noncopyable.h"
#include "mapzone.h"

class AnimatedGLItem;
class PfRectangle;

/**
* @brief Structure regroupant les informations nécessaires à la construction d'un GLItem à partir d'un wad.
*
* A la construction d'un PfWad, les données lues pour chaque slot dans le fichier WAD sont stockées dans cette structure.
*
* Cette structure est ensuite prise comme modèle pour créer les GLItem.
*
* Cette structure possède une liste de listes de frames, et non une liste d'animations.
* En effet les animations sont des objets non copiables qui stockent, en plus de leurs propriétés, des informations quant à leur position par exemple.
* La liste de frames est la partie immuable d'une animation et permet d'en créer.
*/
struct PfWadObject
{
	map<PfAnimationStatus, vector<PfAnimationFrame> > frames_v_map; //!< La map des listes de frames des animations de cet objet.
	map<PfAnimationStatus, pfflag32> flags_map; //!< La map des flags associés à chaque animation de cet objet.
	map<PfBoxType, vector<MapZone> > zones_v_map; //!< La map des zones de cet objet.
	map<PfAnimationStatus, vector<pair<PfBoxType, unsigned int> > > boxAnimLinks_v_map; //!< La map des liens animation <-> zones de cet objet.
	PfPoint center; //!< Le centre de cet objet.
};

/**
* @brief Classe permettant la manipulation d'un WAD.
*
* Le constructeur de cette classe charge toutes les ressources trouvées dans le fichier WAD correspondant dans les contextes adéquats.
* Puis, les informations concernant chaque objet sont stockées.
*
* @warning
* Ne jamais créer deux PfWad en même temps.
* En effet, les objets créés par un PfWad portent un nom correspondant à leur slot, suivi d'un indice géré par cette classe.
* Deux objets créés par deux wads différents peuvent ainsi avoir le même nom, ce qui entrerait en conflit dans la gestion du modèle.
*
* Le champ statique PfWad::s_wadOpen inique si un PfWad est en cours d'utilisation.
* La construction d'un autre PfWad dans ce cas lève une exception.
*
* Pour charger deux wads, il est possible d'utiliser la méthode PfWad::addWad qui ajoute les données d'un fichier WAD à ce PfWad.
*
* Le nom du fichier ayant servi à créer ce wad est stocké dans le champ PfWad::m_name, sans le chemin complet ni l'extension.
* Cette information peut être utilisée pour faire le lien vers ce wad dans une sauvegarde par exemple.
* Par défaut, le nom du wad est "".
* Si un wad est ajouté par la méthode PfWad::addWad, alors c'est le premier nom qui est conservé.
*
* Afin d'éviter de stocker deux fois les textures d'un même wad qui serait ouvert deux fois,
* la map g_wadFirstIndexes_map (fichier "wad.cpp") associe à chaque nom de wad ouvert durant l'exécution du programme
* l'indice PfWad::m_firstTextureIndex.
* Un wad ayant déjà été ouvert, à sa nouvelle création, prendra comme indice de première texture la valeur stockée,
* ce qui conduira PfWad::addRes à ne pas charger deux fois le même fichier en mémoire
* (car la fonction <em>addTexture</em> du fichier "glfunc.h" verra un indice de texture déjà existant).
*
* Les PfWad déterminent leurs indices de texture en utilisant le champ statique PfWad::s_maxTextureIndex.
* Les PfWad, se croyant seuls au monde, considèrent donc que rien d'autre qu'eux ne peut générer de textures.
* Or, c'est faux, la texture de la police par exemple est définie dans le fichier "gen.h".
* Il faut donc faire attention à donner des indices très élevés aux autres générateurs de textures potentiels, comme pour la police par exemple,
* car les PfWad démarrent à 0.
*/
class PfWad : private NonCopyable
{
    public:
	    static bool s_wadOpen; //!< Indique si un wad est déjà ouvert dans ce programme.
		static unsigned int s_maxTextureIndex; //!< Le dernier indice de texture utilisé.

		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur PfWad 1.
		* @param wadName le nom du WAD (nom du fichier sans l'extension).
		* @throw PfException si PfWad::s_wadOpen est vrai.
		* @throw ConstructorException si une erreur survient.
		*
		* Toute ressource trouvée est ajoutée au contexte.
		*
		* A la fin du constructeur, le champ statique PfWad::s_wadOpen passe à l'état vrai, pour empêcher la création d'un autre PfWad.
		*/
		PfWad(const string& wadName);
		/**
		* @brief Constructeur PfWad 2.
		* @param wadName le nom du fichier WAD à charger.
		* @param menuName le nom du menu affichant la progression du chargement.
		* @param menuWadName le nom du wad du menu.
		* @throw PfException si PfWad::s_wadOpen est vrai.
		* @throw ConstructorException si l'un des fichiers ne peut pas être ouvert.
		*
		* Charge les ressources de ce wad en affichant le résultat dans un menu.
		*
		* Les noms de fichier sont donnés sans répertoire et sans extension.
		*
		* Ce constructeur crée un système MVC de menu, à partir des noms de fichiers fournis.
		* La méthode PfWad::loadStepByStep est ensuite appelée jusqu'à lecture complète du fichier wad.
		* A chaque itération, les valeurs du menu sont effacées et le pas de chargement est ajouté en tant que valeur.
		*
		* Ceci peut être utilisé pour afficher une barre de progression.
		*/
		PfWad(const string& wadName, const string& menuName, const string& menuWadName);
		/**
		* @brief Destructeur PfWad.
		*
		* Passe le champ statique PfWad::s_wadOpen à l'état faux.
		*/
		~PfWad();
		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Construit un GLItem à partir des données de ce wad.
		* @param slot le slot à lire.
		* @param rectangle le rectangle des coordonnées de l'objet à construire.
		* @param layer le plan de perspective du GLItem construit.
		* @param value1 une valeur.
		* @param value2 une valeur.
		* @param value3 une valeur.
		* @param value4 une valeur.
		* @param gameMode le mode de jeu à considérer.
		* @return l'AnimatedGLItem construit.
		* @throw ArgumentException si le slot n'est pas présent dans ce wad.
		* @throw PfException si une erreur survient.
		*
		* Le nom de l'AnimatedGLItem est égal au nom du slot suivi d'un '_' et du numéro d'ordre de cet objet, indiqué par le champ PfWad::m_indexes_map.
		*
		* Le type du GLItem retourné est AnimatedGLItem par défaut.
		* Pour les slots suivants, pour un mode de jeu GAME_NONE, le type retourné est indiqué ci-dessous :
		* <ul><li>WAD_MOB : Mob,</li>
		* <li>JUNGLEx : Jungle,</li>
		* <li>WAD_GRASS : Grass,</li>
		* <li>GUI_BUTTONx : PfWidget, value3 et value4 indiquent repsectivement les marges horizontales et verticales pour le texte,</li>
		* <li>WAD_GUI_SELECTIONGRID : PfSelectionGrid ;
		* les valeurs value1 et 2 indiquent le nombre de lignes et de colonnes,</li>
		* <li>WAD_GUI_SCROLLBAR : PfScrollBar ; le nombre de divisions pour l'ascenseur est précisée par value1,
		* value2 différent de 0 indique une barre horizontale,
		* value3 et value4 indiquent respectivement la largeur et la hauteur des boutons,</li>
		* <li>GUI_LISTx : PfList ; value1 indique le nombre de boutons et value3 la taille d'un bouton,</li>
		* <li>GUI_LABELx : PfWidget ; value1 indique le nombre de lignes, qui sera fixé à 1 par défaut (même si value1 == 0),
		* value3 et value4 indiquent respectivement les marges horizontales et verticales pour le texte,</li>
		* <li>WAD_GUI_TEXTBOX : PfTextBox ; value1 indique l'indice de texture de la police, FONT_TEXTURE_INDEX par défaut,
		* value3 et value4 indiquent respectivement les marges horizontales et verticales pour le texte,</li>
		* <li>WAD_GUI_SPINBOX : PfSpinBox ; value1 et value2 indiquent les valeurs min et max de la boîte, il doit s'agir d'entiers non signés,</li>
		* <li>WAD_GUI_SLIDINGCURSOR : PfSlidingCursor ; value1 indique le nombre de pas, il doit s'agir d'un entier non signé non nul,
		* value2 indique si la barre est verticale (valeur non nulle) ou horizontale (valeur nulle),</li>
		* <li>WAD_GUI_PROGRESSBAR : PfProgressBar ; value1 indique le nombre de divisions, il doit s'agir d'un entier non signé non nul.</li></ul>
		*
		* Pour d'autres types de jeu, un type de base peut être imposé, convertissant tout type non compatible en objet standard du type imposé :
		* <ul><li>GAME_MAP : MapObject, les zones COLLISION sont utilisées ;
		* la zone MAIN définit les coordonnées w et h, en multiples de MAP_CELL_SIZE ;
		* la zone COLLISION est décrite en coordonnées relatives à la zone MAIN.</li></ul>
		*
		* Pour le type de jeu GAME_MAP, les slots suivants génèrent des objets spécifiques, tout autre slot génére un MapObject par défaut :
		* <ul><li>WAD_MOB : Mob,</li>
		* <li>JUNGLEx : Jungle,</li>
		* <li>WAD_GRASS : Grass ; si value1 est différent de 0, la génération d'éléments n'est pas réalisée,</li>
		* <li>FENCEx : Fence,</li>
		* <li>SIGNx : Sign,</li>
		* <li>WAD_BACKGROUND : MapBackground ; les paramètres value1 et value2 indiquent respectivement le nombre de lignes et de colonnes de la map ;
		* attention, ce n'est pas un MapObject et il ne faut surtout pas lui associer de zone de collision,</li>
		* <li>PORTALx : Portal.</li></ul>
		* Les propriétés des objets PfWadObjProperty sont affectées ici pour ce type de jeu :
		* <ul><li>colHeight : pour tout objet sauf WAD_BACKGROUND.</li></ul>
		*
		* Certains types de GLItem peuvent nécessiter des informations supplémentaires pour être crées, les champs valueX permettent de remédier à cela.
		*
		* @warning
		* De la mémoire est allouée pour le pointeur retourné.
		*/
		AnimatedGLItem* generateGLItem(PfWadSlot slot, const PfRectangle& rectangle = PfRectangle(), int layer = MAX_LAYER,
                                       int value1 = 0, int value2 = 0, float value3 = 0.0, float value4 = 0.0, PfGameMode gameMode = GAME_NONE);
		/**
		* @brief Ajoute les données d'un wad à ce PfWad.
		* @param wadName le nom du WAD (nom du fichier sans l'extension).
		* @throw FileException si le fichier ne peut pas être ouvert.
		* @throw ArgumentException si le wad passé en paramètre n'est pas valide ou si l'un de ses slots a déjà été chargé dans ce PfWad.
		*/
		void addWad(const string& wadName);
		/**
		* @brief Retourne une icône représentant un objet de ce WAD.
		* @param slot le slot de l'objet à retourner.
		* @return l'image correspondante.
		* @throw PfException si le slot n'est pas trouvé.
		* @throw PfException si l'animation ANIM_IDLE n'est pas trouvée ou n'a pas de frame.
		*
		* L'image est retournée sous la forme d'une paire, contenant un rectangle de coordonnées de texture (second)
		* associé à un indice de texture (first).
		*
		* L'image correspond au coin supérieur gauche de l'image associée à l'animation ANIM_IDLE de chaque objet.
		*/
		pair<unsigned int, PfRectangle> icon(PfWadSlot slot) const;
		/**
		* @brief Retourne une liste d'icônes représentant les objets de ce WAD.
		* @param excludedSlots_set la liste des slots de wad à exclure.
		* @return la liste d'images.
		*
		* Fait appel à la méthode PfWad::icon sur tous les slots non exclus.
		*/
		vector<pair<unsigned int, PfRectangle> > icons(const set<PfWadSlot>& excludedSlots_set = set<PfWadSlot>()) const;
		/**
		* @brief Retourne la liste des slots disponibles dans ce wad.
		* @param excludedSlots_set la liste des slots de wad à exclure s'ils existent.
		* @return la liste des slots.
		*/
		vector<PfWadSlot> slots(const set<PfWadSlot>& excludedSlots_set = set<PfWadSlot>()) const;
		/**
		* @brief Indique si le slot passé en paramètre est disponible dans ce wad.
		* @return vrai si ce slot est disponible.
		*/
		bool hasSlot(PfWadSlot slot) const;
		/**
		* @brief Charge une ressource pour ce wad, à partir d'un flux prépositionné.
		* @param r_ifs le flux en lecture, prépositionné.
		* @param textureIndexOffset le décalage à appliquer aux indices de textures associés aux ressources.
		* @return le pas de chargement en fin de méthode (1 pour une ressource chargée, etc...)
		* @throw PfException si une erreur survient lors de l'ajout de ressources.
		*
		* Le flux doit être placé juste après l'octet de version pour la première lecture.
		*
		* Si le champ PfWad::m_currentLoadStep est nul, alors le nombre total
		* de textures est lu dans le flux, puis une texture est chargée (sauf
		* si la valeur est 0). Le champ PfWad::m_totalTextCount est renseigné.
		*
		* Si le champ PfWad::m_currentLoadStep est égal à PfWad::m_totalTextCount
		* tout en étant non nul, alors le nombre total de sons est lu dans le flux,
		* puis un son est chargé (sauf si la valeur est 0). Le champ
		* PfWad::m_totalSoundCount est renseigné.
		*
		* Si le champ PfWad::m_currentLoadStep est égal à la somme des champs
		* PfWad::m_totalTextCount et PfWad::m_totalSoundCount, alors la méthode
		* PfWad::addSlots est appelée.
		* La valeur retournée par cette méthode est alors égale à 0, ainsi que
		* le champ PfWad::m_currentLoadStep.
		*
		* La méthode pas à pas ne peut être utilisée que pour l'initialisation d'un wad, et non dans une démarche similaire à PfWad::addWad.
		* Le champ PfWad::m_prevResCount n'est pas considéré.
		*
		* Le flux n'est pas rembobiné en fin de méthode.
		*
		* (contexte pas à pas)
		*/
		unsigned int loadStepByStep(ifstream& r_ifs, unsigned int textureIndexOffset);
		/*
		* Accesseurs
		* ----------
		*/
		const string& getName() const {return m_name;}

	private:
		/**
		* @brief Ajoute les ressources d'un fichier WAD au contexte.
		* @param r_ifs le flux en lecture, positionné juste après les quatre octets de version du programme (début de fichier + 4).
		* @param textureIndexOffset le décalage à appliquer aux indices de textures associés aux ressources.
		* @throw PfException si une erreur survient lors de l'ajout de ressources.
		*
		* Le flux n'est pas rembobiné en fin de méthode.
		*/
		void addRes(ifstream& r_ifs, unsigned int textureIndexOffset);
		/**
		* @brief Génère les PfWadObject de ce PfWad à partir d'un fichier WAD.
		* @param r_ifs le flux en lecture, positionné juste après les ressources.
		* @param textureIndexOffset le décalage à appliquer aux indices de textures associés aux ressources.
		* @throw PfException si une erreur survient.
		*
		* Le flux n'est pas rembobiné en fin de méthode.
		*/
		void addSlots(ifstream& r_ifs, unsigned int textureIndexOffset);

		map<PfWadSlot, PfWadObject> m_wadObjects_map; //!< La map des objets de ce wad.
		map<string, int> m_indexes_map; //!< La map stockant le nombre d'objets correspondant à un slot de wad. La clé est le nom du slot (fonction <em>textFrom</em>, fichier "enum.h").
		unsigned int m_resCount; //!< Le nombre d'indices de textures chargées.
		unsigned int m_totalTextCount; //!< Le nombre total d'indices de textures à charger (pas à pas).
		unsigned int m_totalSoundCount; //!< Le nombre total de sons à charger (pas à pas).
		unsigned int m_currentLoadStep; //!< Le pas de chargement (pas à pas).
		string m_name; //!< Le nom du fichier de ce wad.
		vector<unsigned int> m_sounds_v; //!< La liste des indices de sons (gérés par le fichier "fmodfunc.h") dans l'ordre des ajouts par ce wad.
		unsigned int m_prevSoundsCount; //!< Le nombre de sons chargés pour un premier wad (utilisé pour PfWad::addWad).
};

#endif // WAD_H_INCLUDED

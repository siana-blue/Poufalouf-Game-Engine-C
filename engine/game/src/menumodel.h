/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe MenuModel.
* @date 04/04/2016
*/

#ifndef MENUMODEL_H_INCLUDED
#define MENUMODEL_H_INCLUDED

#include "gen.h"

#include <vector>
#include <map>
#include <string>

#include "instructionreader.h"
#include "enum.h"

#include "glmodel.h"

#define MENU_MAX_LAYOUTS_COUNT 10 //!< Le nombre maximal de layouts dans un menu.

class PfWad;

/**
* @brief Modèle MVC dédié à la gestion d'un menu.
*
* Un menu est constitué de layouts contenant des widgets.
*
* Un seul layout est actif à la fois, et toutes les méthodes de cette classe concernant les layouts s'appliquent au layout actif.
* La méthode MenuModel::changeCurrentLayout permet quant à elle de sélectionner un autre layout actif.
*
* Un modèle ne peut contenir que MENU_MAX_LAYOUTS_COUNT layouts.
* S'il y a plus de layouts, les valeurs des widgets ne seront plus comptés.
*
* Des textes et des valeurs dynamiques peuvent être ajoutées à ce menu.
* Ces valeurs sont mises à jour depuis l'extérieur du pattern MVC et servent à définir la valeur de certains widgets en fonction d'actions extérieures.
* Par exemple, la progression d'une barre de chargement, ou le texte d'un label.
*
* Les effets activés par les widgets de ce menu sont stockés dans la variable membre MenuModel::m_widgetEffects.
* Il s'agit d'une variable flag correspondant à l'énumération PfEffect (fichier "enum.h").
*/
class MenuModel : public GLModel, public InstructionReader
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur MenuModel.
		* @param name le nom des fichiers à utiliser (sans extension et sans chemin).
		* @param r_wad le wad à utiliser pour la création des composants.
		* @throw ConstructorException si une erreur survient lors de la création du wad ou du menu.
		* @throw FileException si une erreur survient lors de la lecture du fichier MEN.
		*
		* Les fichiers portant le nom passé en paramètre et les extensions WAD_EXT et MEN_EXT (fichier "gen.h") sont utilisés pour générer le menu.
		*
		* Du fait de la lecture d'un wad, les objets créés ont pour nom leur nom de slot (fonction <em>textFrom</em>, fichier "enum.h").
		* Les layouts ont quant à eux pour nom "layout id" où id est le numéro d'identifiant.
		* Au départ, seul le layout d'identifiant 1 est visible.
		*/
		MenuModel(const string& name, PfWad& r_wad);
		/**
		* @brief Destructeur MenuModel.
		*/
		virtual ~MenuModel() {}
		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Change le layout actif.
		* @param id l'identifiant du nouveau layout actif.
		* @throw PfException si l'identifiant passé en paramètre ne correspond à aucun layout ou si un layout n'est pas trouvé.
		*
		* L'ancien layout actif devient inactif, donc invisible.
		*/
		void changeCurrentLayout(int id);
		/**
		* @brief Désactive tout widget activé du layout actif.
		* @throw PfException si le layout actif n'est pas trouvé.
		*/
		void resetLayout();
		/**
		* @brief Change la sélection du layout actif en fonction d'un point.
		* @param rc_point le point de la nouvelle sélection.
		* @param justHighlight vrai si la sélection ne doit être en fait qu'un survol sans mise à jour des coordonnées actuelles du layout.
		* @return RETURN_OK si la sélection a changé, RETURN_EMPTY si le point n'est pas sur un widget et RETURN_NOTHING si le point est sur un widget mais que la sélection n'a pas lieu.
		* @throw PfException si la sélection est impossible malgré un bon positionnement.
		*
		* Cette méthode est notamment utilisée pour traiter les mouvements de la souris.
		*/
		PfReturnCode changeSelection(const PfPoint& rc_point, bool justHighlight = false);
		/**
		* @brief Remet les effets stockés dans MenuModel::m_widgetEffects à EFFECT_NONE.
		*/
		void resetEffects();
		/**
		* @brief Retourne des valeurs correspondant aux widgets des layouts.
		* @return la liste de valeurs.
		*
		* Au moins une valeur est retournée, par défaut 0.
		*
		* Les valeurs sont les suivantes :
		* <ul><li>La première valeur correspond à la valeur de l'effet de ce modèle.
		* Il s'agit de la variable MenuModel::m_widgetEffects filtrée pour ne retenir que les bits de valeur (1 à 8)
		* voir PfEffect (fichier "enum.h"),</li>
		* <li>les autres valeurs sont toutes les valeurs non nulles rencontrées sur les widgets des layouts, dans l'ordre des colonnes puis des lignes, et enfin dans l'ordre des layouts ;
		* les valeurs récupérées sont : les valeurs des boîtes numériques et l'indice du texte sélectionné des listes.</li></ul>
		*/
		vector<int> values() const;
		/**
		* @brief Ajoute un texte dynamique à ce menu.
		* @param text le texte à ajouter.
		*
		* L'indice de texte dynamique auquel est lié un widget est déterminé par un fichier, en amont du lancement du programme.
		* Tous les widgets liés à l'indice correspondant à celui du texte nouvellement ajouté sont mis à jour (appel à la méthode PfWidget::changeText).
		*/
		void addText(const string& text);
		/**
		* @brief Supprime les textes dynamiques de ce menu.
		* @param update vrai si les widgets concernés doivent être mis à jour (valeur par défaut : "").
		*/
		void clearTexts(bool update = false);
		/**
		* @brief Ajoute une valeur dynamique à ce menu.
		* @param value la valeur à ajouter.
		*
		* Tous les widgets concernés sont mis à jour.
		* L'indice de valeur dynamique auquel est lié un widget est déterminé par un fichier, en amont du lancement du programme.
		* Tous les widgets liés à l'indice correspondant à celui de la valeur nouvellement ajoutée sont mis à jour (appel à la méthode PfWidget::changeValue).
		*/
		void addValue(int value);
		/**
		* @brief Supprime les valeurs dynamiques de ce menu.
		* @param update vrai si les widgets concernés doivent être mis à jour (valeur par défaut : 0).
		*/
		void clearValues(bool update = false);
		/*
		* Redéfinitions
		* -------------
		*/
		/**
		* @brief Traite l'instruction passée en paramètre.
		* @return RETURN_OK si l'instruction a été traitée, RETURN_NOTHING sinon.
		* @throw PfException si une erreur survient.
		*
		* Transmet l'instruction au layout actif.
		* Si l'instruction est INSTRUCTION_ACTIVATE (fichier "enum.h"),
		* alors MenuModel::m_widgetEffects est mis à jour par appel à la méthode PfLayout::getEffectFlags.
		*
		* Une seule valeur d'instruction est lue (valeur int).
		*/
		virtual PfReturnCode processInstruction();
		/*
		* Accesseurs
		* ----------
		*/
		pfflag32 getWidgetEffects() const {return m_widgetEffects;}

	private:
		pfflag32 m_widgetEffects; //!< Les effets de widgets à prendre en compte.
		int m_currentLayoutId; //!< L'identifiant du layout actif.
		vector<string> m_texts_v; //!< La liste des textes dynamiques de ce menu.
		vector<int> m_val_v; //!< La liste des valeurs dynamiques de ce menu.
		map<unsigned int, vector<pair<int, pair<int, int> > > > m_dynamicTextsWidgets_v_map; //!< La map des widgets affectés par les textes dynamiques : [indice de texte, liste de [ indice de layout, [coordonnées] ] ].
		map<unsigned int, vector<pair<int, pair<int, int> > > > m_dynamicValuesWidgets_v_map; //!< La map des widgets affectés par les valeurs dynamiques : [indice de valeur, liste de [indice de layout, [coordonnées] ] ].
};

#endif // MENUMODEL_H_INCLUDED


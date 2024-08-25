/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe PfLayout.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef PFLAYOUT_H_INCLUDED
#define PFLAYOUT_H_INCLUDED

#include "glgame_gen.h"

#include "pfwidget.h"

/**
* @brief Groupe de widgets.
*
* Cette classe est elle-même un PfWidget, afin de pouvoir faire des imbrications de layouts.
*
* Un layout possède un tableau de pointeurs sur PfWidget à 2 dimensions, dont la taille est fixée par le constructeur.
*
* Un layout est un groupe de widgets interconnectés. Ces widgets sont repérés dans le layout au moyen d'une ligne et d'une colonne. Il ne peut y avoir qu'un
* seul widget par coordonnées ainsi définies. Les coordonnées (1;1) correspondent au widget le plus en bas à gauche dans l'ordre de sélection des widgets.
* Des coordonnées égales à 0 peuvent être rencontrées pour indiquer une sélection invalide ou l'absence de widget dans ce layout.
*
* Ces lignes et ces colonnes ne sont pas liées à la position à l'écran de chaque widget, il ne s'agit que de valeurs permettant de se repérer lors de la
* navigation d'un widget à un autre au moyen des flèches directionnelles. Par exemple, si le widget sélectionné est aux coordonnées (1;2), alors la flèche
* directionnelle droite conduit au widget (1;3). Ce dernier widget peut se trouver n'importe où à l'écran.
* Il est bien entendu conseillé de respecter une cohérence géométrique, mais il faut bien distinguer ces deux aspects.
*
* La position à l'écran des widgets est gérée par les polygones des GLItem dont ils héritent, comme tout autre objet affichable à l'écran.
*/
class PfLayout : public PfWidget
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur PfLayout.
    * @param name Le nom de ce layout.
    * @param rows Le nombre de lignes de ce layout.
    * @param columns Le nombre de colonnes de ce layout.
    *
    * Un tableau à deux dimensions est créé, de taille spécifiée en paramètres, et rempli de pointeurs nuls.
    */
    PfLayout(const string& name, unsigned int rows, unsigned int columns);
    /**
    * @brief Destructeur PfLayout.
    *
    * Appelle le destructeur de tous les widgets contenus dans ce layout.
    */
    virtual ~PfLayout();
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Ajoute un widget à la position spécifiée.
    * @param p_widget Le widget à ajouter.
    * @param row La ligne où ajouter le widget.
    * @param col La colonne où ajouter le widget.
    * @throw ArgumentException si les coordonnées ne sont pas valides.
    *
    * Si les coordonnées spécifiées correspondent à un widget existant, celui-ci est détruit et remplacé.
    *
    * @remarks
    * Le widget ajouté sera détruit par le destructeur de ce layout.
    */
    void addWidget(PfWidget* p_widget, unsigned int row, unsigned int col);
    /**
    * @brief Sélectionne le widget aux coordonnées spécifiées.
    * @param row La ligne du widget à sélectionner.
    * @param col La colonne du widget à sélectionner.
    * @param justHighlight <code>true</code> si la ligne et la colonne actuelles de ce layout ne doivent pas être modifiées (simple emphase visuelle).
    * @return RETURN_OK si un nouveau widget a été sélectionné, RETURN_EMPTY si les coordonnées ne sont pas valides ou si le widget est désactivé
    * et RETURN_NOTHING si le widget à sélectionner l'est déjà.
    *
    * Dans le cas d'une simple emphase, la méthode PfWidget::highlight est appelée sur le widget concerné, sans toucher au widget actuellement sélectionné.
    *
    * Sinon, la méthode PfWidget::select est appelée sur le widget concerné
    * et les champs PfLayout::m_currentRow et PfLayout::m_currentCol sont mis à jour.
    *
    * La méthode PfWidget::leave est appelée sur tous les autres widgets (sauf le widget actuellement sélectionné dans le cas d'une simple emphase).
    *
    * Si le champ PfWidget::m_repeatingSelection du widget est <code>true</code>,
    * alors la sélection est faite même si les coordonnées actuelles sont celles passées en paramètre. Sinon, dans ce cas, la sélection n'est pas répétée.
    *
    * Le booléen ModelItem::m_modified passe à l'état <code>true</code> si la sélection a lieu, que ce soit en simple emphase ou en réelle sélection.
    */
    PfReturnCode selectWidget(unsigned int row, unsigned int col, bool justHighlight = false);
    /**
    * @brief Sélectionne le widget voisin dans la direction spécifiée.
    * @param orientation L'orientation dans laquelle faire la sélection.
    * @return RETURN_OK si un nouveau widget a été sélectionné, RETURN_EMPTY si les coordonnées ne sont pas valides ou si le widget est désactivé
    * et RETURN_NOTHING si le widget à sélectionner l'est déjà.
    * @throw ArgumentException si l'orientation n'est pas valide.
    * @throw PfException si une coordonnée actuelle est 0.
    *
    * Les orientations valides sont les valeurs suivantes de l'énumération PfOrientationValue (bibliothèque PfMedia) : NORTH, EAST, SOUTH, WEST.
    *
    * Tout widget indisponible est ignoré.
    *
    * Le booléen ModelItem::m_modified passe à l'état <code>true</code> si la sélection a lieu.
    */
    PfReturnCode selectNextWidget(PfOrientation::PfOrientationValue orientation);
    /**
    * @brief Sélectionne le widget aux coordonnées spécifiées, ou ne fait rien dans le cas où aucun widget n'est présent en ce point.
    * @param rc_point Le point de sélection.
    * @param justHighlight <code>true</code> si la ligne et la colonne actuelles de ce layout ne doivent pas être modifiés (simple emphase visuelle).
    * @return RETURN_OK si la sélection a lieu, RETURN_EMPTY si le point n'est sur aucun widget ou sur un widget désactivé
    * et RETURN_NOTHING si la sélection n'a pas lieu car le widget à sélectionner l'est déjà.
    *
    * Le booléen ModelItem::m_modified passe à l'état <code>true</code> si la sélection a lieu.
    *
    * Le point passé en paramètre devient point d'action pour le widget sélectionné.
    */
    PfReturnCode selectWidgetAt(const PfPoint& rc_point, bool justHighlight = false);
    /**
    * @brief Retourne les coordonnées du widget au point spécifié.
    * @param rc_point Le point à considérer.
    * @return Les coordonnées du widget, ou (0;0) si aucun widget n'est présent au point spécifié.
    */
    pair<unsigned int, unsigned int> coordAt(const PfPoint& rc_point);
    /**
    * @brief Retourne un pointeur vers un widget de ce layout.
    * @param row La ligne du widget.
    * @param col La colonne du widget.
    * @return Le widget demandé ou 0 si les paramètres ne sont pas valides.
    */
    PfWidget* widget(unsigned int row, unsigned int col);
    /**
    * @brief Retourne un pointeur vers le widget de ce layout portant le nom spécifié.
    * @param name Le nom du widget.
    * @return Le widget demandé ou 0 si le nom n'est pas trouvé.
    */
    PfWidget* widget(const string& name);
    /**
    * @brief Retourne un pointeur constant vers un widget de ce layout.
    * @param row La ligne du widget.
    * @param col La colonne du widget.
    * @return Le widget demandé ou 0 si les paramètres ne sont pas valides.
    */
    const PfWidget* constWidget(unsigned int row, unsigned int col) const;
    /**
    * @brief Quitte tous les widgets sauf le widget actuellement sélectionné qui est quitté puis sélectionné (pour annuler l'activation).
    */
    void leaveAllButCurrent();
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Rend ce widget disponible.
    *
    * Appelle la méthode PfWidget::enable.
    *
    * Appelle la méthode PfWidget::enable sur tous les widgets de ce layout, puis sélectionne le widget aux coordonnées actuelles.
    */
    virtual void enable();
    /**
    * @brief Rend ce widget indisponible.
    *
    * Appelle la méthode PfWidget::disable.
    *
    * Appelle la méthode PfWidget::disable sur tous les widgets de ce layout.
    */
    virtual void disable();
    /**
    * @brief Sélectionne ce widget.
    *
    * Appelle la méthode PfWidget::select sur ce layout et sur le widget aux coordonnées actuelles.
    */
    virtual void select();
    /**
    * @brief Désélectionne ce widget.
    *
    * Appelle la méthode PfWidget::deselect sur ce layout et sur tous ses widgets.
    */
    virtual void deselect();
    /**
    * @brief Action à réaliser lorsque ce widget subit un glisser-déplacer.
    * @return <code>false</code> si aucun déplacement n'a lieu.
    *
    * Appelle la méthode PfWidget::slide sur le widget sélectionné.
    *
    * Le point d'arrivée aura été défini par le point d'activation de ce layout.
    * Ce layout prend alors les effets du widget ayant subi le mouvement, après le mouvement.
    *
    * Si le widget est indisponible, rien n'est fait et <code>false</code> est retourné.
    *
    * @remarks
    * Ce ModelItem est marqué modifié.
    */
    virtual bool slide();
    /**
    * @brief Met à jour ce ModelItem.
    *
    * Appelle la méthode PfWidget::update des widgets contenus dans ce layout.
    *
    * Si un ou plusieurs widgets sont alors marqués modifiés, ce layout est marqué modifié et tous les widgets sont marqués non modifiés.
    */
    virtual void update();
    /**
    * @brief Génère un Viewable à partir de ce ModelItem.
    * @return Le Viewable créé.
    *
    * Le Viewable du PfLayout est généré (PfWidget::generateViewable), puis des Viewable générés par chaque widget sont ajoutés à la liste de ce Viewable.
    *
    * Si un Viewable possède un son, alors il est ajouté au Viewable généré. Seul le dernier son rencontré est pris en compte.
    *
    * @warning
    * De la mémoire est allouée pour le pointeur retourné.
    */
    virtual Viewable* generateViewable() const;
    /**
    * @brief Traite l'instruction passée en paramètre.
    * @return RETURN_OK si l'instruction a été traitée, RETURN_NOTHING sinon.
    *
    * L'instruction est d'abord transférée au widget actuellement sélectionné, s'il y en a un. Si le retour est différent de RETURN_OK, alors l'instruction
    * est traitée par le layout lui-même de la façon suivante :
    *
    * <ul><li>INSTRUCTION_LEFT : appelle la méthode PfLayout::selectNextWidget avec pour paramètre PfOrientation::WEST.</li>
    * <li>INSTRUCTION_UP : appelle la méthode PfLayout::selectNextWidget avec pour paramètre PfOrientation::NORTH.</li>
    * <li>INSTRUCTION_RIGHT : appelle la méthode PfLayout::selectNextWidget avec pour paramètre PfOrientation::EAST.</li>
    * <li>INSTRUCTION_DOWN : appelle la méthode PfLayout::selectNextWidget avec pour paramètre PfOrientation::SOUTH.</li>
    * <li>INSTRUCTION_STOP : appelle la méthode PfLayout::disable.</li></ul>
    *
    * Si un widget est activé par traitement d'une instruction, alors ce layout prend ses effets et tous les autres widgets sont désactivés.
    */
    virtual PfReturnCode processInstruction();
    /*
    * Accesseurs
    * ----------
    */
    unsigned int getRowsCount() const  {return m_rowsCount;} //!< Accesseur.
    unsigned int getColumnsCount() const {return m_columnsCount;} //!< Accesseur.
    unsigned int getCurrentRow() const {return m_currentRow;} //!< Accesseur.
    unsigned int getCurrentCol() const {return m_currentCol;} //!< Accesseur.

private:
    unsigned int m_rowsCount; //!< Le nombre de lignes.
    unsigned int m_columnsCount; //!< Le nombre de colonnes.
    unsigned int m_currentRow; //!< La ligne de l'élément sélectionné.
    unsigned int m_currentCol; //!< La colonne de l'élément sélectionné.
    PfWidget*** mp_widgets_t2; //!< La grille des widgets de ce layout.
};

#endif // PFLAYOUT_H_INCLUDED

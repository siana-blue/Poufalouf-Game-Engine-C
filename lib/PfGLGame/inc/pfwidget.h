/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe PfWidget.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef PFWIDGET_H_INCLUDED
#define PFWIDGET_H_INCLUDED

#include "glgame_gen.h"

#include "instructionreader.h"

#include "glitem.h"
#include "pftext.h"

/**
* @brief Elément de l'interface graphique des jeux Poufalouf.
*
* Il s'agit d'une classe héritée par tous les éléments de l'interface graphique.
* La classe PfWidget peut être instanciée, créant alors un widget simple de type label ou bouton, avec les propriétés basiques d'un widget
* (afficher un texte, s'activer suite à un clic).
*
* @section Proprietes Propriétés d'un widget
*
* Un widget possède des effets déterminés par 32 flags.
* Ces effets sont caractérisés par l'énumération PfWidgetEffect.
*
* Un point peut stocker une zone d'action.
* Sur certains widgets, ce point peut être utilisé pour tester la postion de la souris sur le widget avant action.
*
* L'état d'un widget est déterminé au moyen de l'énumération PfWidget::PfWidgetStatus.
* L'ordre de cette énumération indique un ordre de priorité. En effet, par exemple, si un widget est sélectionné, alors il est forcément disponible.
* Ainsi, lorsque la documentation indique que l'état PfWidget::m_status prend la valeur PfWidget::WIDGET_ENABLED sans perdre un état de priorité supérieure,
* cela signifie que si ce widget est à l'état activé, alors il ne changera pas d'état.
* La méthode <code>protected</code> PfWidget::changeStatus permet de gérer cette priorité, en la laissant automatique ou en la forçant.
*
* @section Texte Gestion du texte d'un widget
*
* Le texte d'un widget est stocké dans le champ PfWidget::m_text.
* Un widget directement instancié via cette classe est un simple label affichant du texte ou un bouton simple.
* Le constructeur 2 est recommandé dans ce cas.
*
* L'indice de texture de la police du texte est égale à FONT_TEXTURE_INDEX (fichier "glgame_gen.h").
* Les textures associées aux animations de cet AnimatedGLItem définissent le fond derrière le texte ou l'image principale du bouton.
*
* Typiquement, pour créer un label (donc ne répondant pas aux commandes), on créera un PfWidget hors de tout PfLayout, avec une seule animation ANIM_IDLE.
* Pour un bouton, on créera un PfWidget avec les animations de la map WIDGET_STANDARD_MAP et on l'insérera dans un layout si besoin.
*/
class PfWidget : public AnimatedGLItem, public InstructionReader
{
public:
    /**
    * @brief Enumération des différents états possibles d'un widget.
    */
    enum PfWidgetStatus {WIDGET_DISABLED, //!< Widget indisponible.
                         WIDGET_ENABLED, //!< Widget disponible.
                         WIDGET_HIGHLIGHTED, //!< Widget surligné.
                         WIDGET_SELECTED, //!< Widget sélectionné.
                         WIDGET_ACTIVATED //!< Widget activé.
                        };

    /**
    * @brief Enumération des différents types de map de statut.
    *
    * Cette énumération, utilisée par le constructeur pour définir la map PfWidget::m_animation_map, permet de fournir quelques maps par défaut.
    * Cette map est utilisée pour associer un statut d'animation, PfAnimationStatus, à l'état d'un widget.
    *
    * Lors de la génération à partir d'un wad, le programme peut définir automatiquement la map à utiliser en fonction des animations disponibles.
    * Typiquement :
    * <ul><li>WIDGET_STANDARD_MAP si les animations ANIM_IDLE, ANIM_HIGHLIGHTED, ANIM_SELECTED et ANIM_ACTIVATED sont disponibles.</li>
    * <li>WIDGET_SELECT_MAP si l'animation ANIM_ACTIVATED n'est pas disponible.</li>
    * <li>WIDGET_HIGHLIGHT_MAP si l'animation ANIM_SELECTED n'est pas disponible.</li>
    * <li>WIDGET_IDLE_MAP si l'animation ANIM_HIGHLIGHTED n'est pas disponible.
    * Dans ce cas, ANIM_DISABLED sera également considéré indisponible, alors qu'il est considéré disponible sinon (non testé).</li></ul>
    */
    enum PfWidgetStatusMap {WIDGET_IDLE_MAP, //!< Map pointant vers l'animation ANIM_IDLE pour tout état (map vide, car ANIM_IDLE est par défaut).
                            WIDGET_STANDARD_MAP, //!< Map standard, disposant d'une animation par état.
                            WIDGET_HIGHLIGHT_MAP, //!< Map dans laquelle les animations de sélection et d'activation sont les mêmes que pour l'emphase.
                            WIDGET_SELECT_MAP //!< Map dans laquelle l'animation d'activation est la même que pour la sélection.
                           };

    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur PfWidget par défaut.
    * @param name Le nom de ce widget.
    * @param layer Le plan de perspective de ce widget.
    * @param statusMap La valeur d'initialisation de la map d'association animation <-> état de ce widget.
    */
    PfWidget(const string& name = "", unsigned int layer = MAX_LAYER, PfWidgetStatusMap statusMap = WIDGET_STANDARD_MAP);
    /**
    * @brief Constructeur PfWidget 1.
    * @param name Le nom de ce widget.
    * @param x L'abscisse de ce widget.
    * @param y L'ordonnée de ce widget.
    * @param w La largeur de ce widget.
    * @param h La hauteur de ce widget.
    * @param layer Le plan de perspective de ce widget.
    * @param statusMap La valeur d'initialisation de la map d'association animation <-> état de ce widget.
    *
    * Les coordonnées sont utilisées pour construire le polygone du GLItem (un rectangle).
    */
    PfWidget(const string& name, float x, float y, float w, float h, unsigned int layer = MAX_LAYER, PfWidgetStatusMap statusMap = WIDGET_STANDARD_MAP);
    /**
    * @brief Constructeur PfWidget 2.
    * @param name Le nom de ce widget.
    * @param rect Le rectangle des coordonnées de ce widget.
    * @param layer Le plan de perspective de ce widget.
    * @param linesCount Le nombre de lignes du texte de ce widget.
    * @param hMargin La marge horizontale du texte de ce widget.
    * @param vMargin La marge verticale du texte de ce widget.
    * @param statusMap La valeur d'initialisation de la map d'association animation <-> état de ce widget.
    *
    * Constructeur typiquement utilisé pour créer un widget simple de type label destiné à afficher un texte sans attendre de commandes de l'utilisateur.
    *
    * Les marges utilisées pour l'affichage de ce texte sont exprimées en fraction des dimensions totales de ce label, définies dans le constructeur.
    * Par exemple, une marge horizontale de 0,25 centrera le texte à un quart de largeur à partir de la gauche et de la droite de ce label.
    */
    PfWidget(const string& name, PfRectangle rect, unsigned int layer = MAX_LAYER, unsigned int linesCount = 1, float hMargin = 0.0, float vMargin = 0.0,
             PfWidgetStatusMap statusMap = WIDGET_STANDARD_MAP);
    /**
    * @brief Destructeur PfWidget.
    */
    virtual ~PfWidget() {}
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Ajoute des effets à ce widget.
    * @param effects Les effets à ajouter.
    */
    void addEffects(pfflag32 effects);
    /**
    * @brief Met les effets de ce widget à EFFECT_NONE.
    */
    void resetEffects();
    /**
    * @brief Associe une animation à un état de ce widget.
    * @param status L'état du widget.
    * @param anim Le statut d'animation à associer.
    */
    void changeStatusAnimation(PfWidgetStatus status, PfAnimationStatus anim);
    /**
    * @brief Ajoute du texte à ce label.
    * @param text Le texte à ajouter.
    *
    * @remarks
    * Ce ModelItem est marqué modifié.
    */
    void addText(const string& text);
    /**
    * @brief Efface le texte de ce label.
    *
    * @remarks
    * Ce ModelItem est marqué modifié.
    */
    void clearText();
    /**
    * @brief Change le type d'alignement du texte de ce label.
    * @param align Le type d'alignement.
    *
    * @remarks
    * Ce ModelItem est marqué modifié.
    */
    void changeTextAlignProp(PfText::TextAlignProp align);
    /**
    * @brief Modifie le texte de ce widget.
    * @param text Le nouveau texte.
    *
    * Les méthodes PfWidget::clearText et PfWidget::addText sont successivement appelées.
    *
    * @remarks
    * Ce ModelItem est marqué modifié.
    */
    void changeText(const string& text);
    /**
    * @brief Modifie la valeur de ce widget.
    *
    * Par défaut, ne fait rien.
    */
    virtual void changeValue(int) {}
    /**
    * @brief Rend ce widget disponible.
    *
    * La méthode PfWidget::changeStatus est appelée pour passer ce widget à l'état PfWidget::WIDGET_ENABLED en respectant la priorité.
    */
    virtual void enable();
    /**
    * @brief Rend ce widget indisponible.
    *
    * La méthode PfWidget::changeStatus est appelée pour passer ce widget à l'état PfWidget::WIDGET_DISABLED en forçant la priorité.
    */
    virtual void disable();
    /**
    * @brief Surligne ce widget.
    *
    * Si ce widget n'est pas disponible, alors rien n'est fait.
    *
    * La méthode PfWidget::changeStatus est appelée pour passer ce widget à l'état PfWidget::WIDGET_HIGHLIGHTED en respectant la priorité.
    */
    virtual void highlight();
    /**
    * @brief Désurligne ce widget.
    *
    * Si ce widget n'est pas disponible, alors rien n'est fait.
    *
    * Appelle les méthodes PfWidget::deactivate puis PfWidget::deselect.
    *
    * La méthode PfWidget::changeStatus est appelée pour passer ce widget à l'état PfWidget::WIDGET_ENABLED en forçant la priorité.
    */
    virtual void leave();
    /**
    * @brief Sélectionne ce widget.
    *
    * Si ce widget n'est pas disponible, alors rien n'est fait.
    *
    * La méthode PfWidget::changeStatus est appelée pour passer ce widget à l'état PfWidget::WIDGET_SELECTED en respectant la priorité.
    */
    virtual void select();
    /**
    * @brief Désélectionne ce widget.
    *
    * Si ce widget n'est pas disponible, alors rien n'est fait.
    *
    * Appelle la méthode PfWidget::deactivate.
    *
    * La méthode PfWidget::changeStatus est appelée pour passer ce widget à l'état PfWidget::WIDGET_ENABLED en forçant la priorité.
    */
    virtual void deselect();
    /**
    * @brief Active ce widget.
    *
    * Si ce widget n'est pas disponible, alors rien n'est fait.
    *
    * La méthode PfWidget::changeStatus est appelée pour passer ce widget à l'état PfWidget::WIDGET_ACTIVATED en respectant la priorité.
    */
    virtual void activate();
    /**
    * @brief Désactive ce widget.
    *
    * Si ce widget n'est pas disponible, alors rien n'est fait.
    *
    * La méthode PfWidget::changeStatus est appelée pour passer ce widget à l'état PfWidget::WIDGET_ENABLED en forçant la priorité.
    */
    virtual void deactivate();
    /**
    * @brief Action à réaliser lorsque ce widget subit un glisser-déplacer.
    * @return <code>false</code> si aucun déplacement n'a lieu.
    *
    * Pour un widget standard, ne fait rien et retourne <code>false</code>.
    */
    virtual bool slide() {return false;}
    /**
    * @brief Indique si ce widget est disponible.
    * @return <code>true</code> si le champ PfWidget::m_widgetStatus a pour valeur PfWidget::WIDGET_ENABLED ou une valeur de priorité supérieure.
    */
    bool isEnabled() const;
    /**
    * @brief Indique si ce widget est surligné.
    * @return <code>true</code> si le champ PfWidget::m_widgetStatus a pour valeur PfWidget::WIDGET_HIGHLIGHTED ou une valeur de priorité supérieure.
    */
    bool isHighlighted() const;
    /**
    * @brief Indique si ce widget est sélectionné.
    * @return <code>true</code> si le champ PfWidget::m_widgetStatus a pour valeur PfWidget::WIDGET_SELECTED ou une valeur de priorité supérieure.
    */
    bool isSelected() const;
    /**
    * @brief Indique si ce widget est activé.
    * @return <code>true</code> si le champ PfWidget::m_widgetStatus a pour valeur PfWidget::WIDGET_ACTIVATED ou une valeur de priorité supérieure.
    */
    bool isActivated() const;
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Met à jour ce ModelItem.
    *
    * Appelle la méthode AnimatedGLItem::update.
    *
    * Associe une animation à l'état actuel de ce widget, en fonction de la map PfWidget::m_animation_map.
    * Si un état n'est pas présent dans cette map, l'animation ANIM_IDLE est utilisée par défaut.
    *
    * Si le booléen PfWidget::m_justChangedState est à l'état <code>true</code>, alors l'animation est modifiée avec l'option 'reset'.
    * Le booléen PfWidget::m_justChangedState passe à l'état <code>false</code>.
    *
    * @see AnimatedGLItem::changeCurrentAnimation
    */
    virtual void update();
    /**
    * @brief Génère un Viewable à partir de ce ModelItem.
    * @return Le Viewable créé.
    *
    * A ce niveau, cette méthode gère l'affichage du texte si ce widget en a un.
    *
    * @warning
    * De la mémoire est allouée pour le pointeur retourné.
    */
    virtual Viewable* generateViewable() const;
    /**
    * @brief Traite l'instruction passée en paramètre.
    * @return RETURN_NOTHING par défaut, RETURN_OK s'il y a eu sélection ou activation.
    *
    * Si l'instruction est INSTRUCTION_ACTIVATE (fichier "enum.h", bibliothèque PfMisc), alors active ce widget.
    *
    * Si l'instruction est INSTRUCTION_SELECT, alors sélectionne ce widget.
    */
    virtual PfReturnCode processInstruction();
    /*
    * Accesseurs
    * ----------
    */
    pfflag32 getEffectFlags() const {return m_effectFlags;} //!< Accesseur.
    const PfPoint& getPoint() const {return m_point;} //!< Accesseur.
    void setPoint(const PfPoint& point) {m_point = point;} //!< Accesseur.
    PfWidgetStatus getWidgetStatus() const {return m_widgetStatus;} //!< Accesseur.
    bool isRepeatingSelection() const {return m_repeatingSelection;} //!< Accesseur.

protected:
    /**
    * @brief Change l'état de ce widget.
    * @param status Le nouvel état à prendre en compte.
    * @param onlyHigherRank <code>true</code> pour qu'un état de priorité inférieure ne soit pas assigné.
    *
    * L'ordre de priorité des états est l'ordre de l'énumération PfWidget::PfWidgetStatus.
    *
    * Le booléen PfWidget::m_justChangedState passe à l'état vrai s'il y a changement effectif d'état.
    */
    void changeStatus(PfWidgetStatus status, bool onlyHigherRank = true);

    bool m_repeatingSelection; //!< Indique si ce widget doit répéter sa sélection dans le cadre de la gestion par layout.

private:
    /**
    * @brief Méthode appelée par les constructeurs pour associer les animations aux statuts de ce widget en fonction d'une valeur selon un jeu par défaut.
    * @param statusMap La valeur d'initialisation de la map d'association animation <-> état de ce widget.
    */
    void defineAnimationMap(PfWidgetStatusMap statusMap);

    pfflag32 m_effectFlags; //!< Les effets de ce widget.
    PfPoint m_point; //!< Le point d'action sur ce widget.
    PfWidgetStatus m_widgetStatus; //!< L'état de ce widget.
    bool m_justChangedState; //!< Indique si ce widget vient de changer de statut (disponible, sélectionné ou activé).
    map<PfWidgetStatus, PfAnimationStatus> m_animation_map; //!< La map d'association animation <-> état de ce widget.
    PfText m_text; //!< Le texte de ce label.
};

#endif // PFWIDGET_H_INCLUDED

/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe PfList.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef PFLIST_H_INCLUDED
#define PFLIST_H_INCLUDED

#include "glgame_gen.h"

#include "pfwidget.h"
#include "pfbutton.h"

/**
* @brief Liste de boutons, munie de flèches de défilement.
*
* Trois principales zones sensibles sont présentes sur ce widget. Les deux flèches de défilement et la zone des boutons.
*
* Les indices des boutons commencent à 1, 0 pour aucune sélection.
* L'indice PfList::m_currentIndex indique l'indice du bouton sélectionné, tandis que l'indice PfList::m_firstButtonIndex indique l'indice de la liste de
* textes du premier bouton.
*
* Il est possible pour une liste d'avoir plus de textes que de boutons, d'où l'intérêt du champ PfList::m_firstButtonIndex. Les flèches de défilement
* font varier ce champ pour parcourir la liste complète de textes.
*
* @section AnimDef Définition des animations dans le wad
*
* Cet objet possède une liste de boutons, dont les animations sont définies dans le wad.
* Les images des flèches de défilement sont situées dans le même fichier image que les boutons, dans la case supérieure gauche (pour le statut ANIM_IDLE)
* et la case suivante (pour les autres statuts).
* Les boutons, eux, doivent avoir une texture rectangulaire prenant deux cases horizontales et une case verticale, dans le même fichier image,
* en-dessous des flèches.
*
* Dans le script du wad, ce sont les textures des boutons qui sont visées lors de la définition des frames.
* L'image des flèches est automatiquement prise dans le même fichier image, selon la description du paragraphe ci-dessus.
* Tout ceci est géré par la redéfinition de la méthode PfList::addAnimation dans cette classe.
*
* @remarks
* Le champ PfWidget::m_repeatingSelection de ce widget est à l'état <code>true</code> pour changer la sélection (illumination des boutons)
* en fonction du déplacement de la souris au sein du même widget.
*/
class PfList : public PfWidget
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur PfList.
    * @param name Le nom de cette liste.
    * @param x L'abscisse de cette liste.
    * @param y L'ordonnée de cette liste.
    * @param w La largeur de cette liste.
    * @param buttonSize La hauteur de chaque bouton de cette liste.
    * @param rows Le nombre de lignes de cette liste (nombre de boutons).
    * @param layer Le plan de perspective de ce widget.
    * @param hMargin La marge horizontale du texte des boutons.
    * @param vMargin La marge verticale du texte des boutons.
    * @param statusMap La valeur d'initialisation de la map d'association animation <-> état de ce widget.
    * @throw ConstructorException si une erreur survient lors de la construction de cet objet.
    *
    * @warning
    * Le nombre total de lignes ne doit pas dépasser 255, sinon une exception est levée (et ça ne doit pas tenir dans l'écran de toute façon...)
    * Ceci afin de gérer la compatibilité avec les PfEffect (8 premiers bits pour une valeur égale à l'indice du bouton).
    */
    PfList(const string& name, float x, float y, float w, float buttonSize, unsigned int rows, unsigned int layer = MAX_LAYER,
           float hMargin = 0.0, float vMargin = 0.0, PfWidget::PfWidgetStatusMap statusMap = WIDGET_STANDARD_MAP);
    /**
    * @brief Destructeur PfList.
    *
    * Détruit les boutons de cette liste.
    */
    virtual ~PfList();
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Fait défiler d'un cran cette liste.
    * @param down <code>true</code> si le défilement se fait vers le bas.
    *
    * Si l'indice actuel est 0, alors rien n'est fait.
    *
    * Si la sélection actuelle est en milieu de liste de boutons, elle est modifiée sans toucher à l'affichage des boutons.
    * Autrement, les textes changent sur les boutons pour continuer à parcourir la liste de textes.
    *
    * Le champ PfList::m_currentIndex est modifié.
    *
    * Quoi qu'il arrive (indice à 0 ou non), ce widget est sélectionné en fin de méthode (PfList::select)
    * et le point d'activation est mis à (0.0;0.0).
    * A cette valeur, le champ PfList::m_currentIndex préalablement modifié permettra de mettre à jour le bouton surligné sans être parasité par la souris.
    *
    * @remarks
    * Ce ModelItem est marqué modifié.
    */
    void scroll(bool down);
    /**
    * @brief Ajoute un texte à cette liste.
    * @param text Le texte à ajouter.
    * @throw PfException si la liste de textes contient déjà 255 valeurs.
    */
    void addText(const string& text);
    /**
    * @brief Ajoute une liste de textes à cette liste.
    * @param texts_v La liste de textes.
    * @throw PfException si la liste de textes contiendrait plus de 255 valeurs après ajout.
    */
    void addTexts(const vector<string>& texts_v);
    /**
    * @brief Retourne le texte du bouton actuellement sélectionné.
    * @return Le texte actuellement sélectionné.
    * @throw PfException si l'indice actuel est 0.
    */
    const string& currentText() const;
    /**
    * @brief Retourne l'indice du texte du bouton sélectionné.
    * @return L'indice du texte, 0 s'il n'y a aucune sélection.
    */
    unsigned int currentTextIndex() const;
    /**
    * @brief Supprime un élément de cette liste.
    * @param index L'indice de suppression, en base 1.
    * @throw ArgumentException si l'indice n'est pas valide.
    */
    void removeText(unsigned int index);
    /**
    * @brief Supprime tous les textes de cette liste.
    */
    void clear();
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Surligne ce widget.
    *
    * Appelle la méthode PfWidget::highlight.
    *
    * Surligne le bouton sous la souris (principaux et flèches) et quitte tous les autres sauf le bouton actuellement sélectionné.
    * Si aucun bouton n'est sous la souris, le bouton correspondant à l'indice PfList::m_currentIndex est surligné. A défaut, le premier est surligné.
    */
    virtual void highlight();
    /**
    * @brief Désurligne ce widget.
    *
    * Appelle la méthode PfWidget::leave.
    *
    * Quitte tous les boutons de ce widget (principaux et flèches).
    */
    virtual void leave();
    /**
    * @brief Action à réaliser lorsque ce widget est sélectionné.
    *
    * Appelle la méthode PfWidget::select.
    *
    * Appelle la méthode PfWidget::select sur le bouton sous la souris ou les quitte tous (méthode PfWidget::deselect).
    * Si le point d'activation est (0, 0), alors le bouton activé est celui correspondant à PfList::m_currentIndex s'il n'est pas nul,
    * sinon le premier bouton.
    *
    * Le champ PfList::m_currentIndex est modifié selon les coordonnées de la souris.
    *
    * A défaut de champ PfList::m_currentIndex valide ou d'un positionnement de souris adéquat, le premier bouton disponible est sélectionné.
    *
    * @remarks
    * Ce ModelItem est marqué modifié.
    */
    virtual void select();
    /**
    * @brief Action à réaliser lorsque ce widget perd l'état surligné ou activé.
    *
    * Appelle la méthode PfWidget::deselect.
    *
    * Appelle la méthode PfWidget::deselect tous les boutons.
    * PfList::m_currentIndex passe à 0.
    *
    * @remarks
    * Ce ModelItem est marqué modifié.
    */
    virtual void deselect();
    /**
    * @brief Action à réaliser lorsque ce widget est activé.
    *
    * Appelle la méthode PfWidget::activate.
    *
    * Si le point d'activation est sur une flèche, alors la liste défile.
    * Si c'est un bouton, alors la sélection est faite et l'effet de ce widget prend la valeur EFFECT_SELECT ainsi qu'une valeur correspondant à l'indice sélectionné.
    * Si le point d'activation est (0;0), alors il n'est pas pris en compte et l'activation se fait sur le bouton correspondant à l'indice PfList::m_currentIndex.
    * Rien ne se fait dans ce dernier cas si l'indice est nul.
    *
    * @remarks
    * Ce ModelItem est marqué modifié.
    */
    virtual void activate();
    /**
    * @brief Ajoute une animation aux boutons de cette grille.
    * @param p_anim L'animation à ajouter.
    * @param value Le statut d'animation associé.
    * @throw PfException si une exception survient lors de l'ajout.
    *
    * Si une animation existe déjà pour ce statut, elle est remplacée.
    *
    * Le pointeur à ajouter ne doit pas être nul. Ce pointeur sera
    * détruit par le destructeur du PfAnimationGroup de cette classe.
    *
    * Appelle la méthode AnimatedGLItem::addAnimation sur chaque bouton de cette liste.
    *
    * Si l'animation passée en paramètre est ANIM_IDLE, l'animation ANIM_IDLE des flèches de défilement est crée à partir du même fichier texture (coin supérieur gauche).
    * Si c'est ANIM_HIGHLIGHTED, l'animation ANIM_HIGHLIGHTED est ajoutée aux flèches de défilement (deuxième carré de texture).
    *
    * L'animation passée en paramètre est copiée puis détruite.
    */
    virtual void addAnimation(PfAnimation* p_anim, PfAnimationStatus value);
    /**
    * @brief Met à jour cette liste.
    *
    * Appelle la méthode PfWidget::update sur chaque bouton de cette liste et sur les flèches.
    */
    virtual void update();
    /**
    * @brief Génère un Viewable à partir de ce ModelItem.
    * @return Le Viewable créé.
    *
    * @warning
    * De la mémoire est allouée pour le pointeur retourné.
    */
    virtual Viewable* generateViewable() const;
    /**
    * @brief Traite l'instruction passée en paramètre.
    * @return RETURN_OK si l'instruction a été traitée, RETURN_NOTHING sinon.
    *
    * <ul><li>INSTRUCTION_UP : monte la sélection d'un cran par appel à la méthode PfList::scroll.</li>
    * <li>INSTRUCTION_DOWN : descend la sélection d'un cran par appel à la méthode PfList::scroll.</li>
    * <li>INSTRUCTION_ACTIVATE : active la sélection.</li></ul>
    */
    virtual PfReturnCode processInstruction();
    /*
    * Accesseurs
    * ----------
    */
    unsigned int getCurrentIndex() const {return m_currentIndex;} //!< Accesseur.
    void setCurrentIndex(unsigned int index) {m_currentIndex = index;} //!< Accesseur.

private:
    vector<PfWidget*> mp_buttons_v; //!< La liste des boutons de cette liste.
    vector<string> m_texts_v; //!< La liste des textes de cette liste.
    PfWidget* mp_upArrow; //!< La flèche de défilement haute.
    PfWidget* mp_downArrow; //!< La flèche de défilement basse.
    unsigned int m_currentIndex; //!< L'indice sélectionné dans cette liste.
    unsigned int m_firstButtonIndex; //!< L'indice de cette liste correspondant au premier bouton.
};

#endif // PFLIST_H_INCLUDED

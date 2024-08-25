/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe PfScrollBar.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef PFSCROLLBAR_H_INCLUDED
#define PFSCROLLBAR_H_INCLUDED

#include "glgame_gen.h"

#include "pfwidget.h"
#include "pfbutton.h"

/**
* @brief Barre de défilement.
*
* Ce widget contient trois zones sensibles : les deux boutons flèches et la barre elle-même, qui peut être tirée le long du rail.
*
* L'indice des pas de la barre va de 0 au nombre maximal moins 1.
*
* L'effet des boutons est EFFECT_PREV et EFFECT_NEXT  pour respectivement le premier et le deuxième bouton, avec pour valeur le nombre de pas à effectuer.
*
* @warning
* Une barre ne peut pas avoir plus de 256 pas, pour la gestion des effets.
*
* @section AnimDef Définition des animations dans le wad
*
* Dans un wad, les animations concernent les boutons de cette barre de défilement.
* La barre elle-même est toujours générée au moyen du fichier texture dans lequel se trouvent les informations des boutons,
* en utilisant, pour le rail, la case supérieure gauche, et pour l'ascenseur, la case suivante (sens horizontal ou vertical à défaut).
*
* Pour le calcul des coordonnées de texture des éléments de la barre, il est nécessaire que l'image des boutons soit de dimensions carrées.
*
* Dans le script du wad, ce sont les textures des boutons flèches (vers le bas) qui sont visées lors de la définition des frames.
* Les images de l'ascenseur et du rail sont automatiquement prises dans le même fichier image, selon la description du paragraphe ci-dessus.
*
* Si les boutons doivent être réorientés vers le haut, la gauche ou la droite, c'est le programme qui s'en charge par la rotation nécessaire.
* Le programme considère toujours que l'image de base pointe vers le bas.
*/
class PfScrollBar : public PfWidget
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur PfScrollBar.
    * @param name Le nom de ce ModelItem.
    * @param barCoordRect Les coordonnées de la barre.
    * @param buttonW La largeur des boutons.
    * @param buttonH La hauteur des boutons.
    * @param steps Le nombre de pas pour l'ascenseur.
    * @param vertical <code>true</code> si cette barre est verticale.
    * @param layer Le plan de perspective de ce widget.
    * @throw ConstructorException si une erreur survient.
    */
    PfScrollBar(const string& name, const PfRectangle& barCoordRect, float buttonW, float buttonH, unsigned int steps, bool vertical = true, unsigned int layer = MAX_LAYER);
    /**
    * @brief Destructeur PfScrollBar.
    *
    * Détruit les éléments de cette barre.
    */
    virtual ~PfScrollBar();
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Change le pas actuel.
    * @param step Le nouveau pas.
    * @throw ArgumentException si le pas n'est pas valide.
    *
    * Cette méthode met à jour le polygone de position de la barre.
    *
    * Ce ModelItem est marqué modifié.
    */
    void changeStep(unsigned int step);
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Surligne ce widget.
    *
    * Appelle la méthode PfWidget::highlight.
    *
    * Surligne l'un des deux boutons de défilement de cette barre s'ils sont sur le point d'activation.
    */
    virtual void highlight();
    /**
    * @brief Désurligne ce widget.
    *
    * Appelle la méthode PfWidget::leave.
    *
    * Quitte tous les boutons.
    */
    virtual void leave();
    /**
    * @brief Sélectionne ce widget.
    *
    * Appelle la méthode PfWidget::select.
    *
    * Appelle la méthode PfWidget::select sur l'un des boutons de cette barre si le point d'activation est sur l'un d'eux.
    * Si le point n'est sur aucun bouton, la méthode PfWidget::deselect est appelée sur les deux boutons.
    */
    virtual void select();
    /**
    * @brief Désélectionne ce widget.
    *
    * Appelle la méthode PfWidget::deselect.
    *
    * Appelle la méthode PfWidget::deselect sur les deux boutons de cette barre. Passe le booléen PfScrollBar::m_readyToSlide à l'état <code>false</code>.
    */
    virtual void deselect();
    /**
    * @brief Active ce widget.
    *
    * Appelle la méthode PfWidget::activate.
    *
    * Si le point d'activation est sur la barre, efface tous les effets de ce widget et passe le booléen PfWidget::m_readyToSlide à l'état <code>true</code>.
    * Sinon, passe les effets de ce widget à la valeur indiquée par le bouton cliqué si le point d'activation est sur l'un d'eux. Enfin, si le point d'activation n'est sur
    * aucune zone sensible, efface simplement les effets de ce widget.
    */
    virtual void activate();
    /**
    * @brief Action à réaliser lorsque ce widget subit un glisser-déplacer.
    * @return <code>false</code> si aucun déplacement n'a lieu.
    *
    * Si PfScrollBar::m_readyToSlide est à l'état <code>true</code>, alors fait glisser la barre de l'ascenseur.
    * L'effet de ce widget passe à EFFECT_NEXT ou EFFECT_PREV avec pour valeur le nombre de pas à effectuer,
    * déterminé au moyen du point d'action enregistré par ce widget.
    *
    * @remarks
    * Ce ModelItem est marqué modifié si un déplacement a lieu.
    */
    virtual bool slide();
    /**
    * @brief Ajoute une animation aux boutons de cette barre.
    * @param p_anim L'animation à ajouter.
    * @param value Le statut de l'animation.
    * @throw PfException si une exception survient lors de l'ajout.
    *
    * Si une animation existe déjà pour ce statut, elle est remplacée.
    *
    * Le pointeur à ajouter ne doit pas être nul. Ses copies seront détruites par le destructeur du PfAnimationGroup de cette classe.
    *
    * Appelle la méthode AnimatedGLItem::addAnimation sur chaque bouton de cette barre.
    *
    * L'animation ANIM_IDLE de la barre principale et du rail est créée si la valeur du paramètre value est ANIM_IDLE.
    * Cette animation est construite avec le même indice de texture, comme indiqué dans la description de cette classe.
    *
    * L'animation passée en paramètre est copiée puis détruite.
    */
    virtual void addAnimation(PfAnimation* p_anim, PfAnimationStatus value);
    /**
    * @brief Met à jour cette barre de défilement.
    *
    * Appelle la méthode PfWidget::update.
    *
    * Met à jour la position précédente de cette barre avec la valeur de la position actuelle.
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
    /*
    * Accesseurs
    * ----------
    */
    unsigned int getStepsCount() const {return m_stepsCount;} //!< Accesseur.
    void setStepsCount(unsigned int count) {m_stepsCount = count;} //!< Accesseur.
    bool isReadyToSlide() const {return m_readyToSlide;} //!< Accesseur.
    void setReadyToSlide(bool slide) {m_readyToSlide = slide;} //!< Accesseur.

private:
    unsigned int m_stepsCount; //!< Le nombre de pas pour l'ascenseur.
    bool m_vertical; //!< Indique si cette barre est verticale (sinon horizontale).
    PfWidget* mp_firstButton; //!< Bouton haut ou gauche.
    PfWidget* mp_secondButton; //!< Bouton bas ou droit.
    AnimatedGLItem* mp_rail; //!< Le rail de l'ascenseur.
    AnimatedGLItem* mp_bar; //!< La barre de l'ascenseur.
    unsigned int m_currentStep; //!< Le pas actuel.
    unsigned int m_previousStep; //!< Le pas précédent (pour gérer le slide).
    bool m_readyToSlide; //!< Indique si la barre accepte le "slide".
};

#endif // PFSCROLLBAR_H_INCLUDED

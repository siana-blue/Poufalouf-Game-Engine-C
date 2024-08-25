/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe GLItem et ses dérivées : PolygonGLItem, RectangleGLItem et AnimatedGLItem.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef GLITEM_H_INCLUDED
#define GLITEM_H_INCLUDED

#include "glgame_gen.h"
#include <string>
#include "modelitem.h"
#include "geometry.h"
#include "animation.h"

class Viewable;

/**
* @brief ModelItem générant un Viewable doté d'une GLImage.
*
* Il s'agit d'une classe abstraite. Les classes filles définiront un élément permettant d'afficher une image (comme un polygone).
*
* @section Clignotement Clignotement
*
* Le clignotement d'un objet est géré dans cette classe.
* Il est défini par une vitesse, déterminant le nombre de frames pour parcourir la gamme GLItem::m_color à blanc ou inversement.
* La couleur GLItem::m_color est copiée dans GLItem::m_blinkingColor lorsque le clignotement démarre grâce à la méthode GLItem::blink.
* Ensuite, c'est cette couleur qui est modifiée pour être prise en compte par le Viewable.
* Pour savoir où en est le clignotement, le membre GLItem::m_blinkingTick est utilisé. Positif pour la montée vers la couleur, négatif pour la descente
* vers le blanc. Sa valeur absolue ne dépasse jamais la vitesse GLItem::m_blinkingSpeed.
*/
class GLItem : public ModelItem
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur GLItem par défaut.
    * @param name Le nom de ce GLItem.
    * @param layer Le plan de perspective de ce GLItem.
    * @param color La couleur de ce GLItem.
    * @param line Indique si le rendu OpenGL se fait sous forme de lignes.
    * @param angle L'angle à appliquer au Viewable généré.
    *
    * Crée un GLItem dont l'image n'est pas texturée.
    */
    GLItem(const string& name = "", int layer = 0, const PfColor& color = PfColor::WHITE, bool line = false, double angle = 0.0);
    /**
    * @brief Constructeur GLItem 1.
    * @param name Le nom de ce GLItem.
    * @param layer Le plan de perspective de ce GLItem.
    * @param textureIndex L'indice de la texture de ce ModelItem dans le wad.
    * @param color La couleur de ce GLItem.
    * @param angle L'angle à appliquer au Viewable généré.
    *
    * Crée un ModelItem dont l'image est texturée.
    */
    GLItem(const string& name, int layer, unsigned int textureIndex, const PfColor& color = PfColor::WHITE, double angle = 0.0);
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Fait clignoter ce GLItem.
    * @param speed La vitesse de clignotement, 0 pour annuler le clignotement.
    * @param color La couleur de base de cet objet.
    *
    * La vitesse de clignotement est le nombre de frames nécessaires pour faire une transition complète de blanc
    * à la couleur GLItem::m_color.
    *
    * Le champ GLItem::m_color est modifié pour prendre comme valeur le paramètre <em>color</em>.
    *
    * @remarks
    * Ce ModelItem est marqué modifié.
    */
    void blink(unsigned int speed = 0, PfColor color = PfColor::WHITE);
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Met à jour ce ModelItem.
    *
    * Gère le clignotement. S'il y a clignotement, ce ModelItem est toujours marqué modifié.
    */
    virtual void update();
    /*
    * Accesseurs
    * ----------
    */
    int getLayer() const {return m_layer;} //!< Accesseur.
    void setLayer(int layer) {m_layer = layer;} //!< Accesseur.
    unsigned int getTextureIndex() const {return m_textureIndex;} //!< Accesseur.
    void setTextureIndex(unsigned int index) {m_textureIndex = index;} //!< Accesseur.
    const PfColor& getColor() const {return m_color;} //!< Accesseur.
    void setColor(const PfColor& color) {m_color = color;} //!< Accesseur.
    bool isLine() const {return m_line;} //!< Accesseur.
    bool isCoordRelativeToBorder() const {return m_coordRelativeToBorder;} //!< Accesseur.
    void setCoordRelativeToBorder(bool b) {m_coordRelativeToBorder = b;} //!< Accesseur.
    bool isStatic() const {return m_static;} //!< Accesseur.
    void setStatic(bool b) {m_static = b;} //!< Accesseur.
    double getAngle() const {return m_angle;} //!< Accesseur.

private:
    int m_layer; //!< Le plan de perspective de ce ModelItem (de son Viewable).
    unsigned int m_textureIndex; //!< L'indice de texture de ce GLItem (indice de wad).
    PfColor m_color; //!< La couleur de ce ModelItem.
    bool m_line; //!< Indique si le rendu OpenGL se fait sous forme de lignes.
    bool m_coordRelativeToBorder; //!< Indique si les coordonnées de cet objet sont relatives aux bordures et non aux bords de l'écran.
    bool m_static; //!< Indique si les coordonnées de cet objet sont indépendantes de la caméra.
    double m_angle; //!< L'angle du Viewable généré.
    unsigned int m_blinkingSpeed; //!< La vitesse de clignotement de cet objet (de blanc à la couleur GLItem::m_color), 0 pour aucun clignotement.
    PfColor m_blinkingColor; //!< Stocke la couleur de départ lors d'un clignotement.
    int m_blinkingTick; //!< Le pas actuel de clignotement.
};

/**
* @brief GLItem dont l'image est définie par un polygone.
*/
class PolygonGLItem : public GLItem
{
public:
    #define poly_minX getPolygon().minX //!< Macro simplificatrice.
    #define poly_maxX getPolygon().maxX //!< Macro simplificatrice.
    #define poly_minY getPolygon().minY //!< Macro simplificatrice.
    #define poly_maxY getPolygon().maxY //!< Macro simplificatrice.
    #define poly_maxW getPolygon().maxW //!< Macro simplificatrice.
    #define poly_maxH getPolygon().maxH //!< Macro simplificatrice.

    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur PolygonGLItem par défaut.
    * @param name Le nom de ce GLItem.
    * @param layer Le plan de perspective de ce GLItem.
    * @param polygon Le polygone de l'image de ce GLItem.
    * @param color La couleur de ce GLItem.
    * @param line Indique si le rendu OpenGL se fait sous forme de lignes.
    * @param angle L'angle à appliquer au Viewable généré.
    *
    * Crée un GLItem dont l'image n'est pas texturée.
    */
    PolygonGLItem(const string& name = "", int layer = 0, const PfPolygon& polygon = PfPolygon(), const PfColor& color = PfColor::WHITE, bool line = false, double angle = 0.0);
    /**
    * @brief Constructeur PolygonGLItem 1.
    * @param name Le nom de ce GLItem.
    * @param layer Le plan de perspective de ce GLItem.
    * @param polygon Le polygone de l'image de ce GLItem.
    * @param textureIndex L'indice de la texture de ce GLItem dans le wad.
    * @param coordPolygon Le polygone des coordonnées de texture de ce GLItem.
    * @param color La couleur de ce GLItem.
    * @param angle L'angle à appliquer au Viewable généré.
    *
    * Crée un ModelItem dont l'image est texturée.
    */
    PolygonGLItem(const string& name, int layer, const PfPolygon& polygon, unsigned int textureIndex, const PfPolygon& coordPolygon, const PfColor& color = PfColor::WHITE, double angle = 0.0);
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Déplace l'image de ce GLItem dans la direction passée en paramètre.
    * @param orientation L'orientation du déplacement.
    * @param scale L'échelle de déplacement.
    *
    * Le déplacement se fait d'une unité multipliée par l'échelle de déplacement, pour chaque unité de l'orientation.
    *
    * @remarks
    * Ce ModelItem est marqué modifié.
    */
    void shift(const PfOrientation& orientation, double scale = 1);
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Génère un Viewable à partir de ce ModelItem.
    * @return Le Viewable créé.
    * @throw ViewableException si une erreur survient lors de la génération du Viewable.
    *
    * @warning
    * De la mémoire est allouée pour le pointeur retourné.
    */
    virtual Viewable* generateViewable() const;
    /*
    * Accesseurs
    * ----------
    */
    const PfPolygon& getPolygon() const {return m_polygon;} //!< Accesseur.
    void setPolygon(const PfPolygon& poly) {m_polygon = poly;} //!< Accesseur.
    const PfPolygon& getTextCoordPolygon() const {return m_textCoordPolygon;} //!< Accesseur.
    void setTextCoordPolygon(const PfPolygon& poly) {m_textCoordPolygon = poly;} //!< Accesseur.

private:
    PfPolygon m_polygon; //!< Le polygone donnant la forme de ce ModelItem.
    PfPolygon m_textCoordPolygon; //!< Les coordonnées de texture.
};

/**
* @brief GLItem dont le polygone est un rectangle.
*
* Ce type d'objet est utilisé pour les objets du jeu, notamment ceux dont il faut accéder rapidement aux dimensions x,y,w,h, pour des calculs de
* collision par exemple.
*
* Le polygone de cet objet est un rectangle, dont le point (x;y) est le point inférieur gauche, et premier point du polygone.
*
* Des macros simplificatrices permettent un accès rapide aux dimensions du rectangle.
*/
class RectangleGLItem : public GLItem
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur RectangleGLItem par défaut.
    * @param name Le nom de ce GLItem.
    * @param layer Le plan de perspective de ce GLItem.
    * @param rectangle Le rectangle des coordonnées de ce GLItem.
    * @param color La couleur de ce GLItem.
    * @param line <code>true</code> si l'image doit être sous forme de lignes.
    *
    * Crée un GLItem dont l'image n'est pas texturée, par appel au constructeur GLItem par défaut.
    */
    RectangleGLItem(const string& name = "", int layer = 0, const PfRectangle& rectangle = PfRectangle(), const PfColor& color = PfColor::WHITE, bool line = false);
    /**
    * @brief Constructeur RectangleGLItem 1.
    * @param name Le nom de ce GLItem.
    * @param layer Le plan de perspective de ce GLItem.
    * @param rectangle Le rectangle des coordonnées de ce GLItem.
    * @param textureIndex L'indice de la texture de ce ModelItem dans le wad.
    * @param coordRectangle Le rectangle des coordonnées de la texture de ce ModelItem.
    * @param color La couleur de ce GLItem.
    *
    * Crée un GLItem dont l'image est texturée, par appel au constructeur GLItem 1.
    */
    RectangleGLItem(const string& name, int layer, const PfRectangle& rectangle, unsigned int textureIndex, const PfRectangle& coordRectangle = PfRectangle(1.0, 1.0),
                    const PfColor& color = PfColor::WHITE);
    /**
    * @brief Destructeur RectangleGLItem.
    */
    virtual ~RectangleGLItem() {}
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Déplace l'image de ce GLItem dans la direction passée en paramètre.
    * @param orientation L'orientation du déplacement.
    * @param scale L'échelle de déplacement.
    *
    * Le déplacement se fait d'une unité multipliée par l'échelle de déplacement, pour chaque unité de l'orientation.
    *
    * @remarks
    * Ce ModelItem est marqué modifié.
    */
    void shift(const PfOrientation& orientation, double scale);
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Génère un Viewable à partir de ce ModelItem.
    * @return Le Viewable créé.
    * @throw ViewableException si une erreur survient lors de la génération du Viewable.
    *
    * @warning
    * De la mémoire est allouée pour le pointeur retourné.
    */
    virtual Viewable* generateViewable() const;
    /*
    * Accesseurs
    * ----------
    */
    const PfRectangle& getRect() const {return m_rect;} //!< Accesseur.
    void setRect(const PfRectangle& rect) {m_rect = rect;} //!< Accesseur.
    const PfRectangle& getTextCoordRect() const {return m_textCoordRect;} //!< Accesseur.
    void setTextCoordRect(const PfRectangle& rect) {m_textCoordRect = rect;} //!< Accesseur.
    #define rect_x getRect().getX //!< Macro simplificatrice.
    #define rect_y getRect().getY //!< Macro simplificatrice.
    #define rect_w getRect().getW //!< Macro simplificatrice.
    #define rect_h getRect().getH //!< Macro simplificatrice.

private:
    PfRectangle m_rect; //!< Le rectangle de cet objet.
    PfRectangle m_textCoordRect; //!< Le rectangle des coordonnées de texture de cet objet.
};

/**
* @brief GLItem possdédant une ou plusieurs animations.
*
* Il s'agit d'un RectangleGLItem car les animations ne supportent que des coordonnées rectangulaires.
*
* Lors du rendu de ce GLItem, le Viewable ne prend plus en compte les informations des membres GLItem::m_textureIndex et GLItem::m_textCoordPolygon.
* Ce sont les informations de la frame de l'animation en cours qui sont utilisées.
*
* Si la frame en cours a une couleur différente de PfColor::WHITE, alors c'est celle-ci qui est choisie, sinon, c'est la couleur du GLItem qui est choisie.
*/
class AnimatedGLItem : public RectangleGLItem
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur AnimatedGLItem par défaut.
    * @param name Le nom de ce GLItem.
    * @param layer Le plan de perspective de ce GLItem.
    * @param rectangle Le rectangle des coordonnées de ce GLItem.
    * @param color La couleur de ce GLItem.
    *
    * Crée un GLItem dont l'image n'est pas texturée, par appel au constructeur GLItem par défaut.
    *
    * Une animation à une frame est ajoutée en tant qu'animation ANIM_IDLE.
    * Cette animation n'a pas de texture mais définit une couleur.
    *
    * L'animation en cours est ANIM_IDLE.
    */
    AnimatedGLItem(const string& name = "", int layer = 0, const PfRectangle& rectangle = PfRectangle(), const PfColor& color = PfColor::WHITE);
    /**
    * @brief Constructeur AnimatedGLItem 1.
    * @param name Le nom de ce GLItem.
    * @param layer Le plan de perspective de ce GLItem.
    * @param rectangle Le rectangle des coordonnées GLItem.
    * @param textureIndex L'indice de la texture de ce ModelItem dans le wad.
    * @param soundIndex L'indice du son à associer à la première frame (0 pour aucun son).
    * @param coordRectangle Le rectangle des coordonnées de la texture de ce ModelItem.
    * @param color La couleur de ce GLItem.
    *
    * Crée un GLItem dont l'image est texturée, par appel au constructeur GLItem 1.
    *
    * Une animation à une frame est ajoutée en tant qu'animation ANIM_IDLE.
    *
    * L'animation en cours est ANIM_IDLE.
    */
    AnimatedGLItem(const string& name, int layer, const PfRectangle& rectangle, unsigned int textureIndex, unsigned int soundIndex = 0,
                    const PfRectangle& coordRectangle = PfRectangle(1.0, 1.0), const PfColor& color = PfColor::WHITE);
    /**
    * @brief Constructeur AnimatedGLItem 2.
    * @param name Le nom de ce GLItem.
    * @param layer Le plan de perspective de ce GLItem.
    * @param rectangle Le rectangle des coordonnées GLItem.
    * @param p_anim L'animation ANIM_IDLE à créer.
    *
    * Crée un GLItem dont l'image est texturée.
    *
    * L'animation est ajoutée en tant qu'animation ANIM_IDLE.
    * Elle sera détruite par le destructeur du PfAnimationGroup de ce GLItem.
    *
    * L'animation en cours est ANIM_IDLE.
    */
    AnimatedGLItem(const string& name, int layer, const PfRectangle& rectangle, PfAnimation* p_anim);
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Ajoute une animation pour ce GLItem.
    * @param p_anim L'animation à ajouter.
    * @param value Le statut de l'animation.
    * @throw PfException si une exception survient lors de l'ajout.
    *
    * Si une animation existe déjà pour ce status, elle est remplacée.
    *
    * Le pointeur à ajouter ne doit pas être nul. Ce pointeur sera détruit par le destructeur du PfAnimationGroup de cette classe.
    *
    * @remarks
    * Cette méthode est virtuelle car certaines classes héritant de AnimatedGLItem ont besoin
    * d'ajouter une animation à leurs composants et non à elles-mêmes (ex : PfSelectionGrid).
    */
    virtual void addAnimation(PfAnimation* p_anim, PfAnimationStatus value);
    /**
    * @brief Change l'animation en cours.
    * @param value Le statut de l'animation à jouer.
    * @param reset <code>true</code> si l'animation doit être remise à zéro même si elle n'est pas changée.
    * @return <code>false</code> si l'animation n'a pas pu être changée ou si c'était la même.
    *
    * Le booléen ModelItem::m_modified passe à l'état <code>true</code> si l'animation est changée ou remise à zéro.
    *
    * L'animation est remise à zéro, même s'il s'agit de l'animation déjà en cours, sauf si le paramètre <em>reset</em> est <code>false</code> (par défaut).
    * Une animation changée est systématiquement remise à zéro.
    *
    * @remarks
    * Cette méthode est virtuelle car certaines classes héritant de AnimatedGLItem peuvent avoir à gérer d'autres composants quand elles changent
    * d'animation. Par exemple, certains MapObject changent également leurs zones en fonction de l'animation.
    */
    virtual bool changeCurrentAnimation(PfAnimationStatus value, bool reset = false);
    /**
    * @brief Retourne le statut de l'animation en cours.
    * @return Le statut de l'animation en cours.
    */
    PfAnimationStatus currentAnimationStatus() const;
    /**
    * @brief Change l'orientation de cet objet.
    * @param orientation La nouvelle orientation.
    * @return <code>true</code> si l'orientation a effectivement changé.
    *
    * @remarks
    * Ce ModelItem est marqué modifié si l'orientation a changé.
    */
    bool changeOrientation(PfOrientation::PfOrientationValue orientation);
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Met à jour ce ModelItem.
    *
    * Appelle la méthode GLItem::update.
    *
    * L'animation de cet objet passe à la frame suivante.
    * Si l'animation tourne en boucle, alors c'est ici que la mise à jour des frames est gérée.
    * Sinon, l'animation s'arrête.
    *
    * Le booléen ModelItem::m_modified passe à l'état <code>true</code> s'il y a eu changement de frame.
    */
    virtual void update();
    /**
    * @brief Génère un Viewable à partir de ce ModelItem.
    * @return Le Viewable créé.
    * @throw ViewableGenerationException si le Viewable ne peut être généré.
    *
    * Crée un Viewable à partir du constructeur Viewable 3 prenant en paramètre les membres de ce GLItem ainsi que les informations de la frame actuelle.
    *
    * @warning
    * De la mémoire est allouée pour le pointeur retourné.
    */
    virtual Viewable* generateViewable() const;
    /*
    * Accesseurs
    * ----------
    */
    const PfAnimationGroup& getAnimationGroup() const {return m_animationGroup;} //!< Accesseur.
    PfOrientation::PfOrientationValue getOrientation() const {return m_orientation;} //!< Accesseur.

private:
    PfAnimationGroup m_animationGroup; //!< Les animations de cet objet.
    PfOrientation::PfOrientationValue m_orientation; //!< L'orientation de cet objet.
};

#endif // GLITEM_H_INCLUDED


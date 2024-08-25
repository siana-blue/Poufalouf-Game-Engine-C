/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe PfCamera.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "glgame_gen.h"

#include <string>
#include "geometry.h"

class RectangleGLItem;

/**
* @brief Objet permettant la gestion de la caméra sous OpenGL.
*
* Cette classe est utilisée par une vue OpenGL pour déterminer la position des images d'un modèle à l'écran.
*
* Les coordonnées PfCamera::m_x et PfCamera::m_y indiquent la position de la caméra.
* Il s'agit des valeurs utilisées par la vue, elles sont exprimées dans le repère OpenGL éventuellement corrigé des bordures (c'est à dire en considérant
* que la fenêtre est l'espace d'affichage entre les deux bordures latérales).
*
* Les coordonnées d'une caméra peuvent être exprimées selon différents repères.
* Par défaut, quand le champ PfCamera::mq_targetItem est nul, le repère utilisé est le repère OpenGL éventuellement corrigé des bordures.
* Si une cible objet est définie, alors le repère utilisé est un repère d'échelle égale à l'échelle OpenGL mais de point inférieur gauche choisi de telle
* sorte que l'objet cible soit centré à l'écran quand la caméra est aux coordonnées (0;0).
* Tous les points définissant une caméra sont exprimées selon ces repères, sauf PfCamera::m_x et PfCamera::m_y qui sont toujours absolus.
*
* Le point PfCamera::m_target indique le point visé par la caméra.
* Tant que ce point n'est pas atteint, la caméra se déplace.
* C'est la méthode PfCamera::update qui gère ces déplacements.
*
* La vitesse de déplacement de la caméra est positive ou nulle.
* Une vitesse nulle indique un déplacement instantané.
* Une vitesse positive (nombre entier) s'exprime en pourcentage de la distance totale à parcourir.
* Pour gérer cela, le point PfCamera::m_pin (l'épingle) stocke les coordonnées du point de départ d'un mouvement.
*
* La caméra exprime par défaut ses coordonnées selon le repère entre bordures, ce qui implique une conversion avant appel à la fonction
* <em>translateCamera</em> (fichier "glfunc.h" de la bibliothèque PfMedia).
* Il est possible de spécifier à la caméra d'utiliser le repère absolu, au moyen du membre PfCamera::m_relativeToBorders.
*
* La cible de la caméra est exprimée selon le repère de la caméra, et désigne le point inférieur gauche visible à l'écran.
*
* Une caméra peut avoir des limites fixées par le champ PfCamera::m_limitRect.
* Ces limites empêchent la caméra de fixer sa cible en dehors.
* En pratique, les valeurs de la cible seront bien modifiées, mais au moment de l'affichage, la caméra ne sera pas déplacée.
* Si la largeur ou la hauteur du rectangle de limite est nulle, alors aucune limite n'est prise en compte dans la direction correspondante.
*
* D'autres paramètres sont disponibles, la description des membres correspondants se suffit normalement à elle-même.
*
* @remarks
* PfCamera::m_prev peut laisser dubitatif à première vue. Mais quelle est son utilité ?
* En fait, c'est utile notamment dans le cas du suivi d'un objet, car PfCamera::m_prev stocke l'avancée de la caméra relative à l'objet suivi,
* tandis que PfCamera::m_x et PfCamera::m_y gère les coordonnées absolues.
*/
class PfCamera
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur PfCamera par défaut.
    * @param x L'abscisse de cette caméra.
    * @param y L'ordonnée de cette caméra.
    * @param speed La vitesse de cette caméra.
    * @param borders <code>true</code> si les coordonnées de cette caméra utilisent le repère entre bordures.
    */
    PfCamera(float x = 0.0, float y = 0.0, unsigned int speed = 0, bool borders = true);
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Met à jour la position de cette caméra.
    *
    * Cette caméra est positionnée de façon à centrer un éventuel objet cible PfCamera::mq_targetItem si ce champ est non nul.
    *
    * Puis, le champ PfCamera::m_target est utilisé pour déterminer les déplacements de cette caméra.
    * Si la vitesse de cette caméra est nulle, tout déplacement est instantané.
    * Sinon, l'épingle permet de connaître la distance à parcourir entre la cible et le point de départ et de calculer la distance à parcourir
    * pour faire un pas à la vitesse spécifiée.
    *
    * S'il n'y a pas de déplacement en cours et si PfCamera::m_keepSpeed est <code>false</code>, alors PfCamera::m_speed prend pour valeur 0.
    *
    * Enfin, la fonction <em>translateCamera</em> (fichier "glfunc.h", bibliothèque PfMedia) est appelée.
    * Le rectangle de limite est pris en compte à ce stade.
    */
    void update();
    /**
    * @brief Déplace la caméra au point spécifié.
    * @param x L'abscisse du point d'arrivée.
    * @param y L'ordonnée du point d'arrivée.
    *
    * La vitesse de la caméra n'est pas modifiée.
    *
    * L'épingle et le point PfCamera::m_prev sont fixés à la position actuelle de la caméra.
    */
    void moveAt(float x, float y);
    /**
    * @brief Déplace la caméra au point spécifié.
    * @param x L'abscisse du point d'arrivée.
    * @param y L'ordonnée du point d'arrivée.
    * @param speed La vitesse de déplacement, 0 pour un déplacement instantané.
    * @param keepSpeed <code>true</code> si la vitesse doit être modifiée et conservée après le mouvement.
    *
    * L'épingle et le point PfCamera::m_prev sont fixés à la position actuelle de la caméra.
    */
    void moveAt(float x, float y, unsigned int speed, bool keepSpeed = false);
    /**
    * @brief Déplace la caméra d'une distance spécifiée par les coordonnées.
    * @param x Le déplacement selon x.
    * @param y Le déplacement selon y.
    *
    * La vitesse de la caméra n'est pas modifiée.
    *
    * L'épingle et le point PfCamera::m_prev sont fixés à la position actuelle de la caméra.
    */
    void move(float x, float y);
    /**
    * @brief Déplace la caméra d'une distance spécifiée par les coordonnées.
    * @param x Le déplacement selon x.
    * @param y Le déplacement selon y.
    * @param speed La vitesse de déplacement, 0 pour un déplacement instantané.
    * @param keepSpeed L'épingle et le point PfCamera::m_prev sont fixés à la position actuelle de la caméra. si la vitesse doit être modifiée et conservée après le mouvement.
    *
    * L'épingle et le point PfCamera::m_prev sont fixés à la position actuelle de la caméra.
    */
    void move(float x, float y, unsigned int speed, bool keepSpeed = false);
    /**
    * @brief Fait suivre un objet par cette caméra.
    * @param rc_target L'objet cible.
    *
    * Lors de la mise à jour de cette caméra, celle-ci se placera tout d'abord de façon à centrer l'objet cible,
    * puis subira les éventuels déplacements liés à PfCamera::m_target.
    *
    * En temps normal, PfCamera::m_target a des coordonnées relatives à (0;0).
    * Avec un GLItem comme cible, il faut imaginer cette PfCamera::m_target comme relative à l'objet.
    *
    * @warning
    * Le pointeur PfCamera::mq_targetItem pointe vers le GLItem passé en paramètre.
    * Si cet objet est détruit, une erreur de segmentation peut survenir (et n'y manquera certainement pas...)
    */
    void follow(const RectangleGLItem& rc_target);
    /**
    * @brief Annule la cible suivie.
    *
    * Le champ PfCamera::mq_targetItem pointe vers 0.
    */
    void stopFollowing();
    /**
    * @brief Modifie les limites de cette caméra.
    * @param rect Le rectangle de limite.
    */
    void changeLimit(const PfRectangle& rect);
    /**
    * @brief Retourne les coordonnées du point inférieur gauche du viewport vu de cette caméra.
    * @return Les coordonnées du viewport.
    *
    * Si cette caméra exprime ses coordonnées selon le repère entre bordures, alors la conversion vers le repère absolu est effecutée.
    *
    * Cette méthode prend en compte le rectangle de limites.
    */
    pair<float, float> viewport() const;
    /**
    * @brief Retourne le nom de l'objet suivi, ou une chaîne vide si aucun objet n'est suivi.
    * @return Le nom de l'objet suivi.
    */
    const string followedObjectName() const;
    /*
    * Accesseurs
    * ----------
    */
    float getX() const {return m_x;} //!< Accesseur.
    float getY() const {return m_y;} //!< Accesseur.
    void setMinStep(float minStep) {m_minStep = minStep;} //!< Accesseur.
    bool isRelativeToBorders() const {return m_relativeToBorders;} //!< Accesseur.

private:
    float m_x; //!< L'abscisse de cette caméra.
    float m_y; //!< L'ordonnée de cette caméra.
    PfPoint m_prev; //!< Le point où se trouvait cette caméra au pas précédent.
    PfPoint m_target; //!< Le point visé par cette caméra.
    PfPoint m_pin; //!< Le point épingle.
    unsigned int m_speed; //!< La vitesse de déplacement de cette caméra.
    bool m_keepSpeed; //!< Indique si les méthodes de déplacements, accompagnées d'un paramètre vitesse, donnent cette vitesse de manière définitive (si faux, à la fin du déplacement, la vitesse repasse à 0).
    float m_minStep; //!< La distance minimale à parcourir par pas, quelque soit la vitesse de cette caméra.
    bool m_relativeToBorders; //!< Indique si les coordonnées de cette caméra utilisent le repère entre bordures.
    const RectangleGLItem* mq_targetItem; //!< Un pointeur constant vers un objet cible, 0 si aucune cible.
    PfRectangle m_limitRect; //!< Le rectangle définissant les limites du champ de la caméra.
};

#endif // CAMERA_H_INCLUDED



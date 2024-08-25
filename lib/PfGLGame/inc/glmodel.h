/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe GLModel.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef GLMODEL_H_INCLUDED
#define GLMODEL_H_INCLUDED

#include "glgame_gen.h"
#include "abstractmodel.h"
#include "camera.h"

/**
* @brief Modèle MVC dédié à la gestion d'un programme rendu sous OpenGL.
*
* Ce modèle intègre deux coordonnées de caméra. Il s'agit de la caméra déterminant la position des images à l'écran.
* Ces valeurs correspondent à la translation OpenGL corrigée des bordures de la vue.
*
* Les bordures mentionnées ici sont les bandes latérales à l'écran qui permettent d'obtenir une image carrée.
* Deux repères de coordonnées peuvent être utilisés :
* <ul><li>un repère lié à l'écran entier, dont le point inférieur gauche se situe au coin inférieur gauche de l'écran,</li>
* <li>un repère entre bordures, dont le point inférieur gauche se situe au coin inférieur droit de la bordure gauche.</li></ul>
*
* Les coordonnées de la caméra de ce modèle sont exprimées selon le second repère.
* La fonction translateCamera (fichier "glfunc.h" de la bibliothèque PfMedia) est donc appelée après conversion vers le premier repère.
*/
class GLModel : public AbstractModel
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur GLModel.
    */
    GLModel() {}
    /**
    * @brief Destructeur GLModel.
    */
    virtual ~GLModel() {}
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Retourne un pointeur vers la caméra de ce modèle.
    * @return Un pointeur vers la caméra de ce modèle.
    */
    PfCamera* camera();
    /**
    * @brief Affiche les informations DEBUG.
    *
    * Ne fonctionne que si la macro NDEBUG n'est pas définie, et quand la variable globale <em>g_debug</em> est vraie.
    * Sinon, rien n'est fait.
    *
    * Un label noir est affiché au coin supérieur gauche de l'écran, avec la liste des informations contenues dans les maps globales
    * <em>g_dbInt_v_map</em> et <em>g_dbFloat_v_map</em> (fichier "gen.h" de la bibliothèque PfMisc).
    * Ces deux maps sont effacées après affichage (pour le cycle de mise à jour des valeurs à chaque tour).
    */
    void showDebugInfo();

protected:
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Met à jour chaque ModelItem de la liste de ce modèle.
    *
    * Chaque ModelItem est marqué non modifié, puis, la méthode ModelItem::update est appelée.
    *
    * Les ModelItem de statut ModelItem::VISIBLE passent au statut ModelItem::UNCHANGED : une fois que la vue
    * a mis à jour le statut de visibilité du Viewable correspondant, il n'est plus nécessaire de le refaire éternellement.
    * Ceci n'est par contre pas possible avec ModelItem::INVISIBLE car il faut constamment dire à la vue que l'objet est invisible si on ne veut pas qu'il réapparaisse dès qu'il est modifié.
    *
    * Les ModelItem de statut ModelItem::DEAD sont supprimés.
    */
    virtual void updateItems();
    /**
    * @brief Retourne une information permettant la mise à jour de la partie visible des vues surveillant ce modèle.
    *
    * Les deux valeurs retournées sont les coordonnées de la caméra.
    */
    virtual pair<float, float> viewportData() const;

    PfCamera m_camera; //!< La caméra gérée par ce modèle.
};

#endif // GLMODEL_H_INCLUDED


/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe GLView.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef GLVIEW_H_INCLUDED
#define GLVIEW_H_INCLUDED

#include "glgame_gen.h"
#include "abstractview.h"
#include "geometry.h"

class Viewable;

/**
* @brief Vue du système MVC spécialisée dans l'affichage d'images rendues grâce à OpenGL.
*
* Les Viewable sont rendus via l'interface OpenGL.
*
* Cette vue définit un viewport, rectangle caractéristant la partie visible du monde représenté.
* Cette information est utilisée par la méthode AbstractView::viewportContains et seuls les Viewable compris dans cet espace sont affichés.
*/
class GLView : public AbstractView
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur GLView.
    */
    GLView();
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Met à jour la partie visible de cette vue.
    * @param x L'abscisse du viewport.
    * @param y L'ordonnée du viewport.
    *
    * Met à jour le rectangle de la partie visible de cette vue.
    * Le rectangle est de côté 1.0 en X et Y_X_RATIO en Y, de coordonnées inférieures gauches correspondant aux paramètres.
    * L'argument <em>y</em> est multiplié par Y_X_RATIO pour prendre en compte des coordonnées "entre bordures". Par exemple, (1;1) décalera le viewport
    * d'un carré vers le haut à droite par rapport à la position initiale, ce qui ne serait pas le cas sans cette conversion.
    */
    void updateViewport(float x, float y);

private:
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Indique si un Viewable est visible sur cette vue.
    * @param rc_viewable Le viewable à tester.
    *
    * L'ensemble des images de ce Viewable sont testées.
    * Si au moins une image a son rectangle entièrement ou partiellement compris dans le viewport, alors <code>true</code> est retourné.
    *
    * Les Viewable contenant au moins une image statique sont toujours considérés visibles.
    *
    * Une image statique dans ce Viewable retournera toujours <code>true</code>.
    *
    * @warning
    * Les Viewable liés au Viewable passé en paramètre ne sont pas considérés.
    */
    virtual bool viewportContains(const Viewable& rc_viewable) const;
    /**
    * @brief Action réalisée par la vue avant l'affichage de tous les Viewable.
    *
    * Appelle la fonction <em>clearGL</em>.
    */
    virtual void initializeDisplay() const;
    /**
    * @brief Affiche un Viewable à l'écran.
    * @param viewable Le Viewable à afficher.
    * @throw PfException si une erreur survient lors de l'affichage du Viewable.
    *
    * Appelle la fonction <em>drawGL</em> sur chaque GLImage du Viewable.
    *
    * Appelle la fonction <em>playSound</em> si le Viewable a un son à jouer.
    */
    virtual void displayViewable(const Viewable& viewable) const;
    /**
    * @brief Action réalisée après affichage de tous les Viewable.
    *
    * Appelle la fonction <em>drawTransition</em>, puis appelle les fonctions <em>flushGL</em> et <em>swapSDLBuffers</em>.
    */
    virtual void finalizeDisplay() const;

    PfRectangle m_viewport; //!< Le rectangle définissant la partie visible de cette vue.
};

#endif // GLVIEW_H_INCLUDED


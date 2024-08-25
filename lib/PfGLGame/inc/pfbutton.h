/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe PfButton.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef PFBUTTON_H_INCLUDED
#define PFBUTTON_H_INCLUDED

#include "glgame_gen.h"

#include "pftext.h"
#include "pfwidget.h"

/**
* @brief Bouton cliquable.
*
* Ce widget est le widget le plus simple qui soit, et ne redéfinit que peu de choses.
* Il s'agit presque d'un simple label, mais il ne fournit pas les méthodes de manipulation de texte de ce dernier, se contentant d'un texte centré sur une ligne.
*
* Un bouton peut afficher un texte.
*
* Les marges utilisées pour l'affichage de ce texte sont exprimées en fraction des dimensions totales de ce bouton, définies dans le constructeur.
* Par exemple, une marge horizontale de 0,25 centrera le texte à un quart de largeur à partir de la gauche et de la droite de ce bouton.
*/
class PfButton : public PfWidget
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur PfButton.
    * @param name Le nom de ce bouton.
    * @param x L'abscisse de ce bouton.
    * @param y L'ordonnée de ce bouton.
    * @param w La largeur de ce bouton.
    * @param h La hauteur de ce bouton.
    * @param layer Le plan de perspective de ce widget.
    * @param fontTextureIndex L'indice de texture de la police de ce bouton.
    * @param text Le texte de ce bouton.
    * @param hMargin La marge horizontale du texte de ce bouton.
    * @param vMargin La marge verticale du texte de ce bouton.
    * @param statusMap La valeur d'initialisation de la map d'association animation <-> état de ce widget.
    * @throw ConstructorException si une erreur survient lors de la génération des animations de cet objet.
    *
    * La taille de la police est automatiquement déterminée en fonction de la taille de ce bouton et de celle du texte.
    */
    PfButton(const string& name, float x = 0.0, float y = 0.0, float w = 1.0, float h = 1.0,
             unsigned int layer = MAX_LAYER, unsigned int fontTextureIndex = FONT_TEXTURE_INDEX, const string& text = "",
             float hMargin = 0.0, float vMargin = 0.0, PfWidget::PfWidgetStatusMap statusMap = WIDGET_STANDARD_MAP);
    /**
    * @brief Destructeur PfButton.
    */
    virtual ~PfButton() {}
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Modifie le texte de ce bouton.
    * @param text Le nouveau texte.
    *
    * Ce ModelItem est marqué modifié.
    */
    virtual void changeText(const string& text);
    /**
    * @brief Génère un Viewable à partir de ce ModelItem.
    * @return Le Viewable créé.
    *
    * @warning
    * De la mémoire est allouée pour le pointeur retourné.
    */
    virtual Viewable* generateViewable() const;
};

#endif // PFBUTTON_H_INCLUDED

/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe PfProgressBar.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef PFPROGRESSBAR_H_INCLUDED
#define PFPROGRESSBAR_H_INCLUDED

#include "glgame_gen.h"

#include "pfwidget.h"

/**
* @brief Barre de progression.
*
* Ce widget est composé d'une barre de progression se remplissant d'un composant graphique élémentaire.
* Il ne traite pas les actions de l'utilisateur, se limitant à un rôle d'affichage.
*
* La barre de progression est divisée en pas de division, appelés composants élémentaires.
*
* @section AnimDef Définition des animations dans le wad
*
* Seule l'animation ANIM_IDLE est à considérer.
*
* La texture du composant graphique élémentaire est déterminée par le wad.
* La barre de défilement possède une texture carrée 3x3, située dans le coin inférieur droit du même fichier.
*
* Pour le calcul des coordonnées de texture des éléments de ce widget, il est nécessaire que l'image du composant graphique élémentaire soit carrée et de dimensions trois fois plus petites
* que celles de la barre.
*
* Dans le script du wad, c'est la texture du composant élémentaire qui est visée lors de la définition de la frame.
* L'image du corps de la barre est automatiquement prise dans le même fichier image, selon la description du paragraphe ci-dessus.
*
* Ceci est géré par la méthode PfProgressBar::addAnimation redéfinie.
*/
class PfProgressBar : public PfWidget
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur PfProgressBar.
    * @param name Le nom de cette barre.
    * @param rect Le rectangle des coordonnées de ce widget.
    * @param steps Le nombre de divisions de cette barre.
    * @param layer Le plan de perspective de ce widget.
    * @throw ConstructorException si une erreur survient lors de la construction.
    */
    PfProgressBar(const string& name, const PfRectangle& rect, unsigned int steps, int layer = MAX_LAYER);
    /**
    * @brief Destructeur PfProgressBar.
    *
    * Détruit les éléments de cette barre de progression.
    */
    virtual ~PfProgressBar();
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Modifie le taux de remplissage de cette barre de progression.
    * @param ratio Le taux de remplissage.
    *
    * Le nombre de divisions remplies est mis à jour en fonction du taux passé en paramètre.
    *
    * @remarks
    * Ce ModelItem est marqué modifié.
    */
    void changeStep(float ratio);
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Change le taux de remplissage de cette barre.
    * @param value La valeur de 0 à 100 du taux de remplissage de cette barre.
    *
    * La valeur représente le taux de pourcentage de cette barre, si elle est négative ou supérieure à 100, elle est écrêtée.
    *
    * La méthode PfProgressBar::changeStep est appelée.
    *
    * @remarks
    * Ce ModelItem est marqué modifié.
    */
    virtual void changeValue(int value);
    /**
    * @brief Ajoute une animation aux composants graphiques élémentaires.
    * @param p_anim L'animation à ajouter.
    * @param value Le statut de l'animation.
    * @throw PfException si une exception survient lors de l'ajout.
    *
    * Si une animation existe déjà pour ce statut, elle est remplacée.
    *
    * Le pointeur à ajouter ne doit pas être nul.
    * Il sera copié pour ajout à chaque composant, puis détruit en fin de méthode.
    *
    * L'animation ANIM_IDLE de la barre principale est créée si la valeur du paramètre value est ANIM_IDLE.
    * Cette animation est construite avec le même indice de texture, comme indiqué dans la description de cette classe.
    */
    virtual void addAnimation(PfAnimation* p_anim, PfAnimationStatus value);
    /**
    * @brief Génère un Viewable à partir de ce ModelItem.
    * @return Le Viewable créé.
    *
    * @warning
    * De la mémoire est allouée pour le pointeur retourné.
    */
    virtual Viewable* generateViewable() const;

private:
    AnimatedGLItem* mp_left; //!< La partie gauche de la barre.
    AnimatedGLItem* mp_right; //!< La partie droite de la barre.
    AnimatedGLItem** mp_bar_t; //!< Le tableau des composants de la barre vide et ses contours inférieurs et supérieurs.
    AnimatedGLItem** mp_tiles_t; //!< Le tableau des composants graphiques élémentaires de chargement.
    unsigned int m_stepsCount; //!< Le nombre de divisions de cette barre.
    unsigned int m_currentStep; //!< Le nombre de divisions remplies.
};

#endif // PFPROGRESSBAR_H_INCLUDED

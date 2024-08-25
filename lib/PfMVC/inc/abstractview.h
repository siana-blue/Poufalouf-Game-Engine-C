/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe AbstractView.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef ABSTRACTVIEW_H_INCLUDED
#define ABSTRACTVIEW_H_INCLUDED

#include "mvc_gen.h"
#include <map>
#include <string>
#include "noncopyable.h"

class ModelItem;
class Viewable;

/**
* @brief Vue MVC.
*
* Une vue est mise à jour grâce à une liste de ModelItem qui lui est envoyée, normalement par un modèle MVC.
* Dans sa méthode AbstractView::update, la vue contrôle le statut de chaque ModelItem reçu, et en fonction de celui-ci,
* génère un Viewable grâce à la méthode ModelItem::generateViewable et rend ce Viewable visible ou non.
*
* Tout Viewable visible placé dans la liste AbstractView::mpn_viewables_map est affiché à l'écran.
*
* Les Viewable sont classés dans la liste par les noms des ModelItem qui les ont créés.
* Ainsi, la vue peut reconnaître un ModelItem reçu et ne pas avoir à régénérer un Viewable si le ModelItem est inchangé.
* Le modèle MVC envoie systématiquement une référence vers sa liste de ModelItem, mais la vue ne perd pas de temps à tout régénérer à chaque tour.
* Seuls les ModelItem dont le nom n'est pas encore présent dans la liste de Viewable ou qui sont marqués modifiés génèreront un nouveau Viewable.
*
* @warning
* Aucun pointeur nul n'est toléré dans la liste de Viewable. Des assertions vérifient ce point.
*
* <b>Rappel MVC</b>
*
* La vue est mise à jour grâce aux données fournies par le modèle, tout d'abord en ce qui concerne le viewport, puis en ce qui concerne chaque objet
* du jeu.
*
* La liste des ModelItem est transmise à la vue par le modèle (AbstractView::update), puis les objets du modèle sont mis à jour
* (pour que la première frame soit prise en compte), et enfin la méthode AbstractView::display est appelée.
*/
class AbstractView : private NonCopyable
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur AbstractView.
    *
    * Initialise le compteur de temps AbstractView::m_timeCounter d'après la fonction <em>SDL_GetTicks</em>.
    */
    AbstractView();
    /**
    * @brief Destructeur AbstractView.
    *
    * Ce destructeur détruit les Viewable de la liste.
    */
    virtual ~AbstractView();
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Affiche tous les Viewable visibles à l'écran.
    *
    * Cette méthode est générale à tous les types de vue MVC.
    * C'est la méthode AbstractView::displayViewable qu'il faut redéfinir pour spécifier le mode d'affichage.
    *
    * Cette méthode gère la fréquence d'affichage en gelant le programme si le dernier rafraîchissement
    * s'est fait trop tôt par rapport à la valeur FPS_RATE (fichier "mvc_gen.h").
    * La fonction <em>SDL_Delay</em> est utilisée pour retarder le déroulement du programme en cas d'avance.
    *
    * Cette méthode trie les Viewable de la liste AbstractView::mpn_viewables_map en fonction de leurs plans de perspective et les affiche
    * chacun au moyen de la méthode virtuelle AbstractView::displayViewable en commençant par le plan le plus profond (d'indice inférieur).
    * Si un Viewable présente des Viewables liés, alors ceux-ci sont triés comme des Viewable indépendants.
    * Seuls les Viewable visibles dans la vue (testés par la méthode AbstractView::viewportContains) sont affichés et triés.
    * Les Viewable liés subissent ce test de manière indépendante de leur Viewable principal, il est donc inutile dans les redéfinitions
    * de AbstractView de tenir compte des Viewable liés dans cette méthode de test.
    *
    * Avant cela, la méthode virtuelle AbstractView::initializeDisplay est appelée.
    * Après l'affichage des Viewable, la méthode virtuelle AbstractView::finalizeDisplay est appelée.
    */
    void display();
    /**
    * @brief Met à jour la liste de Viewable de cette vue MVC en fonction d'une liste de ModelItem, normalement émise par un modèle MVC.
    * @param p_modelItems_map La map de ModelItem.
    *
    * Pour chaque ModelItem, si aucun Viewable n'est connu de cette vue avec le même nom, alors il est généré à partir du ModelItem
    * (ModelItem::generateViewable). Si l'objet est connu mais marqué modifié, alors le Viewable est regénéré.
    *
    * L'état du ModelItem est ensuite utilisé pour effectuer les actions suivantes sur les Viewable gérés par la vue :
    * <ul><li>ModelItem::VISIBLE : rendre le Viewable visible,</li>
    * <li>ModelItem::INVISIBLE : rendre le Viewable invisible,</li>
    * <li>ModelItem::DEAD : suppression du Viewable.</li></ul>
    *
    * @warning
    * Aucun pointeur passé en paramètre ne doit être nul. Une assertion vérifie ce point.
    */
    void update(const map<string, ModelItem*>& p_modelItems_map);
    /**
    * @brief Met à jour la partie visible de cette vue.
    * @param x Une valeur à utiliser dans la méthode redéfinie, typiquement une abscisse.
    * @param y Une valeur à utiliser dans la méthode redéfinie, typiquement une ordonnée.
    *
    * Par défaut, cette méthode ne fait rien.
    */
    virtual void updateViewport(float x, float y) {}

private:
    /**
    * @brief Indique si un Viewable est visible sur cette vue.
    * @param rc_viewable Le viewable à tester.
    *
    * Par défaut, retourne <code>true</code>.
    *
    * Ceci est dissocié du caractère visible ou invisible du Viewable.
    * Un Viewable peut être invisible tout en étant dans la partie visible de la vue.
    * Cette méthode sert à éviter le traitement des Viewable visibles hors de la partie visible de la vue.
    *
    * Pour information, les Viewable visibles dans la vue génèreront des images qui, elles, seront encore testées avant d'être affichées
    * (fonction <em>drawGL</em> du fichier "glfunc.h" de la bibliothèque PfMedia).
    */
    virtual bool viewportContains(const Viewable& rc_viewable) const;
    /**
    * @brief Action réalisée par la vue avant l'affichage de tous les Viewable.
    */
    virtual void initializeDisplay() const = 0;
    /**
    * @brief Méthode à redéfinir pour afficher un Viewable à l'écran.
    * @param rc_viewable Le Viewable à afficher.
    *
    * Cette méthode est appelée par AbstractView::display pour chaque Viewable visible.
    */
    virtual void displayViewable(const Viewable& rc_viewable) const = 0;
    /**
    * @brief Action réalisée après affichage de tous les Viewable.
    */
    virtual void finalizeDisplay() const = 0;

    map<string, Viewable*> mpn_viewables_map; //!< La map des viewables de cette vue. La méthode AbstractView::update y alloue de la mémoire.
    uint32_t m_timeCounter; //!< Le compteur de temps servant à la gestion du taux FPS.
};

#endif // ABSTRACTVIEW_H_INCLUDED


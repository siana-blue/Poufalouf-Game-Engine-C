/**
* @file
* @author Anaïs Vernet
* @brief Header inclus dans tous les fichiers header de cette bibliothèque.
* @date xx/xx/xxxx
* @version 0.0.0
*
* La macro FPS_RATE est définie ici. Elle fixe le temps minimal entre deux rafraîchissements successifs de la vue.
*/

/**
* @mainpage
*
* <b>Bibliothèque contenant les classes de base d'un système MVC</b>
*
* @warning
* Cette bibliothèque dépend des bibliothèques PfMisc et PfMedia.
*
* Les éléments basiques d'un système MVC sont les suivants :
*
* <ul><li>Le modèle, gérant les actions du jeu au travers de la manipulation d'objets (AbstractModel),</li>
* <li>La vue, affichant les objets du modèle auquel elle est liée (AbstractView),</li>
* <li>Le contrôleur, traitant les entrées utilisateur et emettant des instructions vers le modèle pour sa mise à jour (AbstractController),</li>
* <li>Les objets du modèle (ModelItem),</li>
* <li>Les images affichées à l'écran, correspondant aux objets du modèle (Viewable).</li></ul>
*
* Le pattern MVC utilisé ici est un peu différent de ce que l'on peut voir sur internet, il ne s'agit pas du pattern classique.
* Pour résumer les interactions entre classe, il faut plutôt voir : C -> M -> V et non le cycle classique entre les trois composants.
* Ici, c'est à sens unique.
*
* Le contrôleur possède un lien vers le modèle qu'il contrôle. Grâce à ce lien, il transmet diverses instructions au modèle par appel à ses méthodes.
*
* Le modèle possède un lien vers une ou plusieurs vues qu'il met à jour. Il dispose d'une liste d'objets qui sont mis à jour via ses différentes méthodes,
* et ceux-ci sont ensuite transmis aux vues liées. Des informations sur le viewport peuvent également être transmises.
*
* Les vues affichent des images à l'écran, ou des textes sur console, des sons et toute sortie imaginable, via la classe Viewable. Ces Viewable sont
* générés à partir des ModelItem transmis par le modèle.
*
* @see MVCSystem
*/

#ifndef MVC_GEN_H_INCLUDED
#define MVC_GEN_H_INCLUDED

#include "misc_gen.h"

#define FPS_RATE 50 //!< La cadence de rafraîchissement de l'image en ms.

#endif // MVC_GEN_H_INCLUDED

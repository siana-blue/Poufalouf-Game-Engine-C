/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant les éléments de l'interface graphique des jeux Poufalouf.
* @date xx/xx/xxxx
* @version 0.0.0
*
* Ce fichier contient les éléments utilisés pour l'interface graphique, sous forme d'inclusions des fichiers correspondants.
* Ces éléments, appelés widgets, fonctionnent comme indiqué ci-dessous.
*
* Un widget est un AnimatedGLItem.
* Il dispose de méthodes génériques caractérisant une action de l'utilisateur, comme PfWidget::highlight ou PfWidget::activate par exemple.
* Ces méthodes, vides dans la classe mère PfWidget, peuvent être redéfinies par les classes filles, si nécessaire.
*
* Un widget peut posséder un effet, défini par un flag PfEffect.
* Ces flags sont constitués de valeurs (flags) sur les 12 bits de poids le plus fort, les 8 premier bits servant à associer une valeur numérique à ces effets.
*
* Les widgets peuvent être associés en layouts (PfLayout).
* Les layouts présentent des méthodes permettant de sélectionner chaque widget.
* En fonction de cette sélection, des méthodes de lien permettent d'appeler les méthodes spécifiques de chaque widget (activate etc...)
* Un layout est un widget.
*
* Chaque widget peut stocker un point (coordonnées OpenGL).
* En fonction de ce point, l'action du widget lors de son activation peut varier.
* Cela est utilisé pour traiter l'action de la souris sur un widget composé de plusieurs éléments.
* Lors de la sélection par un point sur un layout, le layout transmet l'information de ce point au widget sur lequel il est.
*
* Le rectangle de coordonnées d'un widget doit englober l'ensemble de ses composants, même s'il n'a pas d'image.
* C'est ce rectangle qui sera utilisé pour déterminer si on clique dessus, et qui redistribuera ensuite l'information de ce point à ses composants.
*/

#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include "pftext.h"
#include "pfwidget.h"
#include "pflayout.h"
#include "pfbutton.h"
#include "pftextbox.h"
#include "pfspinbox.h"
#include "pfselectiongrid.h"
#include "pflist.h"
#include "pfslidingcursor.h"
#include "pfscrollbar.h"
#include "pfprogressbar.h"

#endif // GUI_H_INCLUDED

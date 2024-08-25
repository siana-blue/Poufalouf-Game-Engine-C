/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe MVCSystem.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef MVCSYSTEM_H_INCLUDED
#define MVCSYSTEM_H_INCLUDED

#include "mvc_gen.h"
#include "noncopyable.h"
#include "datapackage.h"

class AbstractModel;
class AbstractView;
class AbstractController;

/**
* @brief Système MVC regroupant un modèle, une vue et un contrôleur.
*
* Les trois composants du système sont accessibles sous forme de pointeurs vers des éléments constants.
* Il sera certainement nécessaire de les "caster" en fonction du type de système MVC créé.
*
* Ce type de système ne prend en compte qu'une vue, bien qu'il soit techniquement possible d'associer plusieurs vues à un même modèle.
*
* Le modèle stocke les données du programme sous forme d'objets ModelItem.
* Ces ModelItem peuvent générer une image de type Viewable qui sera affichée sur la vue.
*
* La vue affiche les Viewable générés grâce à la liste de ModelItem du modèle.
*
* Le contrôleur demande au modèle de modifier ses objets en fonction des actions de l'utilisateur et d'un cycle de mise à jour.
* Pour cela, il appelle les méthodes propres au modèle.
* Le modèle sait réaliser des tâches de manipulation de ses objets, mais c'est le contrôleur qui ordonne ces actions.
*
* Pour lancer un MVCSystem une fois construit, utiliser la méthode MVCSystem::run.
* Cette méthode lance une boucle infinie qui est quittée dès que le contrôleur quitte le statut AbstractController::ALIVE.
* Si ce statut passe à AbstractController::DEAD, alors <code>false</code> est retourné, sinon, <code>true</code>.
*
* Un MVCSystem de statut AbstractController::DEAD ne pourra pas être relancé par la méthode MVCSystem::run une fois la boucle infinie quittée.
*
* Les pointeurs vers les trois composants du système sont <code>protected</code>, ainsi que MVCSystem::m_selfDestructible,
* ceci pour permettre aux constructeurs des classes filles de les définir directement.
*
* Quand la boucle infinie est appelée par la méthode MVCSystem::run, il est possible de préciser le mode <em>One Shot</em>.
* Dans ce mode, les données utilisateurs ne sont pas traitées et la boucle ne s'exécute qu'une seule fois avant d'être quittée.
*
* Il est possible via cette classe de récupérer des valeurs stockées dans le contrôleur. Ces valeurs peuvent être utilisées de diverses façons
* pour transmettre des informations au programme dans la boucle principale ayant recours à un MVCSystem.
* La méthode MVCSystem::values permet de faire ce lien, en retournant un DataPackage.
*
* <b>Structure MVC</b>
*
* Le modèle est observé par une ou plusieurs vues, mises à jour en fonction de ses données.
* Le contrôleur observe un unique modèle qu'il met à jour.
* Contrairement au pattern MVC classique, ici, la vue n'a aucune action sur le système. Elle est passive.
* C'est le contrôleur qui traite les entrées de l'utilisateur.
*/
class MVCSystem : private NonCopyable
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur MVCSystem.
    * @param p_model Un pointeur sur le modèle à utiliser.
    * @param p_view Un pointeur sur la vue à utiliser.
    * @param p_controller Un pointeur sur le contrôleur à utiliser.
    * @param selfDestructible <code>true</code> si le destructeur de ce MVCSystem doit détruire les trois composants ci-dessus.
    *
    * Si les pointeurs de la vue et du modèle ne sont pas nuls, alors la vue est liée au modèle.
    */
    MVCSystem(AbstractModel* p_model = 0, AbstractView* p_view = 0, AbstractController* p_controller = 0, bool selfDestructible = false);
    /**
    * @brief Destructeur MVCSystem.
    *
    * Si ce MVCSystem est auto-destructible, alors les trois composants de ce système MVC sont également détruits.
    */
    virtual ~MVCSystem();
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Lance la boucle infinie de ce système.
    * @param once <code>true</code> si la boucle ne doit être parcourue qu'une fois (<em>One Shot</em>).
    * @return <code>true</code> si le contrôleur donne un statut différent de AbstractController::DEAD en sortie de boucle.
    *
    * Avant d'entrer dans la boucle, une fois la méthode MVCSystem::run lancée, le contrôleur doit tout d'abord être réveillé dans le cas où il était
    * endormi.
    * Ceci se fait par appel de la méthode AbstractController::wakeUp.
    *
    * Ensuite, la boucle suivante est lancée et tourne tant que le contrôleur est à l'état AbstractController::ALIVE.
    * <ul><li>Si les entrées utilisateurs sont acceptées et qu'il ne s'agit pas d'un <em>One Shot</em>, elles sont traitées par appel à la méthode
    * AbstractController::pollInput. Si elles ne sont pas acceptées ou s'il s'agit d'un <em>One Shot</em>, alors la méthode AbstractController::flushInput
    * est appelée à la place.</li>
    * <li>Un nouveau test de réveil du contrôleur est effectué mais cette fois sans action. Si le contrôleur est endormi ou mort suite aux actions traitées,
    * alors la boucle passe directement à la dernière ligne de cette liste.</li>
    * <li>Le contrôleur est mis à jour par appel à la méthode AbstractController::update.
    * Typiquement, les données stockées par le gestionnaire d'événements utilisateurs sont utilisées pour mettre à jour le modèle au moyen d'actions
    * induites par le contrôleur.</li>
    * <li>Mise à jour des vues par le modèle par appel à la méthode AbstractModel::notifyAll.</li>
    * <li>Mise à jour des objets du modèle par appel à la méthode AbstractModel::updateItems. Cela se fait après mise à jour des vues,
    * et se traduit par exemple par la destruction d'un ModelItem de statut ModelItem::DEAD.
    * Ce ModelItem aura entraîné la destruction du Viewable associé dans la vue et peut maintenant être effectivement détruit dans le modèle.</li>
    * <li>Rendu des vues par appel à la méthode AbstractView::display.</li>
    * <li>S'il s'agit d'un <em>One Shot</em>, passage du contrôleur en mode AbstractController::ASLEEP.</li></ul>
    */
    bool run(bool once = false);
    /**
    * @brief Retourne les valeurs stockées dans le contrôleur de ce système.
    * @return La liste de valeurs.
    */
    const DataPackage& values() const;
    /*
    * Accesseurs
    * ----------
    */
    const AbstractModel* getModel() const {return mp_model;} //!< Accesseur.
    const AbstractView* getView() const {return mp_view;} //!< Accesseur.
    const AbstractController* getController() const {return mp_controller;} //!< Accesseur.

protected:
    AbstractModel* mp_model; //!< Le modèle de ce système.
    AbstractView* mp_view; //!< La vue de ce système.
    AbstractController* mp_controller; //!< Le contrôleur de ce système.
    bool m_selfDestructible; //!< Indique si les composants de ce système sont détruits par le destructeur.
};

#endif // MVCSYSTEM_H_INCLUDED


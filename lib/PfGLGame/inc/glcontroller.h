/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe GLController.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef GLCONTROLLER_H_INCLUDED
#define GLCONTROLLER_H_INCLUDED

#include "glgame_gen.h"
#include "abstractcontroller.h"
#include "eventhandler.h"

class GLModel;

/**
* @brief Contrôleur MVC dédié à la gestion d'un programme sous OpenGL.
*
* Ce contrôleur redéfinit notamment les méthodes virtuelles pures AbstractController::pollInput et AbstractController::update
* pour traiter l'appui de la touche F11 gérant le mode plein écran.
* La touche ESCAPE ainsi que l'événement SDL_Quit sont également gérés ici, conduisant au passage à l'état AbstractController::DEAD de ce contrôleur.
*
* Les événements SDL sont gérés par ce modèle sous la forme d'un EventHandler.
* La méthode GLController::pollInput met à jour l'EventHandler et la méthode GLController::update utilise l'état stocké dans cette instance.
*/
class GLController : public AbstractController
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur GLController.
    * @param r_glmodel Le modèle géré par ce contrôleur.
    */
    explicit GLController(GLModel& r_glmodel);
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Lit les entrées de l'utilisateur et effectue les traitements nécessaires.
    *
    * La méthode EventHandler::pollEvents est appelée sur le gestionnaire d'événéments de cette classe.
    */
    virtual void pollInput();
    /**
    * @brief Efface toute entrée utilisateur.
    *
    * Appelle la méthode EventHandler::reset.
    */
    virtual void flushInput();
    /**
    * @brief Cycle de mise à jour du contrôleur : le modèle est mis à jour par appel à ses différentes méthodes.
    *
    * L'EventHandler est utilisé de la manière suivante :
    * <ul><li>SDL_QUIT : le statut de ce contrôleur passe à DEAD,</li>
    * <li>appui sur la touche SDLK_ESCAPE : le statut de ce contrôleur passe à DEAD,</li>
    * <li>appui sur la touche SDLK_F11 : le mode plein écran est inversé.</li></ul>
    *
    * Ici, les actions suivantes sont réalisées :
    * <ul><li>le mode plein écran est activé ou désactivé en fonction de la valeur du booléen GLController::m_fullScreen,</li>
    * <li>la caméra du modèle observé est mise à jour.</li></ul>
    */
    virtual void update();
    /**
    * @brief Indique si ce contrôleur accepte les entrées utilisateurs.
    * @return <code>true</code> si les entrées utilisateur sont admises.
    *
    * Ici, retourne toujours <code>true</code>.
    */
    virtual bool isAvailable() const;

protected:
    EventHandler m_eventHandler; //!< Le gestionnaire d'événements.

private:
    bool m_fullscreen; //!< Indique si l'affichage SDL doit être en mode plein écran.
    uint32_t m_flags; //!< Les flags SDL de l'affichage en cours.
};

#endif // GLCONTROLLER_H_INCLUDED


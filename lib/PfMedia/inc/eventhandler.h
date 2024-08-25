/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe EventHandler.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef EVENTHANDLER_H_INCLUDED
#define EVENTHANDLER_H_INCLUDED

#include "media_gen.h"

#include <SDL.h>
#include <map>
#include <vector>
#include "geometry.h"

/**
* @brief Classe permettant une gestion groupée de multiples événements SDL.
*
* Cette classe contient une liste de champs représentant l'état résultant des derniers événements SDL enregistrés.
*
* La méthode EventHandler::pollEvents appelle la fonction <em>SDL_PollEvent</em> tant que celle-ci lit un événement SDL.
* Chaque événement est traité pour modifier l'état représenté par cette instance de classe.
*
* Les événements stockés sont les suivants :
* <ul><li>les événements clavier, gérés par la map EventHandler::m_keys_map contenant un entier par type de touche SDL,
* suivant l'énumération EventHandler::EventHandlerCode (absent de la map = EVENT_RELEASED),</li>
* <li>les événements souris, traités par les deux champs EventHandler::m_leftMouse et EventHandler::m_rightMouse, suivant l'énumération
* EventHandler::EventHandlerCode, et la position du curseur gérée par EventHandler::m_mouseCoord, EventHandler::m_mouseRelX et EventHandler::m_mouseRelY,</li>
* <li>l'événement SDL_QUIT, géré par EventHandler::m_quitting,</li>
* <li>le texte entré, via EventHandler::m_inputText_v,</li>
* <li>la mise en mémoire d'un accent, via EventHandler::m_caret (^), EventHandler::m_umlaut (¨) et EventHandler::m_tilde (~).</li></ul>
*
* L'événement le plus récent l'emporte sur un appui ou un relâchement de touche ou de bouton de souris.
* Les mouvements relatifs sont cumulés. Enfin, la dernière position de souris est prise en compte.
*
* L'événement SDL_TEXTINPUT n'est pas utilisé par cette classe. A la place, le membre EventHandler::m_inputText_v est mis à jour en fonction
* des touches entrées et de la méthode EventHandler::handleTextKey.
*/
class EventHandler
{
public:
    /**
    * @brief Enumération des codes de gestion des événements utilisateur.
    *
    * Ces valeurs servent à qualifier un événement lié à un bouton du clavier ou de la souris.
    */
    enum EventHandlerCode
    {
        EVENT_RELEASED, //!< Touche relâchée.
        EVENT_PRESSED, //!< Touche enfoncée.
        EVENT_JUST_PRESSED, //!< Touche tout juste enfoncée.
        EVENT_JUST_RELEASED //!< Touche tout juste relâchée.
    };

    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur EventHandler par défaut.
    *
    * Les événements relatifs aux touches de souris sont à l'état EVENT_RELEASED et les autres valeurs sont à 0 ou <code>false</code>.
    *
    * La map d'événements clavier est vide.
    */
    EventHandler();
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Lit tous les événements SDL dans la file d'attente.
    *
    * Avant de commencer à traiter la pile, les actions suivantes sont réalisées :
    * <ul><li>les valeurs EventHandler::m_mouseRelX et EventHandler::m_mouseRelY sont mises à 0,</li>
    * <li>dans le tableau EventHandler::m_keys_map, toute valeur EVENT_JUST_PRESSED passe à EVENT_PRESSED et toute valeur EVENT_JUST_RELEASED passe à EVENT_RELEASED (retirée de la map),</li>
    * <li>idem pour les valeurs de la souris,</li>
    * <li>le vecteur EventHandler::m_inputText_v est vidé.</li></ul>
    *
    * Chaque événement est ensuite traité pour modifier l'état représenté par cette classe.
    *
    * Dans le cas d'une touche du clavier enfoncée (SDL_KEYDOWN), la méthode privée EventHandler::handleTextKey est également appelée pour éventuellement
    * mettre à jour le champ EventHandler::m_inputText_v en ajoutant le retour de cette méthode.
    */
    void pollEvents();
    /**
    * @brief Annule tout événement enregistré.
    *
    * La valeur EventHandler::m_mouseCoord passe à (0.0, 0.0).
    */
    void reset();
    /**
    * @brief Indique si une touche est enfoncée.
    * @param key La valeur SDLK.
    * @param justNow Indique si <code>true</code> ne doit être retourné que si la touche vient d'être enfoncée.
    * @return <code>true</code> si la touche est enfoncée (ou juste enfoncée).
    */
    bool isKeyPressed(int key, bool justNow = false) const;
    /**
    * @brief Indique si une touche est relâchée.
    * @param key La valeur SDLK.
    * @param justNow Indique si <code>true</code> ne doit être retourné que si la touche vient d'être relâchée.
    * @return <code>true</code> si la touche est relâchée (ou juste relâchée).
    */
    bool isKeyReleased(int key, bool justNow = false) const;
    /**
    * @brief Indique si le bouton gauche de la souris est enfoncé.
    * @param justNow Indique si <code>true</code> ne doit être retourné que si le bouton vient d'être enfoncé.
    * @return <code>true</code> si le bouton est enfoncé (ou juste enfoncé).
    */
    bool isLeftMouseButtonClicked(bool justNow = false) const;
    /**
    * @brief Indique si le bouton gauche de la souris est relâché.
    * @param justNow Indique si <code>true</code> ne doit être retourné que si le bouton vient d'être relâché.
    * @return <code>true</code> si le bouton est relâché (ou juste relâché).
    */
    bool isLeftMouseButtonReleased(bool justNow = false) const;
    /**
    * @brief Indique si le bouton droit de la souris est enfoncé.
    * @param justNow Indique si <code>true</code> ne doit être retourné que si le bouton vient d'être enfoncé.
    * @return <code>true</code> si le bouton est enfoncé (ou juste enfoncé).
    */
    bool isRightMouseButtonClicked(bool justNow = false) const;
    /**
    * @brief Indique si le bouton droit de la souris est relâché.
    * @param justNow Indique si <code>true</code> ne doit être retourné que si le bouton vient d'être relâché.
    * @return <code>true</code> si le bouton est relâché (ou juste relâché).
    */
    bool isRightMouseButtonReleased(bool justNow = false) const;
    /*
    * Accesseurs
    * ----------
    */
    const PfPoint& getMouseCoord() const {return m_mouseCoord;} //!< Accesseur.
    int getMouseRelX() const {return m_mouseRelX;} //!< Accesseur.
    int getMouseRelY() const {return m_mouseRelY;} //!< Accesseur.
    bool isQuitting() const {return m_quitting;} //!< Accesseur.
    const vector<unsigned char>& getInputText() const {return m_inputText_v;} //!< Accesseur.

private:
    /**
    * @brief Gère l'entrée de texte.
    * @param code Un scancode de touche SDL.
    *
    * Les touches sont traitées pour générer un texte ASCII (pour l'instant toujours un seul caractère).
    */
    unsigned char handleTextKey(SDL_Scancode code);

    map<SDL_Keycode, EventHandlerCode> m_keys_map; //!< La map des touches SDLK.
    EventHandlerCode m_leftMouse; //!< Indique si le bouton gauche de la souris est enfoncé.
    EventHandlerCode m_rightMouse; //!< Indique si le bouton droit de la souris est enfoncé.
    PfPoint m_mouseCoord; //!< La position du curseur de la souris (coordonnées SDL).
    int m_mouseRelX; //!< Le déplacement en X de la souris, en points SDL.
    int m_mouseRelY; //!< Le déplacement en Y de la souris, en points SDL.
    bool m_quitting; //!< L'événement SDL_QUIT.
    vector<unsigned char> m_inputText_v; //!< Le texte entré sous forme d'une liste de caractères.
    bool m_caret; //!< Indique si l'accent circonflexe est en attente.
    bool m_umlaut; //!< Indique si le tréma est en attente.
    bool m_tilde; //!< Indique si le tilde est en attente.
};

#endif // EVENTHANDLER_H_INCLUDED

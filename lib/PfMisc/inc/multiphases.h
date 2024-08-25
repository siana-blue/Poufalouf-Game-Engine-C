/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe MultiPhases.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef MULTIPHASES_H_INCLUDED
#define MULTIPHASES_H_INCLUDED

#include "misc_gen.h"

#include <string>
#include <set>

/**
* @brief Classe abstraite définissant un objet dont le comportement dépend de phases.
*
* Cette classe est typiquement héritée par héritage multiple.
*
* Il s'agit d'une interface adaptée aux classes pouvant changer de comportement du tout au tout en fonction de l'état du programme.
* Typiquement, les modèles du pattern MVC peuvent hériter de MultiPhases. En fonction de la phase en cours, leurs réactions aux instructions du modèle
* sont différentes.
*
* Cette interface permet d'associer un nom (string) à un état de l'objet indiquant sa phase. Il ne s'agit ni plus ni moins que d'automatiser, via les
* méthodes redéfinies par les classes filles, des actions récurrentes liées à des phases et les actions de sortie de ces phases.
*
* Le booléen MultiPhases::m_newPhase permet de savoir si l'objet est tout juste entré dans une phase ou non. Ce booléen peut être utilisé
* dans la méthode MultiPhases::processCurrentPhase, redéfinie, et est ensuite automatiquement repassé à l'état faux s'il était vrai.
*
* MultiPhases::m_newPhase est à l'état vrai à la construction.
*/
class MultiPhases
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur MultiPhases.
    */
    MultiPhases();
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Change la phase en cours.
    * @param phaseName Le nom de la nouvelle phase.
    * @throw ArgumentException si la phase passée en paramètre n'existe pas.
    *
    * Le booléen MultiPhases::m_newPhase passe à l'état vrai, même si le paramètre correspond déjà à la phase en cours.
    *
    * Si le paramètre <em>phaseName</em> est différent de la phase en cours, la méthode MultiPhases::exitCurrentPhase est appelée.
    */
    void switchPhase(const string& phaseName);
    /**
    * @brief Ajoute une phase.
    * @param phaseName le nom de la phase à ajouter.
    *
    * Si une phase du même nom existe déjà, rien n'est fait.
    */
    void addPhase(const string& phaseName);
    /**
    * @brief Joue la phase en cours.
    *
    * Cette méthode appelle la méthode virtuelle pure privée MultiPhases::processCurrentPhase.
    * Elle gère le booléen MultiPhases::m_newPhase et le passe à l'état faux dès que le premier tour dans la phase en cours est jouée
    * (dès que MultiPhases::processCurrentPhase est exécutée).
    */
    void playCurrentPhase();
    /*
    * Accesseurs
    * ----------
    */
    const string& getCurrentPhaseName() const {return m_currentPhaseName;} //!< Accesseur.
    bool isNewPhase() const{return m_newPhase;} //!< Accesseur.

private:
    /**
    * @brief Réalise les actions associées à la phase en cours.
    */
    virtual void processCurrentPhase() = 0;
    /**
    * @brief Réalise les actions associées à la sortie de la phase en cours.
    */
    virtual void exitCurrentPhase() = 0;

    set<string> m_phasesNames_set; //!< La liste des noms de phases.
    string m_currentPhaseName; //!< Le nom de la phase en cours.
    bool m_newPhase; //!< Indique si cet objet vient d'entrer dans la phase en cours.
};

#endif // MULTIPHASES_H_INCLUDED

/**
* @file
* @author Anaïs Vernet
* @brief Fichier regroupant les types d'exceptions pouvant être levées par les programmes liés aux jeux Poufalouf.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef ERRORS_H_INCLUDED
#define ERRORS_H_INCLUDED

#include "misc_gen.h"

#include <stdexcept>

/**
* @brief Exception pouvant être levée par les programmes liés aux jeux Poufalouf.
*
* Toutes les autres exceptions de ce fichier héritent de cette classe.
*
* Ces exceptions peuvent être construites de deux manières différentes :
* <ul><li>par le constructeur 1, définissant un message pour l'exception,</li>
* <li>par le constructeur 2, définissant un message pour l'exception et liant ce message à une autre exception, ce qui se traduit par
* la succession du message de cette exception et de celui de l'exception liée. Ainsi, il est possible en liant une à une les exceptions de
* faire remonter une série d'événements, le plus haut dans la pile (le plus général) étant placé en premier dans la liste.</li></ul>
*
* Les exceptions héritant de cette classe proposent dans leur constructeur un message type complété par différents arguments.
*/
class PfException : public runtime_error
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur PfException 1.
    * @param line Le numéro de la ligne où l'exception est levée, typiquement __LINE__.
    * @param q_fileName Le nom du fichier source où l'exception est levée, typiquement __FILE__.
    * @param message Le message à afficher.
    *
    * Cette exception affiche le message suivant :
    *
    * Ligne <em>line</em>, fichier <em>q_fileName</em> : <em>message</em>
    */
    PfException(int line, const char* q_fileName, const string& message);
    /**
    * @brief Constructeur PfException 2.
    * @param line Le numéro de la ligne où l'exception est levée, typiquement __LINE__.
    * @param q_fileName Le nom du fichier source où l'exception est levée, typiquement __FILE__.
    * @param message Le message à afficher.
    * @param e L'exception à lier à celle-ci.
    *
    * Cette exception affiche le message suivant :
    *
    * Ligne <em>line</em>, fichier <em>q_fileName</em> : <em>message</em>
    *
    * <code>e.what()</code>
    */
    PfException(int line, const char* q_fileName, const string& message, const PfException& e);
};

/**
* @brief Exception levée par un constructeur.
*/
class ConstructorException : public PfException
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur ConstructorException 1.
    * @param line Le numéro de la ligne où l'exception est levée, typiquement __LINE__.
    * @param q_fileName Le nom du fichier source où l'exception est levée, typiquement __FILE__.
    * @param message Le message à afficher.
    * @param className Le nom de la classe n'ayant pas pu être construite.
    *
    * Ce constructeur appelle le constructeur PfException 1 avec pour message :
    *
    * La construction a échoué, classe <em>className</em> : <em>message</em>
    */
    ConstructorException(int line, const char* q_fileName, const string& message, const string& className);
    /**
    * @brief Constructeur ConstructorException 2.
    * @param line Le numéro de la ligne où l'exception est levée, typiquement __LINE__.
    * @param q_fileName Le nom du fichier source où l'exception est levée, typiquement __FILE__.
    * @param message Le message à afficher.
    * @param className Le nom de la classe n'ayant pas pu être construite.
    * @param e L'exception à lier à celle-ci.
    *
    * Ce constructeur appelle le constructeur PfException 2 avec pour message :
    *
    * La construction a échoué, classe <em>className</em> : <em>message</em>
    *
    * <code>e.what()</code>
    */
    ConstructorException(int line, const char* q_fileName, const string& message, const string& className, const PfException& e);
};

/**
* @brief Exception levée si un argument passé à une fonction ou à une méthode n'est pas valide.
*/
class ArgumentException : public PfException
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur ArgumentException 1.
    * @param line Le numéro de la ligne où l'exception est levée, typiquement __LINE__.
    * @param q_fileName Le nom du fichier source où l'exception est levée, typiquement __FILE__.
    * @param message Le message à afficher.
    * @param argumentName Le nom de l'argument non valide.
    * @param functionName Le nom de la fonction ou de la méthode concernée.
    *
    * Ce constructeur appelle le constructeur PfException 1 avec pour message :
    *
    * Argument non valide <em>argumentName</em> dans la fonction <em>functionName</em> : <em>message</em>
    */
    ArgumentException(int line, const char* q_fileName, const string& message, const string& argumentName, const string& functionName);
    /**
    * @brief Constructeur ArgumentException 2.
    * @param line Le numéro de la ligne où l'exception est levée, typiquement __LINE__.
    * @param q_fileName Le nom du fichier source où l'exception est levée, typiquement __FILE__.
    * @param message Le message à afficher.
    * @param argumentName Le nom de l'argument non valide.
    * @param functionName Le nom de la fonction ou de la méthode concernée.
    * @param e L'exception à lier à celle-ci.
    *
    * Ce constructeur appelle le constructeur PfException 2 avec pour message :
    *
    * Argument non valide <em>argumentName</em> dans la fonction <em>functionName</em> : <em>message</em>
    *
    * <code>e.what()</code>
    */
    ArgumentException(int line, const char* q_fileName, const string& message, const string& argumentName, const string& functionName, const PfException& e);
};

/**
* @brief Exception levée lors de la lecture d'un fichier.
*/
class FileException : public PfException
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur FileException 1.
    * @param line Le numéro de la ligne où l'exception est levée, typiquement __LINE__.
    * @param q_fileName Le nom du fichier source où l'exception est levée, typiquement __FILE__.
    * @param message Le message à afficher.
    * @param file Le nom du fichier lu.
    *
    * Ce constructeur appelle le constructeur PfException 1 avec pour message :
    *
    * Erreur I/O, fichier <em>file</em> : <em>message</em>
    */
    FileException(int line, const char* q_fileName, const string& message, const string& file);
    /**
    * @brief Constructeur FileException 2.
    * @param line Le numéro de la ligne où l'exception est levée, typiquement __LINE__.
    * @param q_fileName Le nom du fichier source où l'exception est levée, typiquement __FILE__.
    * @param message Le message à afficher.
    * @param file Le nom du fichier lu.
    * @param e L'exception à lier à celle-ci.
    *
    * Ce constructeur appelle le constructeur PfException 2 avec pour message :
    *
    * Erreur I/O, fichier <em>file</em> : <em>message</em>
    *
    * <code>e.what()</code>
    */
    FileException(int line, const char* q_fileName, const string& message, const string& file, const PfException& e);
};

/**
* @brief Exception levée lors de la génération d'un Viewable.
*/
class ViewableGenerationException : public PfException
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur ViewableGenerationException 1.
    * @param line Le numéro de la ligne où l'exception est levée, typiquement __LINE__.
    * @param q_fileName Le nom du fichier source où l'exception est levée, typiquement __FILE__.
    * @param message Le message à afficher.
    * @param itemName Le nom de l'objet représenté.
    *
    * Ce constructeur appelle le constructeur PfException 1 avec pour message :
    *
    * Erreur lors de la génération du Viewable <em>itemName</em> : <em>message</em>
    */
    ViewableGenerationException(int line, const char* q_fileName, const string& message, const string& itemName);
    /**
    * @brief Constructeur ViewableGenerationException 2.
    * @param line Le numéro de la ligne où l'exception est levée, typiquement __LINE__.
    * @param q_fileName Le nom du fichier source où l'exception est levée, typiquement __FILE__.
    * @param message Le message à afficher.
    * @param itemName Le nom de l'objet représenté.
    * @param e L'exception à lier à celle-ci.
    *
    * Ce constructeur appelle le constructeur PfException 2 avec pour message :
    *
    * Erreur lors de la génération du Viewable <em>itemName</em> : <em>message</em>
    *
    * <code>e.what()</code>
    */
    ViewableGenerationException(int line, const char* q_fileName, const string& message, const string& itemName, const PfException& e);
};

/**
* @brief Exception levée lors de la lecture d'une instruction.
*/
class InstructionException : public PfException
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur InstructionException 1.
    * @param line Le numéro de la ligne où l'exception est levée, typiquement __LINE__.
    * @param q_fileName Le nom du fichier source où l'exception est levée, typiquement __FILE__.
    * @param message Le message à afficher.
    * @param instructionName Le nom de l'instruction lue.
    *
    * Ce constructeur appelle le constructeur PfException 1 avec pour message :
    *
    * Erreur lors de l'application de l'instruction <em>instructionName</em> : <em>message</em>.
    */
    InstructionException(int line, const char* q_fileName, const string& message, const string& instructionName);
    /**
    * @brief Constructeur InstructionException 2.
    * @param line Le numéro de la ligne où l'exception est levée, typiquement __LINE__.
    * @param q_fileName Le nom du fichier source où l'exception est levée, typiquement __FILE__.
    * @param message Le message à afficher.
    * @param instructionName Le nom de l'instruction lue.
    * @param e L'exception à lier à celle-ci.
    *
    * Ce constructeur appelle le constructeur PfException 2 avec pour message :
    *
    * Erreur lors de l'application de l'instruction <em>instructionName</em> : <em>message</em>
    *
    * <code>e.what()</code>
    */
    InstructionException(int line, const char* q_fileName, const string& message, const string& instructionName, const PfException& e);
};

/**
* @brief Exception levée lors de la lecture d'un script incorrect.
*/
class ScriptException : public PfException
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur ScriptException 1.
    * @param line Le numéro de la ligne où l'exception est levée, typiquement __LINE__.
    * @param q_fileName Le nom du fichier source où l'exception est levée, typiquement __FILE__.
    * @param message Le message à afficher.
    * @param scriptName Le nom du script.
    * @param errorLine La ligne non valide.
    *
    * Ce constructeur appelle le constructeur PfException 1 avec pour message :
    *
    * Script <em>scriptName</em> non valide, à la ligne :
    *
    * <em>errorLine</em> :
    *
    * <em>message</em>
    */
    ScriptException(int line, const char* q_fileName, const string& message, const string& scriptName, const string& errorLine);
    /**
    * @brief Constructeur ScriptException 2.
    * @param line Le numéro de la ligne où l'exception est levée, typiquement __LINE__.
    * @param q_fileName Le nom du fichier source où l'exception est levée, typiquement __FILE__.
    * @param message Le message à afficher.
    * @param scriptName Le nom du script.
    * @param errorLine La ligne non valide.
    * @param e L'exception à lier à celle-ci.
    *
    * Ce constructeur appelle le constructeur PfException 2 avec pour message :
    *
    * Script <em>scriptName</em> non valide, à la ligne :
    *
    * <em>errorLine</em> :
    *
    * <em>message</em>
    *
    * <code>e.what()</code>
    */
    ScriptException(int line, const char* q_fileName, const string& message, const string& scriptName, const string& errorLine, const PfException& e);
};

#endif // ERRORS_H_INCLUDED


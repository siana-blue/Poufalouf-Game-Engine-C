/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe InstructionReader.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef INSTRUCTIONREADER_H_INCLUDED
#define INSTRUCTIONREADER_H_INCLUDED

#include "misc_gen.h"

#include "enum.h"
#include "datapackage.h"

/**
* @brief Classe abstraite définissant une classe pouvant traiter des instructions.
*
* Cette classe est typiquement héritée par héritage multiple.
*
* La méthode InstructionReader::readInstruction stocke les valeurs passées en paramètres dans les champs d'instruction de cette classe,
* jusqu'à lecture de l'instruction suivante.
* Ceci permet à une classe héritée d'accéder aux spécifications de l'instruction en dehors de la méthode virtuelle pure privée
* InstructionReader::processInstruction.
*
* Ceci est peu utilisé car en pratique, la classe héritant de InstructionReader définit tous les traitements liés dans sa redéfinition de
* InstructionReader::processInstruction, mais il est possible que l'information de l'instruction doive être à nouveau utilisée en dehors.
*
* La méthode InstructionReader::processInstruction accomplit l'action en elle-même, puis remonte à InstructionReader::readInstruction un PfReturnCode.
* Les différentes valeurs typiques de ce code peuvent être :
* <ul><li>RETURN_OK : l'instruction a été traitée correctement,</li>
* <li>RETURN_FAIL : l'instruction n'a pas pu être traitée correctement ou l'action en résultant a échoué,</li>
* <li>RETURN_NOTHING : l'instruction a été ignorée.</li></ul>
*
* Cet interface permet le dialogue entre une classe et un objet protéiforme (comme du marshmallow).
* En gros, un instructeur émet des instructions globales vers un objet aux multiples facettes, tel un modèle de jeu par exemple. Ce dernier peut ensuite
* transmettre ces instructions vers ses composants ou les traiter directement.
* Typiquement, les instructions peuvent être envoyées vers un MultiPhases (l'objet cible héritera donc de InstructionReader et MultiPhases).
* En fonction de la phase en cours, la même instruction a un rôle bien différent et l'objet protéiforme saura faire la différence, tandis que l'instructeur,
* lui, n'a rien à faire de l'état de sa cible.
*
* Un contrôleur réagit aux entrées utilisateur sur le clavier, et a pour simple rôle de convertir ces entrées en instructions compatibles avec tout type
* d'objet InstructionReader. Si l'utilisateur tape 'bas' sur son clavier, l'instruction INSTRUCTION_DOWN est envoyée à la cible du contrôleur, et peu
* importe que cela soit transmis à un objet type personnage, à un menu déroulant ou à des patates d'eau douce, l'ordre est transmis.
*/
class InstructionReader
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur InstructionReader.
    */
    InstructionReader();
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Stocke les valeurs passées en paramètres en tant qu'instruction.
    * @param instruction L'instruction à traiter.
    * @param data Les valeurs à associer à l'instruction.
    * @return Un code indiquant si l'instruction a été traitée.
    *
    * La méthode privée InstructionReader::processInstruction est immédiatement appelée.
    */
    PfReturnCode readInstruction(PfInstruction instruction, const DataPackage& data = DataPackage());
    /**
    * @brief Stocke les valeurs passées en paramètres en tant qu'instruction.
    * @param instruction L'instruction à traiter.
    * @param value Un entier à associer à l'instruction.
    * @return Un code indiquant si l'instruction a été traitée.
    *
    * Un DataPackage est créé avec pour unique valeur <em>value</em>.
    *
    * La méthode privée InstructionReader::processInstruction est immédiatement appelée.
    */
    PfReturnCode readInstruction(PfInstruction instruction, int value);
    /**
    * @brief Retourne le package de valeurs de l'instruction en cours.
    * @return Une référence vers le DataPackage.
    */
    DataPackage& instructionValues();
    /*
    * Accesseurs
    * ----------
    */
    PfInstruction getInstruction() const {return m_instruction;} //!< Accesseur.

private:
    /**
    * @brief Traite l'instruction définie par les valeurs PfInstruction::m_instruction et PfInstruction::m_instructionValues.
    * @return Un code indiquant si l'instruction a été traitée.
    */
    virtual PfReturnCode processInstruction() = 0;

    PfInstruction m_instruction; //!< L'instruction en cours.
    DataPackage m_instructionValues; //!< Les valeurs de l'instruction en cours.
};

#endif // INSTRUCTIONREADER_H_INCLUDED

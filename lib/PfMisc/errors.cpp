#include "errors.h"

#include "misc.h"

// PfException

PfException::PfException(int line, const char* q_fileName, const string& message) :
    runtime_error("Ligne " + itostr(line) + ", fichier " + string(q_fileName) + " : " + message) {}

PfException::PfException(int line, const char* q_fileName, const string& message, const PfException& e) :
    runtime_error("Ligne " + itostr(line) + ", fichier " + string(q_fileName) + " : " + message + "\n" + e.what()) {}

// ConstructorException

ConstructorException::ConstructorException(int line, const char* q_fileName, const string& message, const string& className) :
    PfException(line, q_fileName, "La construction a échoué, classe " + className + " : " + message) {}

ConstructorException::ConstructorException(int line, const char* q_fileName, const string& message, const string& className, const PfException& e) :
    PfException(line, q_fileName, "La construction a échoué, classe " + className + " : " + message, e) {}

// ArgumentException

ArgumentException::ArgumentException(int line, const char* q_fileName, const string& message, const string& argumentName, const string& functionName) :
    PfException(line, q_fileName, "Argument non valide " + argumentName + " dans la fonction " + functionName + " : " + message) {}

ArgumentException::ArgumentException(int line, const char* q_fileName, const string& message, const string& argumentName, const string& functionName, const PfException& e) :
    PfException(line, q_fileName, "Argument non valide " + argumentName + " dans la fonction " + functionName + " : " + message, e) {}

// FileException

FileException::FileException(int line, const char* q_fileName, const string& message, const string& file) :
    PfException(line, q_fileName, "Erreur I/O, fichier " + file + " : " + message) {}

FileException::FileException(int line, const char* q_fileName, const string& message, const string& file, const PfException& e) :
    PfException(line, q_fileName, "Erreur I/O, fichier " + file + " : " + message, e) {}

// ViewableGenerationException

ViewableGenerationException::ViewableGenerationException(int line, const char* q_fileName, const string& message, const string& itemName) :
    PfException(line, q_fileName, "Erreur lors de la génération du Viewable " + itemName + " : " + message) {}

ViewableGenerationException::ViewableGenerationException(int line, const char* q_fileName, const string& message, const string& itemName, const PfException& e) :
    PfException(line, q_fileName, "Erreur lors de la génération du Viewable " + itemName + " : " + message, e) {}

// InstructionException

InstructionException::InstructionException(int line, const char* q_fileName, const string& message, const string& instructionName) :
    PfException(line, q_fileName, "Erreur lors de l'application de l'instruction " + instructionName + " : " + message) {}

InstructionException::InstructionException(int line, const char* q_fileName, const string& message, const string& instructionName, const PfException& e) :
    PfException(line, q_fileName, "Erreur lors de l'application de l'instruction " + instructionName + " : " + message, e) {}

// ScriptException

ScriptException::ScriptException(int line, const char* q_fileName, const string& message, const string& scriptName, const string& errorLine) :
    PfException(line, q_fileName, "Script " + scriptName + " non valide, à la ligne :\n" + errorLine + " :\n" + message) {}

ScriptException::ScriptException(int line, const char* q_fileName, const string& message, const string& scriptName, const string& errorLine, const PfException& e) :
    PfException(line, q_fileName, "Script " + scriptName + " non valide, à la ligne :\n" + errorLine + " :\n" + message, e) {}

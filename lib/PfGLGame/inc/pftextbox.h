/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe PfTextBox.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef PFTEXTBOX_H_INCLUDED
#define PFTEXTBOX_H_INCLUDED

#include "glgame_gen.h"

#include "pfwidget.h"

/**
* @brief Boîte de texte.
*
* Ce widget est composé d'une boîte dans laquelle l'utilisateur peut cliquer puis taper du texte.
*
* Une boîte de texte peut ne pas accepter les entrées utilisateur, et afficher un texte elle-même suivant des instructions données par le programme.
* Dans ce cas, elle peut l'afficher en une fois ou opérer une écriture progressive caractère par caractère jusqu'à l'affichage du texte complet.
*/
class PfTextBox : public PfWidget
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur PfTextBox.
		* @param name le nom de cette boîte de texte.
		* @param rect le rectangle de coordonnées de ce widget.
		* @param layer le plan de perspective de ce widget.
		* @param hMargin la marge horizontale du texte de ce bouton.
        * @param vMargin la marge verticale du texte de ce bouton.
        * @param textureIndex l'indice de texture de la police.
		* @param statusMap la valeur d'initialisation de la map d'association animation <-> état de ce widget.
		*/
		PfTextBox(const string& name, const PfRectangle& rect, unsigned int layer = MAX_LAYER, float hMargin = 0.0, float vMargin = 0.0,
                  unsigned int textureIndex = FONT_TEXTURE_INDEX, PfWidget::PfWidgetStatusMap statusMap = WIDGET_STANDARD_MAP);
		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Ajoute un caractère à cette boîte.
		* @param c le caractère à ajouter.
		*
		* Si PfTextBox::m_fileNameChar est vrai, alors les seuls caractères acceptés sont a-zA-Z0-9_.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*/
		void addChar(char c);
		/**
		* @brief Supprime la dernière lettre de cette boîte.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*/
		void backspace();
		/**
		* @brief Efface le texte de cette boîte.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*/
		void reset();
        /*
        * Redéfinitions
        * -------------
        */
		/**
		* @brief Modifie le texte de cette boîte.
		* @param text le nouveau texte.
		*
		* Si PfTextBox::m_fileNameChar est vrai, alors les seuls caractères acceptés sont a-zA-Z0-9_.
		* Un texte non compatible ne sera pas ajouté.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*/
		virtual void changeText(const string& text);
		/**
		* @brief Désurligne ce widget.
		*
		* Appelle la méthode PfWidget::leave.
		*
		* Masque le curseur.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*/
		virtual void leave();
		/**
		* @brief Active ce widget.
		*
		* Appelle la méthode PfWidget::activate.
		*
		* Affiche le curseur.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*
		* @remarks
		* La sélection et l'activation ont le même effet.
		*/
		virtual void select();
		/**
		* @brief Déselectionne ce widget.
		*
		* Appelle la méthode PfWidget::deselect.
		*
		* Masque le curseur.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*/
		virtual void deselect();
		/**
		* @brief Active ce widget.
		*
		* Appelle la méthode PfWidget::activate.
		*
		* Affiche le curseur.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*
		* @remarks
		* La sélection et l'activation ont le même effet.
		*/
		virtual void activate();
		/**
		* @brief Met à jour ce ModelItem.
		*
		* Appelle la méthode PfWidget::update, puis, si PfTextBox::m_typingMode est différent de PfText::TEXT_TYPING_ALLATONCE,
		* attribue systématiquement la valeur vraie à ModelItem::m_modified.
		*/
		virtual void update();
        /**
        * @brief Génère un Viewable à partir de ce ModelItem.
        * @return le Viewable créé.
        * @throw ViewableGenerationException si une erreur survient dans la gestion de l'affichage du texte.
        *
        * @warning
        * De la mémoire est allouée pour le pointeur retourné.
        */
        virtual Viewable* generateViewable() const;
		/**
		* @brief Traite l'instruction passée en paramètre.
		* @return RETURN_OK si l'instruction a été traitée, RETURN_NOTHING sinon.
		* @throw ArgumentException si la valeur associée à l'instruction INSTRUCTION_SELECT n'est pas une valeur char valide.
		*
		* <ul><li>INSTRUCTION_ACTIVATE : affiche le curseur.</li>
		* <li>INSTRUCTION_MODIFY : la valeur indique la lettre à ajouter au texte. Ceci ne fonctionne que si le curseur est visible.</li>
		* <li>INSTRUCTION_DELETE : efface la dernière lettre du texte. Ceci ne fonctionne que si le curseur est visible.</li></ul>
		*/
		virtual PfReturnCode processInstruction();
		/*
		* Accesseurs
		* ----------
		*/
		const string& getText() const {return m_text;}
		bool isCursorShown() const {return m_cursorShown;}
		bool isFileNameChar() const {return m_fileNameChar;}
		void setFileNameChar(bool fileNameChar) {m_fileNameChar = fileNameChar;}
		void setMultiLine(bool multiLine) {m_multiLine = multiLine;}
		void setLinesCount(unsigned int linesCount) {m_linesCount = linesCount;}

	private:
		string m_text; //!< Le texte de cette boîte.
		bool m_cursorShown; //!< Indique si le curseur est indiqué.
		bool m_fileNameChar; //!< Indique si les caractères acceptés ne sont que des caractères compatibles avec les noms de fichier (a-zA-Z0-9_).
		bool m_multiLine; //!< Indique si cette boîte de texte accepte plusieurs lignes.
		unsigned int m_linesCount; //!< Le nombre de lignes d'affichage sur cette boîte de texte.
		float m_hMargin; //!< La marge horizontale de centrage du texte.
		float m_vMargin; //!< La marge verticale de centrage du texte.
		unsigned int m_fontTextureIndex; //!< L'indice de texture de la police du texte.
};

#endif // PFTEXTBOX_H_INCLUDED

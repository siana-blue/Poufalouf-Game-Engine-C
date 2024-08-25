/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe PfSlidingCursor.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef PFSLIDINGCURSOR_H_INCLUDED
#define PFSLIDINGCURSOR_H_INCLUDED

#include "glgame_gen.h"

#include "pfwidget.h"

/**
* @brief Curseur glissant.
*
* Ce widget est composé d'un curseur défilant sur une barre verticale ou horizontale.
*
* La barre de défilement est divisée en pas associés à une valeur, et éventuellement accompagnés d'un texte.
*
* Le rectangle de coordonnées donné au constructeur de cette classe englobe l'ensemble barre + curseur + textes.
* Le constructeur se charge de répartir les différents composants dans ce rectangle.
* Si au moins un texte est présent, alors le texte prendra un quart (3/4 en vertical) du rectangle,
* sinon, seuls la barre et le curseur occupent l'ensemble du rectangle.
*
* Les pas sont comptés à partir de 0.
*
* @section AnimDef Définition des animations dans le wad
*
* La texture du curseur est déterminée par le WAD.
* La barre de défilement est située dans le même fichier, sur les deux cases horizontales de la ligne du haut.
*
* Pour le calcul des coordonnées de texture des éléments de ce widget, il est nécessaire que l'image du curseur soit de dimensions carrées.
*
* Dans le script du wad, c'est la texture du curseur qui est visée lors de la définition des frames.
* L'image de la barre est automatiquement prise dans le même fichier image, selon la description du paragraphe ci-dessus.
*/
class PfSlidingCursor : public PfWidget
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur PfSlidingCursor.
		* @param name le nom de ce curseur.
		* @param rect le rectangle des coordonnées de ce widget.
		* @param steps le nombre de pas de cette barre.
		* @param vertical vrai pour une barre verticale, horizontale sinon.
		* @param layer le plan de perspective de ce widget.
		* @throw ConstructorException si une erreur survient lors de la construction.
		*/
		PfSlidingCursor(const string& name, const PfRectangle& rect, unsigned int steps, bool vertical = false, int layer = MAX_LAYER);
		/**
		* @brief Destructeur PfSlidingCursor.
		*
		* Détruit les éléments de ce curseur.
		*/
		virtual ~PfSlidingCursor();
		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Ajoute un texte à ce curseur.
		* @param text le texte à ajouter.
		*
		* Il est possible d'ajouter plus de textes que de pas, mais les textes excédentaires ne seront pas affichés.
		*/
		void addText(const string& text);
		/**
		* @brief Change le pas actuel.
		* @param step le nouveau pas.
		* @throw ArgumentException si le pas n'est pas valide.
		*
		* Cette méthode met à jour le polygone de position du curseur.
		*
		* Ce ModelItem est marqué modifié.
		*/
		void changeStep(unsigned int step);
		/*
		* Redéfinitions
		* -------------
		*/
		/**
		* @brief Ajoute une animation au curseur de ce widget.
        * @param p_anim l'animation à ajouter.
        * @param value le statut de l'animation.
        * @throw PfException si une exception survient lors de l'ajout.
		*
		* Si une animation existe déjà pour ce statut, elle est remplacée.
		*
        * Le pointeur à ajouter ne doit pas être nul. Ce pointeur sera
        * détruit par le destructeur du PfAnimationGroup de cette classe.
		*
		* Appelle la méthode AnimatedGLItem::addAnimation sur le curseur de ce widget.
		*
		* L'animation ANIM_IDLE de la barre principale est créée si la valeur du paramètre value est ANIM_IDLE.
		* Cette animation est construite avec le même indice de texture, comme indiqué dans la description de cette classe.
		*/
		virtual void addAnimation(PfAnimation* p_anim, PfAnimationStatus value);
        /**
		* @brief Sélectionne ce widget.
		*
		* Appelle la méthode PfWidget::select.
		*
		* Annule les effets de ce widget.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*/
		virtual void select();
		/**
		* @brief Déselectionne ce widget.
		*
		* Appelle la méthode PfWidget::deselect.
		*
		* Le booléen PfSlidingCursor::m_readyToSlide passe à l'état faux.
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
		* Si le point d'activation est sur le curseur, alors le booléen PfSlidingCurosr::m_readyToSlide passe à l'état vrai (il passe à l'état faux dans le cas contraire).
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*/
		virtual void activate();
		/**
		* @brief Action à réaliser lorsque ce widget subit un glisser-déplacer.
		* @return faux si aucun déplacement n'a lieu.
		*
		* Si PfSlidingCursor::m_readyToSlide est vrai, alors fait glisser le curseur.
		* L'effet de ce widget passe à EFFECT_NEXT ou EFFECT_PREV avec pour valeur le nombre de pas à effectuer, déterminé au moyen du point d'action enregistré par ce widget.
		*
		* @remarks
		* Ce ModelItem est marqué modifié si un déplacement a lieu.
		*/
		virtual bool slide();
		/**
		* @brief Met à jour ce curseur.
		*
		* Appelle la méthode PfWidget::update.
		*
		* Met à jour la position précédente de ce curseur avec la valeur de la position actuelle.
		*/
		virtual void update();
        /**
        * @brief Génère un Viewable à partir de ce ModelItem.
        * @return le Viewable créé.
        *
        * @warning
        * De la mémoire est allouée pour le pointeur retourné.
        */
        virtual Viewable* generateViewable() const;
		/*
		* Accesseurs
		* ----------
		*/
		unsigned int getStepsCount() const {return m_stepsCount;}
		void setStepsCount(unsigned int count) {m_stepsCount = count;}
		bool isVertical() const {return m_vertical;}
		unsigned int getCurrentStep() const {return m_currentStep;}
		bool isReadyToSlide() const {return m_readyToSlide;}
		void setReadyToSlide(bool slide) {m_readyToSlide = slide;}

	private:
		unsigned int m_stepsCount; //!< Le nombre de pas.
		bool m_vertical; //!< Indique si ce curseur est vertical.
		vector<string> m_texts_v; //!< La liste des textes de ce curseur.
		AnimatedGLItem* mp_rail; //!< Le rail du curseur.
		AnimatedGLItem* mp_cursor; //!< Le curseur.
		unsigned int m_currentStep; //!< Le pas actuel.
		unsigned int m_previousStep; //!< Le pas précédent (pour gérer le slide).
		bool m_readyToSlide; //!< Indique si le curseur accepte le "slide".
};

#endif // PFSLIDINGCURSOR_H_INCLUDED

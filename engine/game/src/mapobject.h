/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe MapObject.
* @date 31/07/2016
*/

#ifndef MAPOBJECT_H_INCLUDED
#define MAPOBJECT_H_INCLUDED

#include "gen.h"
#include <fstream>
#include <map>
#include <vector>
#include "glitem.h"
#include "serializable.h"
#include "instructionreader.h"
#include "geometry.h"
#include "enum.h"
#include "mapzone.h"

/**
* @brief Objet d'une map 2D vue de haut.
*
* La vitesse verticale d'un objet peut être une fraction de z.
* La conversion se fait de sorte que la valeur flottante devienne une valeur entière la plus éloignée de 0.
* Par exemple, -1.25 devient -2 tandis que 3.75 devient 4.
* Ceci permet de ralentir lentement un objet, au moyen d'une accélération fixe (la gravité notamment, fraction de 1).
*
* Un objet dispose d'un statut MapObject::m_objStat.
* Ce statut indique l'état d'un objet (volant, sautant, tombant, recevant des dommages etc...).
* Ces statuts sont indiqués sous forme de flags, énumération PfObjectStatus (fichier "enum.h").
*
* La classe MapObject réserve le statut OBJSTAT_ONTHEFLOOR pour sa propre gestion de chute, de saut etc...
* Tout autre statut est géré par les classes héritant de celle-ci.
* Le MapModel a également quelques comportements dépendant de ce statut.
*
* Le code objet permet d'associer une propriété à un objet, à définir selon le type d'objet.
*
* Un MapObject possède une liste de zones servant à définir la collision, la sensibilité à l'activation etc...
* Chaque zone est repérée dans la map MapObject::m_zones_v_map, avec pour clé un élément de l'énumération PfBoxType (fichier "enum.h").
* Pour chaque type de zone, les zones correspondantes sont stockées dans un vecteur. En effet, il est possible pour un MapObject d'avoir plusieurs
* zones de collision par exemple, la première étant prise par défaut, mais certaines animations ou frames d'animation pouvant imposer l'utilisation
* d'une autre.
*
* Pour définir quelle zone est actuellement utilisée, la map MapObject::m_zonesIndex_map associe l'indice du vecteur à utiliser pour chaque
* PfBoxType.
*
* Les animations d'un MapObject peuvent également définir un changement de zone (de collision par exemple).
* Pour cela, le membre MapObject::m_boxAnimLinks_v_map est utilisé.
* Dans la redéfinition de la méthode AnimatedGLItem::changeCurrentAnimation, cette map est consultée.
* Si un vecteur est présent pour le statut d'animation concerné, les zones affectées sont modifiées en fonction, grâce à l'indice
* de zone spécifié.
*
* Le centre d'un MapObject est défini par le champ MapObject::m_center. Il s'agit d'un point dont les coordonnées sont relatives au coin inférieur gauche
* de l'objet, et exprimées en fractions des dimensions l'objet.
* Ce centre est notamment utilisé pour le placement d'un objet sur la map dans l'éditeur.
*/
class MapObject : public AnimatedGLItem, public Serializable, public InstructionReader
{
	public:
	    /**
		* @brief Enumération des sections de sauvegarde d'un MapObject.
		*/
		enum SaveMapObject {SAVE_ORIENTATION, //!< section de sauvegarde de l'orientation
                            SAVE_COORD, //!< section de sauvegarde des coordonnées
                            SAVE_Z, //!< section de sauvegarde de la hauteur
                            SAVE_SPEED, //!< section de sauvegarde des vitesses
                            SAVE_CODEOBJ, //!< section de sauvegarde du code objet
                            SAVE_END = SAVE_END_VALUE //!< fin de sauvegarde
                            };

		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur MapObject.
		* @param name le nom de cet objet.
		* @param rect le rectangle de coordonnées de cet objet.
		* @param orientation l'orientation de cet objet.
		*
		* @remarks
		* Le rectangle de coordonnées sert surtout aux coordonnées w et h,
		* car les coordonnées x et y seront normalement définies par la map en fonction de la case où cet objet se trouvera.
		*/
		MapObject(const string& name = "", const PfRectangle& rect = PfRectangle(), PfOrientation::PfOrientationValue orientation = PfOrientation::NO_ORIENTATION);
		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Déplace cet objet sur la map.
		* @param steps le nombre de pas imposé.
		*
		* Si <em>steps</em> n'est pas nul, alors le mouvement se fait selon ce paramètre, sinon, c'est la vitesse de cet objet qui compte.
		*
		* Cette méthode appelle GLItem::shift, et met à jour les zones de cet objet.
		*
		* Aucun calcul de collision n'est fait par cette méthode.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*/
		void move(unsigned int steps = 0);
		/**
		* @brief Déplace cet objet sur la map sans tenir compte de l'orientation de cet objet.
		* @param orientation l'orientation du déplacement.
		* @param scale l'échelle de déplacement.
		*
		* Cette méthode appelle GLItem::shift, et met à jour les zones de cet objet.
		*
		* C'est le paramètre <em>scale</em> ou la norme du vecteur d'orientation qui détermine la distance parcourue.
		*
		* Aucun calcul de collision n'est fait par cette méthode.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*/
		void move(PfOrientation orientation, double scale = 1.0);
		/**
		* @brief Retourne un rectangle indiquant le chemin à parcourir par cet objet.
		* @return le rectangle de parcours.
		*
		* Le parcours est calculé en fonction de l'orientation et de la vitesse de cet objet.
		* Aucune collision n'est prise en compte.
		*
		* Le rectangle de parcours prend pour départ le rectangle de collision de cet objet et se termine à une distance
		* égale à la taille d'un pas multipliée par la vitesse de cet objet.
		* Le rectangle retourné est juxtaposé au rectangle de collision sans recouvrement.
		*
		* Pour une orientation nord par exemple, le rectangle de parcours partira du haut du rectangle de collision (y+h),
		* tandis que pour une orientation ouest, il partira de la gauche (x).
		*/
		PfRectangle pathRect() const;
		/**
		* @brief Indique si cet objet est en collision avec un autre.
		* @param rc_object l'objet avec lequel tester la collision.
		* @param dz le déplacement Z à affecter à cet objet avant correction de la collision.
		* @param borders vrai si deux objets se touchant uniquement par les bordures (non recouvrement) doivent retourner vrai.
		* @return vrai si la collision a lieu.
		*
		* Le rectangle de collision de l'objet passé en paramètre est déplacé pour corriger la différence de Z entre cet objet et l'objet passé en paramètre.
		* Ceci s'ajoute à la correction <em>dz</em> effectuée sur cet objet.
		*
		* Si, après cette correction, les rectangles se recouvrent et que les altitudes des objets concordent, alors vrai est retourné.
		*/
		bool isColliding(const MapObject& rc_object, int dz = 0, bool borders = false) const;
		/**
		* @brief Calcule la distance à parcourir par cet objet avant d'entrer en collision avec l'objet passé en paramètre.
		* @param rc_object l'objet de collision.
		* @return la distance à parcourir avant collision.
		*
		* La dimension Z est prise en compte pour vérifier si la collision a lieu ou non.
		*
		* La valeur retournée est toujours positive.
		*/
		float distanceBeforeCollision(const MapObject& rc_object) const;
		/**
		* @brief Modifie la hauteur de cet objet.
		* @param z la nouvelle hauteur.
		*
		* L'image de cet objet est mise à jour sur l'axe Y.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*/
		void changeZ(int z);
		/**
		* @brief Modifie la vitesse Z (valeur flottante) de cet objet.
		* @param vz l'incrément de vitesse à apporter.
		*/
		void addSpeedZ(float vz);
		/**
		* @brief Retourne une valeur de vitesse Z entière à partir de la valeur flottante de cet objet.
		* @return la valeur de vitesse Z entière.
		*
		* La conversion se fait de sorte que la valeur flottante devienne une valeur entière la plus éloignée de 0.
		* Par exemple, -1.25 devient -2 tandis que 3.75 devient 4.
		*/
		int speedZ() const;
		/**
		* @brief Retourne la zone spécifiée en paramètre, en tenant compte de l'indice actuel correspondant.
		* @param type le type de zone à retourner.
		* @param returnInhibited vrai si la méthode doit retourner la zone même si elle est inhibée.
		* @return un pointeur vers la zone, ou 0 si cet objet ne possède pas de telle zone ou si la zone est inhibée.
		*
		* Attention : la zone BOX_TYPE_MAIN n'est pas stockée dans l'objet sous forme de MapZone, elle définit la géométrie de l'objet.
		* Ce paramètre renverra donc un pointeur nul.
		*/
		MapZone* zone(PfBoxType type, bool returnInhibited = false);
		/**
		* @brief Retourne la zone spécifiée en paramètre, en tenant compte de l'indice actuel correspondant.
		* @param type le type de zone à retourner.
		* @param returnInhibited vrai si la méthode doit retourner la zone même si elle est inhibée.
		* @return un pointeur constant vers la zone, ou 0 si cet objet ne possède pas de telle zone ou si la zone est inhibée.
		*
		* Attention : la zone BOX_TYPE_MAIN n'est pas stockée dans l'objet sous forme de MapZone, elle définit la géométrie de l'objet.
		* Ce paramètre renverra donc un pointeur nul.
		*/
		const MapZone* constZone(PfBoxType type, bool returnInhibited = false) const;
		/**
		* @brief Ajoute une zone à cet objet.
		* @param type le type de zone à ajouter.
		* @param zone la zone à ajouter.
		*
		* Si une zone du même type existe déjà, la nouvelle zone est ajoutée à la suite du vecteur.
		*
		* Les coordonnées de la zone doivent être données à partir de (0;0). La zone est ensuite déplacée d'une valeur égale aux coordonnées
		* (X, Y) de cet objet. Ainsi, si la zone a pour coordonnées (0.01;0), elle sera déplacée pour atteindre les coordonnées de l'objet + 0.01
		* en abscisse.
		*/
		void addZone(PfBoxType type, const MapZone& zone);
		/**
		* @brief Retourne l'indice de zone à utiliser pour le type spécifié.
		* @param type le type de zone concerné.
		* @return l'indice de zone.
		* @throw ArgumentException si aucune zone n'est trouvée pour le type spécifié.
		*/
		unsigned int zoneIndex(PfBoxType type) const;
		/**
		* @brief Change l'indice de zone à utiliser pour le type spécifié.
		* @param type le type de zone à considérer.
		* @param index l'indice de zone à utiliser.
		* @throw ArgumentException si l'indice n'est pas valide.
		*
		* Si le type de zone n'est pas trouvé, ne fait rien.
		*/
		void changeZoneIndex(PfBoxType type, unsigned int index);
		/**
		* @brief Active cet objet.
		* @param code un code d'activation PfActivationCode indiquant le type d'activation effectuée.
		* @return un PfEffect sous forme de flags, par défaut EFFECT_NONE.
		*
		* Par défaut, ajoute simplement l'état OBJSTAT_ACTIVATED à cet objet.
		*/
		virtual pfflag32 activate(pfflag32 code = ACTIVCODE_ANY);
		/**
		* @brief Désactive cet objet.
		* @return un PfEffect sous forme de flags, par défaut EFFECT_NONE.
		*
		* Par défaut, retire simplement l'état OBJSTAT_ACTIVATED de cet objet.
		*/
		virtual pfflag32 deactivate();
		/*
		* Redéfinitions
		* -------------
		*/
		/**
		* @brief Change l'animation en cours.
		* @param value le statut de l'animation à jouer.
		* @param reset vrai si l'animation doit être remise à zéro même si elle n'est pas changée.
		* @return faux si l'animation n'a pas pu être changée ou si c'était la même.
		*
		* Appelle la méthode AnimatedGLItem::changeCurrentAnimation.
		*
		* Puis, modifie les zones de cet objet en fonction du membre MapObject::m_boxAnimLinks_v_map.
		*/
		virtual bool changeCurrentAnimation(PfAnimationStatus value, bool reset = false);
		/**
		* @brief Met à jour ce ModelItem.
		*
		* Les actions suivantes sont réalisées :
		* <ul><li>appel de la méthode AnimatedGLItem::update,</li>
		* <li>appel de la méthode MapObject::updateObjStat,</li>
		* <li>appel de la méthode MapObject::updateAnim,</li>
		* <li>dans le cas où g_debug (fichier "gen.h") est vrai, alors marque systématiquement ce ModelItem comme modifié
		* (afin de générer l'image des zones).</li></ul>
		*/
		virtual void update();
		/**
		* @brief Génère un Viewable à partir de ce ModelItem.
		* @return le Viewable créé.
		* @throw ViewableGenerationException si le Viewable ne peut être généré.
		*
		* Appelle la méthode AnimatedGLItem::generateViewable, puis, dans le cas où g_debug (fichier "gen.h") est vrai, la zone de collision est affichée.
		*
		* @warning
		* De la mémoire est allouée pour le pointeur retourné.
		*/
		virtual Viewable* generateViewable() const;
		/**
		* @brief Sérialise cet objet.
		* @param r_ofs le flux en écriture.
		*
		* Le flux doit être positionné à l'endroit désiré pour l'écriture, il n'est pas rembobiné en fin de méthode.
		*
		* Les données sont stockées de façon à pouvoir être affectées à un DataPackage (indication du type de valeur avant chaque valeur).
		*/
		virtual void saveData(ofstream& r_ofs) const;
		/**
		* @brief Utilise les données sauvegardées pour placer cet objet sur la map.
		* @param r_data les données.
		* @throw PfException si les données ne sont pas valides.
		*
		* @remarks
		* Cet objet est marqué modifié.
		*/
		virtual void loadData(DataPackage& r_data);
		/**
		* @brief Traite l'instruction passée en paramètre.
		* @return RETURN_NOTHING.
		* @throw PfException si l'instruction ne peut pas être traitée.
		*
		* Par défaut, ne fait rien.
		*/
		virtual PfReturnCode processInstruction() {return RETURN_NOTHING;}
		/*
		* Accesseurs
		* ----------
		*/
		int getZ() const {return m_z;}
		int getSpeed() const {return m_speed;}
		void setSpeed(int speed) {m_speed = speed;}
		void setSpeedZ(float speedZ) {m_speedZ = speedZ;}
		pfflag32 getObjStat() const {return m_objStat;}
		void setObjStat(pfflag32 objStat) {m_objStat = objStat;}
		bool isFloating() const {return m_floating;}
		void setFloating(bool floating) {m_floating = floating;}
		int getObjCode() const {return m_objCode;}
		void setObjCode(int code) {m_objCode = code;}
		void setBoxAnimLinks(const map<PfAnimationStatus, vector<pair<PfBoxType, unsigned int> > >& links) {m_boxAnimLinks_v_map = links;}
		const PfPoint& getCenter() const {return m_center;}
		void setCenter(const PfPoint& center) {m_center = center;}

	protected:
		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Met à jour les statuts de cet objet.
		*
		* Pour un MapObject en général, effectue les actions suivantes :
		* <ul><li>nettoyage de certains états ponctuels ou à durée limitée,</li>
		* <li>mise à jour des autres états,</li>
		* <li>entretien de certains états</li></ul>
		*
		* La phase de nettoyage comprend les actions suivantes :
		* <ul><li>si l'objet a l'état OBJSTAT_LANDING, alors il le perd (frame unique).</li></ul>
		*
		* La phase de mise à jour comprend les actions suivantes :
		* <ul><li>si la vitesse Z de cet objet est strictement négative, alors ajouter l'état FALLING,</li>
		* <li>si la vitesse Z de cet objet est nulle et qu'il a l'état OBJSTAT_FALLING,
		* alors perdre les états OBJSTAT_JUMPING et OBJSTAT_FALLING et prendre l'état OBJSTAT_LANDING,</li>
		* <li>si la vitesse horizontale de cet objet est non nulle, alors perdre l'état OBJSTAT_STOPPED.</li></ul>
		*
		* La phase d'entretien, consistant à maintenir certains états en fonction d'autres, comprend les actions suivantes :
		* <ul><li>si l'objet n'a aucun des états OBJSTAT_JUMPING ou OBJSTAT_FALLING, alors prendre l'état OBJSTAT_ONTHEFLOOR ; sinon, le perdre.</li></ul>
		*/
		virtual void updateObjStat();
		/**
		* @brief Met à jour l'animation de cet objet.
		*
		* Dans l'idée, les états ne sont modifiés que par MapObject::updateObjStat.
		* Seules les animations sont modifiées ici.
		* L'exception est l'état STANDBY qui a pour rôle d'indiquer qu'une animation doit se terminer avant de passer à autre chose.
		* Cette méthode manipulera donc le seul état STANDBY.
		*
		* Par défaut, gère l'animation d'activation et de désactivation.
		*/
		virtual void updateAnim();
		/**
		* @brief Ajoute des flags PfObjectStatus à cet objet.
		* @param objStat les flags à ajouter.
		*/
		void addObjStat(PfObjectStatus objStat);
		/**
		* @brief Supprime des flags PfObjectStatus de cet objet.
		* @param objStat les flags à retirer.
		*/
		void removeObjStat(PfObjectStatus objStat);
		/**
		* @brief Supprime tous les flags PfObjectStatus de cet objet.
		*
		* MapObject::m_objStat prend la valeur OBJSTAT_NONE.
		*/
		void resetObjStat();
		/**
		* @brief Ajoute des effets à cet objet.
		* @param effects les effets à ajouter sous forme d'énumération PfEffect.
		*/
		void addEffects(pfflag32 effects);
		/**
		* @brief Supprime tous les flags PfEffect de cet objet.
		*
        * MapObject::m_effects prend la valeur EFFECT_NONE.
		*/
		void resetEffects();

	private:
		int m_z; //!< La hauteur de cet objet.
		int m_speed; //!< La vitesse horizontale de cet objet (sur le plan de la map).
		float m_speedZ; //!< La vitesse verticale de cet objet.
		pfflag32 m_objStat; //!< Le statut de cet objet.
		pfflag32 m_effects; //!< Les effets de cet objet.
		map<PfBoxType, vector<MapZone> > m_zones_v_map; //!< La liste des zones d'effet de cet objet.
		map<PfBoxType, unsigned int> m_zonesIndex_map; //!< La liste des indices de zones à utiliser.
		bool m_floating; //!< Indique si cet objet ignore la gravité.
		int m_objCode; //!< Code objet.
		map<PfAnimationStatus, vector<pair<PfBoxType, unsigned int> > > m_boxAnimLinks_v_map; //!< La liste des liens animations <-> zones.
		PfPoint m_center; //!< Le centre d'un objet, servant notamment à son placement sur la map lors d'un clic dans l'éditeur.
};

#endif // MAPOBJECT_H_INCLUDED


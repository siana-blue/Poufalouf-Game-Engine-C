/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe PNGToGLLoader et la structure PNGDataBuffer.
* @date xx/xx/xxxx
* @version 0.0.0
*
* Réalisé à l'aide de la page <em>http://tfc.duke.free.fr/coding/png.html</em>.
*/

#ifndef PNGTOGLLOADER_H_INCLUDED
#define PNGTOGLLOADER_H_INCLUDED

#include "media_gen.h"

#include <GL/gl.h>
#include <png.h>
#include <string>

/**
* @brief Fonction utilisée en tant que callback lors de la lecture d'un fichier PNG, à présent boîte noire...
* @param p_png Structure libpng
* @param data_t Cherche pas à comprendre, ça fonctionne
* @param length Tiens, Poufalouf a trouvé un Snackirs, là-bas, dans la forêt...
*
* Cette fonction n'est pas documentée, ce fichier est à prendre comme un tout qui, maintenant,
* fonctionne plutôt bien, et j'ai oublié comment je l'ai codé.
*/
void png_read_data(png_structp p_png, png_bytep data_t, png_size_t length);

/**
* @brief Structure permettant de parcourir une liste d'octets.
*
* Pour des raisons de compatibilité avec les procédures de la bibliothèque libpng,
* la liste d'octets n'est pas constante, mais elle n'est en aucun cas modifiée.
*
* Durant l'utilisation de cette structure dans les fonctions callback appelées par libpng, il n'est pas possible de lever d'exception,
* la bibliothèque étant codée en C.
* Un booléen PNGDataBuffer::m_error passe à l'état vrai dès qu'une erreur est levée.
*
* La méthode PNGDataBuffer::isValid indique si une erreur survient lors de la lecture des données.
* Ainsi, dès que la fonction callback est quittée et que l'exécution reprend dans le programme C++,
* un test de validité permet de lever une exception si nécessaire.
*
* Cela est notamment utile pour tester un dépassement de mémoire.
* Si le constructeur PNGDataBuffer est appelé avec une longueur trop importante, une erreur de segmentation peut survenir.
* Dans ce cas, le programme est quitté brusquement.
* Par contre, il est possible que la zone mémoire contigue à celle réservée par la liste d'octets appartienne à cette application.
* Dans ce cas, il n'est pas facilement possible de détecter au debuggage qu'il y a eu dépassement.
* C'est là qu'intervient le test de validité de PNGDataBuffer.
*/
struct PNGDataBuffer
{
	/*
	 * Constructeurs et destructeur
	 * ----------------------------
	 */
	/**
	* @brief Constructeur PNGDataBuffer par défaut.
	*
	* Ce constructeur génère un PNGDataBuffer vide dont le marqueur d'erreur est à l'état vrai.
	*/
	PNGDataBuffer() : length(0), offset(0), m_data_t(0), m_error(true) {}
	/**
	* @brief Constructeur PNGDataBuffer.
	* @param dt_t Le tableau d'octets à copier dans cet objet.
	* @param lgth La longueur du tableau d'octets.
	*
	* L'offset est placé à 0.
	*/
	PNGDataBuffer(char dt_t[], long lgth) : length(lgth), offset(0), m_data_t(dt_t), m_error(false) {}
	/**
	* @brief Destructeur PNGDataBuffer.
	*
	* Détruit le tableau d'octets m_data_t.
	*/
	~PNGDataBuffer() {delete [] m_data_t;}
	/*
	 * Méthodes
	 * --------
	 */
	/**
	* @brief Retourne un tableau d'octets de longueur lgth à partir de l'offset en cours.
	* @param lgth Le nombre d'octets à lire.
	* @return L'adresse du premier octet lu.
	*
	* Si le nombre d'octets lus depuis l'offset conduit à un dépassement de la mémoire allouée au tableau d'octets PNGDataBuffer::m_data_t,
	* le booléen PNGDataBuffer::m_error passe à l'état vrai.
	*
	* En réalité, seule l'adresse du premier octet du tableau est retournée, mais cette méthode s'est assurée que la longueur demandée est valide,
	* grâce au test de validité.
	*/
	inline char* data(long lgth)
	{
		if (offset + lgth > length)
			m_error = true;
		return m_data_t + offset;
	}
	/**
	* @brief Indique si une erreur est survenue lors de la lecture des données de ce PNGDataBuffer.
	* @return <code>false</code> si aucune erreur n'est survenue, <code>true</code> sinon.
	*/
	bool isValid() {return !m_error;}

	long length; //!< La longueur, en octets, du tableau d'octets stocké dans ce PNGDataBuffer.
	long offset; //!< La position de lecture.

private:
    char* m_data_t; //!< Le tableau d'octets stockés.
    bool m_error; //!< Indique si une erreur est survenue lors de la lecture des données.
};

/**
* @brief Classe permettant de charger un fichier PNG en tant que texture utilisable avec OpenGL.
*
* Une fois générée par l'un des constructeurs, cette texture peut être liée au contexte OpenGL.
* La première fois, la méthode PNGToGLLoader::addTextureToGL doit être appelée.
* Par la suite, pour la lier à nouveau sans recréer d'identifiant de texture, la méthode PNGToGLLoader::bindTextureToGL doit être appelée.
*
* Lors de la construction de cet objet, par l'un ou l'autre de ses constructeurs, le nom de la texture est 0.
* C'est après appel à la méthode PNGToGLLoader::addTextureToGL que la texture se voit attribuer un nom (nombre entier).
*/
class PNGToGLLoader
{
public:
    /*
     * Constructeurs et destructeur
     * ----------------------------
     */
    /**
    * @brief Constructeur PNGToGLLoader 1.
    * @param fileName Le nom du fichier PNG à charger.
    * @throw ConstructorException si une erreur de fichier survient.
    *
    * Charge un fichier texture à partir d'un fichier PNG.
    */
    explicit PNGToGLLoader(const string& fileName);
    /**
    * @brief Constructeur PNGToGLLoader 2.
    * @param r_data Une référence vers l'objet PNGDataBuffer.
    * @throw ConstructorException si une erreur de lecture survient (PNGDataBuffer dont le test de validité échoue après lecture).
    *
    * Charge un fichier texture à partir d'un objet PNGDataBuffer.
    *
    * Les données du PNGDataBuffer ne sont pas modifiées, mais la référence n'est pas constante car le test de validité peut changer de valeur.
    */
    explicit PNGToGLLoader(PNGDataBuffer& r_data);
    /**
    * @brief Destructeur PNGToGLLoader.
    *
    * Détruit le tableau de texels.
    */
    ~PNGToGLLoader();
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Rend cette texture disponible sous OpenGL.
    *
    * Après appel à cette méthode, la texture est liée au contexte OpenGL.
    * Un nom OpenGL (entier) lui est attribué et le champ PNGToGLLoader::m_name est mis à jour.
    * Pour la lier à nouveau par la suite, la méthode PNGToGLLoader::bindTextureToGL devra être appelée.
    */
    void addTextureToGL();
    /**
    * @brief Lie cette texture au contexte OpenGL.
    * @throw PfException si le nom de cette texture est 0 (si elle n'a pas été initialisée).
    *
    * @warning
    * Cette méthode ne doit pas être appelée avant que cette texture ait été initialisée grâce à la méthode PNGToGLLoader::addTextureToGL.
    *
    * @deprecated
    * En pratique, une fois le PNGToGLLoader ayant créé une image, cette image est liée au contexte OpenGL au moyen de la méthode PNGToGLLoader::addTextureToGL
    * puis le nom de texture PNGToGLLoader::m_name généré est stocké dans une liste qui servira par la suite à lier l'image à nouveau, sans avoir à conserver ce PNGToGLLoader.
    * Ainsi, plutôt utiliser la fonction <em>bindTexture</em> (fichier "glfunc.h").
    */
    void bindTextureToGL();
    /**
    * @brief Indique si cette texture a déjà été ajoutée au contexte OpenGL.
    * @return <code>true</code> si le nom de cette texture est différent de 0.
    *
    * @deprecated
    * N'a jamais vraiment servi. L'accesseur PNGToGLLoader::getName étant plutôt utilisé.
    */
    bool isAddedToGL();
    /*
    * Accesseurs
    * ----------
    */
    int getName() const {return m_name;} //!< Accesseur.

private:
    /**
    * @brief La principale fonction de cette classe, à savoir la lecture du fichier PNG.
    * @param r_data Les données à charger.
    * @param p_file Le fichier PNG à charger.
    * @throw ArgumentException si aucun argument valide n'est fourni.
    * @throw PfException si une exception survient lors de la lecture des données.
    *
    * Afin de gagner de la place, une seule fonction est utilisée pour les deux modes de chargement (structure PNGDataBuffer et fichier).
    * Pour utiliser un fichier PNG, un PNGDataBuffer standard devra être passé en premier paramètre, et sera omis.
    */
    void loadPNG(PNGDataBuffer& r_data, FILE* p_file = 0);

    GLuint m_name; //!< Le nom de la texture.
    GLuint m_width; //!< La largeur du fichier PNG.
    GLuint m_height; //!< La hauteur du fichier PNG.
    GLuint m_internalFormat; //!< Le format "interne" du fichier PNG (on s'en fout de ce que c'est, ça marche, c'est bon).
    GLenum m_format; //!< Le format du fichier PNG.
    GLubyte* m_texels_t; //!< Le tableau de texels.
};

#endif // PNGTOGLLOADER_H_INCLUDED


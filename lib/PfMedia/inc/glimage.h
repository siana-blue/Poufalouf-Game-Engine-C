/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe GLImage.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef GLIMAGE_H_INCLUDED
#define GLIMAGE_H_INCLUDED

#include "media_gen.h"

#include <vector>
#include <GL/gl.h>
#include "geometry.h"

/**
* @brief Classe regroupant les données nécessaires à l'affichage d'une image OpenGL.
*
* Les données stockées dans GLImage::m_data_t sont transmises à la fonction <em>glInterleavedArrays</em> selon la structure
* GL_T2F_C3F_V3F ou GL_C3F_V3F si GLImage::m_textureIndex est nul.
*
* Si l'image construite a un nombre de points nul, alors m_data_t est égal à 0.
*
* Le booléen GLImage::m_valid indique si l'image construite possède des données lisibles.
*
* @warning
* Pas plus de MAX_VERTICES_PER_POLYGON points par image pour s'assurer de la compatibilité avec d'autres fonctions, notamment <em>drawGL</em>
* du fichier "glfunc.h".
*/
class GLImage
{
public:
    /*
     * Constructeurs et destructeur
     * ----------------------------
     */
     /**
     * Constructeur GLImage par défaut.
     *
     * Construit une image non texturée, sans point, non valide.
     */
     GLImage();
     /**
     * @brief Constructeur GLImage par copie.
     * @param glImage L'image à copier.
     *
     * Le tableau de données est copié en profondeur.
     *
     * @warning
     * Une erreur de segmentation est possible si le tableau de données de l'image copiée n'est pas cohérent, à savoir que sa taille n'est pas égale
     * au nombre de points multiplié par 8 ou 6 selon que l'image est texturée ou non.
     */
     GLImage(const GLImage& glImage);
    /**
    * @brief Constructeur GLImage 1.
    * @param data_t_c Le tableau de données, selon la structure GL_C3F_V3F ou GL_T2F_C3F_V3F.
    * @param verticesCount Le nombre de points.
    * @param textureIndex L'indice de texture, 0 pour une image non texturée.
    * @param mode Le mode de rendu par OpenGL.
    * @param centerX L'abscisse du centre de cette image.
    * @param centerY L'ordonnée du centre de cette image.
    * @throw ConstructorException si <em>verticesCount</em> est supérieur à MAX_VERTICES_PER_POLYGON.
    *
    * @warning
    * Erreur de segmentation si <em>data_t_c</em> possède moins d'éléments que 6 ou 8 fois <em>verticesCount</em>
    * (6 pour une image non texturée, 8 pour une image texturée).
    *
    * @remarks
    * Ce constructeur n'est pas vraiment utile, d'autant plus qu'il faut préciser le centre soi-même.
    */
    GLImage(const GLfloat* data_t_c, int verticesCount, unsigned int textureIndex = 0, GLenum mode = GL_TRIANGLE_FAN, float centerX = 0.0, float centerY = 0.0);
    /**
    * @brief Constructeur GLImage 2a.
    * @param polygon Le polygone de l'image à créer.
    * @param color La couleur de cette image.
    * @param mode Le mode de rendu par OpenGL.
    * @param coordRelativeToBorder <code>true</code> si les coordonnées de cette image sont relatives aux bordures de la vue (SYSTEM_BORDER_WIDTH, fichier "media_gen.h").
    * @param stat <code>true</code> pour une image ne dépendant pas de la caméra.
    * @throw ConstructorException si une erreur survient lors de la lecture des points des polygones.
    *
    * Crée une image non texturée.
    *
    * Si le polygone passé en paramètre n'a pas de point, l'image ne sera pas valide.
    *
    * @warning
    * De la mémoire est allouée lors de la création de la GLImage.
    * Le destructeur se charge de ces données.
    *
    * Le centre de cette image est calculé comme le centre de sa "bounding box".
    *
    * Si <em>coordRelativeToBorder</em> est faux, alors l'image sera également statique.
    */
    GLImage(const PfPolygon& polygon, const PfColor& color = PfColor::WHITE, GLenum mode = GL_TRIANGLE_FAN, bool coordRelativeToBorder = true, bool stat = false);
    /**
    * @brief Constructeur GLImage 2b.
    * @param polygon Le polygone de l'image à créer.
    * @param textureIndex L'indice de texture, 0 pour une image non texturée.
    * @param coordPolygon Le polygone du fragment de texture.
    * @param color La couleur de cette image.
    * @param coordRelativeToBorder <code>true</code> si les coordonnées de cette image sont relatives aux bordures de la vue (SYSTEM_BORDER_WIDTH, fichier "media_gen.h").
    * @param stat <code>true</code> pour une image ne dépendant pas de la caméra.
    * @throw ConstructorException si une erreur survient lors de la lecture des points des polygones.
    *
    * Crée une image texturée.
    * Si l'indice de texture fourni est 0, alors une image non texturée est crée.
    *
    * Le mode de rendu OpenGL choisi est GL_TRIANGLE_FAN.
    *
    * Si le polygone passé en paramètre n'a pas de point, l'image ne sera pas valide.
    *
    * @warning
    * De la mémoire est allouée lors de la création de la GLImage.
    * Le destructeur se charge de ces données.
    *
    * Le centre de cette image est calculé comme le centre de sa "bounding box".
    *
    * Si <em>coordRelativeToBorder</em> est faux, alors l'image sera également statique.
    */
    GLImage(const PfPolygon& polygon, unsigned int textureIndex, const PfPolygon& coordPolygon, const PfColor& color = PfColor::WHITE, bool coordRelativeToBorder = true, bool stat = false);
    /**
    * @brief Destructeur GLImage.
    *
    * Détruit le tableau de données.
    */
    ~GLImage();
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Indique si cette image est texturée.
    * @return <code>true</code> si GLImage::m_textureIndex n'est pas 0.
    */
    bool isTextured() const;
    /**
    * @brief Retoure une liste de points correspondant au polygone de cette image.
    * @return La liste de points.
    */
    vector<PfPoint> points() const;
    /*
    * Accesseurs
    * ----------
    */
    GLenum getMode() const {return m_mode;} //!< Accesseur.
    int getVerticesCount() const {return m_verticesCount;} //!< Accesseur.
    unsigned int getTextureIndex() const {return m_textureIndex;} //!< Accesseur.
    const GLfloat* getData() const {return m_data_t;} //!< Accesseur.
    bool isValid() const {return m_valid;} //!< Accesseur.
    bool isStatic() const {return m_static;} //!< Accesseur.
    double getAngle() const {return m_angle;} //!< Accesseur.
    void setAngle(double angle) {m_angle = angle;} //!< Accesseur.
    float getCenterX() const {return m_centerX;} //!< Accesseur.
    void setCenterX(float x) {m_centerX = x;} //!< Accesseur.
    float getCenterY() const {return m_centerY;} //!< Accesseur.
    void setCenterY(float y) {m_centerY = y;} //!< Accesseur.
    /*
    * Opérateurs
    * ----------
    */
    /**
    * @brief Opérateur d'affectation.
    * @param glImage L'image à affecter à celle-ci.
    *
    * Le tableau de données actuel est détruit puis copié en profondeur d'après le paramètre.
    *
    * @warning
    * Une erreur de segmentation est possible si le tableau de données de l'image copiée n'est pas cohérent, à savoir que sa taille n'est pas égale
    * au nombre de points multiplié par 8 ou 6 selon que l'image est texturée ou non.
    */
    GLImage& operator=(const GLImage& glImage);

private:
    GLenum m_mode; //!< Le mode d'affichage de l'image avec OpenGL (GL_TRIANGLES, GL_LINE_LOOP etc...).
    int m_verticesCount; //!< Le nombre de points de cette image.
    unsigned int m_textureIndex; //!< L'indice de texture, 0 pour une image non texturée.
    GLfloat* m_data_t; //!< Les données de l'image.
    bool m_valid; //!< Indique si cette image contient des données valides.
    bool m_static; //!< Indique si cette image est statique (non affectée par les mouvements de caméra).
    double m_angle; //!< L'angle de rotation de cette image.
    float m_centerX; //!< L'abscisse du centre de cette image.
    float m_centerY; //!< L'ordonnée du centre de cette image.
};

#endif // GLIMAGE_H_INCLUDED


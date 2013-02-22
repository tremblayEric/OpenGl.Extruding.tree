/**
 * Copyright 2012 Eric Tremblay TIandSE@gmail.com.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/************************************
* INF5071-20                        *
* INFOGRAPHIE                       *
* TP2 :: Extrusion :: Arbre         *
* HIVER 2012                        *
* 12 septembre                      *
*                                   *
* Cree un arbre par extrusion       *
*(un saule pleureur, plus il est    *
* grand et plus ses branche se      *
* dirige vers le sol)               *
*                                   *  
* Auteur       : Eric Tremblay      *   
*************************************/

/****************************
 * commande de compilation: *
 *************************************************************************************
 * g++ -Wall -o tree_extrusion tree_extrusion.cpp -L/usr/X11R6/lib -lX11 -lGL -lm -lGLU -lglut  *
 *************************************************************************************/

#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <ostream>
#include <math.h>
#include <GL/freeglut.h>
#include <GL/glext.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <time.h>

using namespace std;

#define hauteur 64
#define largeur 32

enum DIMENSION_ARBRE{ GRINGALET, MASSIF };
enum SAISON{AUTOMNE, HIVER, ETE,PRINTEMPS};

/**
 * Une image ( une texture )
 */
struct StructureImage {

  GLubyte image[hauteur][largeur][4];//definition de tableau hXl * 4
};

/**
* un point dans un univer a 3 dimenssion,
* coordonnees x,y,z
*/

struct point3f{

	float x;
	float y;
	float z;

};


/**************
 * GLOBAL     *
 **************/

 GLsizei nombreDeTextures;
 static GLuint tableauDeTexNom[100];
 vector<string> vecteurNomTexture;//vecteur contenant les nom de texture en entre
 vector< StructureImage *>  vecteurDimage;//vecteur contenant des pointeurs vers les image

/**
 * Parametre du logiciel modifiant l'apparence de l'arbre
 */
unsigned int HAUTEUR_DE_ARBRE = 150;
DIMENSION_ARBRE CORPULENCE_ARBRE = MASSIF;// tour de taille du tronc, le tour de taille des branche sera ajuste en consequence
unsigned int QUANTITE_DE_BRANCHE = rand()%75;// entre 1 et 75, ce chiffre peut etre celui que l'on veut, mais la taille de l'arbre doit etre considere lors du choix du nombre de branche voulu, ( un arbre d'un pied avec 1 millions de branche ressemblera a  une tache )
SAISON LA_SAISON = ETE;//la tecture du sol et la couleur ainsi que le tronc de l'arbre change selon la saison
//automne : beaucoup de couleur, hiver : de la neige et pas de feuillage( branche brune ) printemps : sol boeux et quelques bourgeao et tronc givre ( il n'y a pas de feuile mais la texture contient des pixel de couleurs) ETE : pelouse, tronc brun et feuillage vert.

/************************************************
 * FIN DE LA DECLARATION DES VARIABLES GLOBALES *
 ************************************************/

/*****************************
 * Declaration des fonctions *
 *****************************/
    int rand_a_b( int a, int b );
    point3f creer_point( float x, float y, float z );
    vector<point3f> creer_cadre_branche( );
    vector<point3f> creer_cadre_tronc( DIMENSION_ARBRE la_corpulence, float y );
    vector<point3f> deplacer_cadre( vector<point3f> le_tronc, float dx, float dy, float dz );
    void reduire_diametre_tronc( vector<point3f> & le_tronc, float ratio );
    void tronc( unsigned int  hauteur_arbre, DIMENSION_ARBRE corpulence );
    void chargerTexture( );
    void initLights( );
    void init( );
    vector<point3f> extrusion( vector<point3f> le_cadre );
    vector<point3f> extrusion_branche( vector<point3f> la_branche );
    void sol( );
    void placer_branche( int hauteur_branche, int longueur_branche, int angle_rotation_sur_y, DIMENSION_ARBRE corpulence_tronc );
    void dessiner_branche( int hauteur_branche, int longueur_branche,  DIMENSION_ARBRE corpulence_tronc );
    void display( );
    void reshape( int w, int h );
    void charger_vecteur_global_texture( SAISON la_saison );

/******************************************
 * Fin de la declaration des fonctions    *
 ******************************************/

/**
* Retourne un entier aleatoire comprit entre a et b
* @Param: a : entier : borne minimale de l'Ã¬ntervale
* @Param: b : entier : borne maximale de l'intervale
* @retour   : un entier compris entre a et b
*/
int rand_a_b(int a, int b){
    return rand()%(b-a) +a;
}


/**
* Creer un point dans un univer a 3 dimensions
* @Param  : x : entier : coordonnee selon l'axe des X
* @Param  : y : entier : coordonnee selon l'axe des Y
* @Param  : z : entier : coordonnee selon l'axe des z
* @retour : un_point   : une structure Point3f : le point correspondant aux trois coordonnees entrees en parametre.
*/
point3f creer_point( float x, float y, float z )
{

	point3f un_point;

	un_point.x = x;
	un_point.y = y;
	un_point.z = z;

	return un_point;
}


/**
* Cree le cadre qui servira a fabriquer la branche avec le procede d'extrusion
* @retour : un vecteur de points correppondant a la forme de la branche ( un octogone ).
*/
vector<point3f> creer_cadre_branche( )
{
	vector<point3f> vecteur_point_octogne;

	vecteur_point_octogne.clear();

	point3f point_A = creer_point(0.5, 2, 0.5);
	vecteur_point_octogne.push_back(point_A);

	point3f point_B = creer_point(0,1.5,1);
	vecteur_point_octogne.push_back(point_B);

	point3f point_C = creer_point(0,0.5,1);
	vecteur_point_octogne.push_back(point_C);

	point3f point_D = creer_point(0,0,0.5);
	vecteur_point_octogne.push_back(point_D);

	point3f point_E = creer_point(0,0,-0.5);
	vecteur_point_octogne.push_back(point_E);

	point3f point_F = creer_point(0,0.5,-1);
	vecteur_point_octogne.push_back(point_F);

	point3f point_G = creer_point(0,1.5,-1);
	vecteur_point_octogne.push_back(point_G);

	point3f point_H = creer_point(0,2,-0.5);
	vecteur_point_octogne.push_back(point_H);


	return vecteur_point_octogne;
}


/**
* Cree un cadre qui servira a fabrique le tronc avec le procede d'extrusion
* Param  : la_corpulence : type enumere DIMENSSION_ARBRE :   l'arbre est MASSIF ou GRINGALET
* Param  : y : flottant : hauteur sur l'axe des y ( sert a deplaacer le cadre lors de l'extrusion.
* retour : vecteur de point3f : contenant les points du cadre;
*/
vector<point3f> creer_cadre_tronc( DIMENSION_ARBRE la_corpulence, float y )
{

	glEnable( GL_TEXTURE_2D );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glBindTexture( GL_TEXTURE_2D, nombreDeTextures   );

	vector<point3f> vecteur_point_octogne;
	vecteur_point_octogne.clear();

	//serviront pour les x et les z
	int x_a, x_b, x_e, x_f, z_c, z_d, z_g, z_h, x_c, x_d, x_g, x_h, z_a, z_b, z_e, z_f;



	if(la_corpulence == MASSIF)
	{
		x_a = x_b = z_g = z_h = 10;// le les valeurs x des point ab sont les memes que celles des z des points gh,  idem pour les x des points ef et les z des points cd ( mais en nagatif donc * -1 plus bas )
		x_d = x_g = z_b = z_e  = -5;//patern semblable a la variable precedente.

		x_e = x_f = z_c = z_d = -10;
		z_a = z_f = x_h = x_c = 5;
	}
	else
	{
		//idem , mais avec des valeurs plus petites,
		x_a = x_b = z_g = z_h = 5;
		x_d = x_g = z_b = z_e  = -2;

		x_e = x_f = z_c = z_d = -2;
		z_a = z_f = x_h = x_c = 5;
	}


		//definition des point de la forme a extruder pour faire le tronc
	    //et insertion de ceux-ci dans le vecteur de point de la forme.
			point3f point_A = creer_point(x_a,y,z_a);
			vecteur_point_octogne.push_back(point_A);

			point3f point_B = creer_point(x_b,y,z_b);
			vecteur_point_octogne.push_back(point_B);

			point3f point_C = creer_point(x_c,y,z_c);
			vecteur_point_octogne.push_back(point_C);

			point3f point_D = creer_point(x_d,y,z_d);
			vecteur_point_octogne.push_back(point_D);

			point3f point_E = creer_point(x_e,y,z_e);
			vecteur_point_octogne.push_back(point_E);

			point3f point_F = creer_point(x_f,y,z_f);
			vecteur_point_octogne.push_back(point_F);

			point3f point_G = creer_point(x_g,y,z_g);
			vecteur_point_octogne.push_back(point_G);

			point3f point_H = creer_point(x_h,y,z_h);
			vecteur_point_octogne.push_back(point_H);


	return vecteur_point_octogne;
}


/**
 * Deplacement des point du tronc en x,y et z
 * @Param  : x : entier : deplacement en x
 * @Param  : y : entier : deplacement en y
 * @Param  : z : entier : deplacement en z
 * @retour : vecteur de point3f : coordonnees du cadre a sont nouvel en=mplacement.
 */
vector<point3f> deplacer_cadre(vector<point3f> le_tronc, float dx, float dy, float dz)
{


	for(unsigned int i = 0; i < le_tronc.size(); ++i)
	{
		le_tronc[i].x += dx;
		le_tronc[i].y += dy;
		le_tronc[i].z += dz;
	}

	return le_tronc;
}


/**
* Reduis la taille du cadre selon le ratio fournit en parametre.
* @Param : le_tronc : vecteur de point3f : la cadre a redimenssioner
* @Param : ratio    : float : ratio du changement de taille.
*/
void reduire_diametre_tronc(vector<point3f> & le_tronc, float ratio)
{

	for( unsigned int i = 0; i < le_tronc.size(); ++i)
	{
		le_tronc[i].x *= ratio;
		le_tronc[i].y *= ratio;
		le_tronc[i].z *= ratio;
	}

}


/**
* Cree et dessine le tronc a partir d'un cadre de tronc de hauteur quelconque et d'une certaine corpulence.
* @Param : hauteur_arbre : entier non signe : la hauteur, non-negative, de l'arbre.
* @Param : corplence     : DIMENSION_ARBRE  : le tour de taille de l'arbre
* @Procedure() : appel   :  procedure extrusion( le_tronc );
*/
void tronc( unsigned int  hauteur_arbre, DIMENSION_ARBRE corpulence )
{
	unsigned int i = 0;

	vector<point3f> le_tronc = creer_cadre_tronc(corpulence,0);//creer la base du tronc

	for( i = 0; i < hauteur_arbre; ++i)
	{
	  le_tronc = extrusion( le_tronc );
	}

}


/**
* Procedure qui va charger chaque textures a partir du (ou des) nom(s)
* contenu(s) dans le vecteurTexture chaque texture sera charger dans une
* StructureImage dont l'adresse sera conserve dans le vecteurDimage.
* Erreur: pas de traitement d'erreur, il revient a l'utilisateur de
* fournir des variable globale correct.
*/
void chargerTexture( )
{

for(  int k = 0; k < nombreDeTextures; ++k){

 StructureImage * lImage;
 StructureImage uneImage;
 lImage = &uneImage;
 vecteurDimage.push_back( lImage );

if( k < nombreDeTextures )
{

  ifstream fichier( vecteurNomTexture[k].c_str() );
  string s;
  int p;
  fichier >> s >> p >> p >> p;//entrer des parametre

  int i, j;

  for( i = hauteur - 1; i >= 0; --i ) {
    for( j = 0; j < largeur; ++j ) {
      int c;
      fichier >> c;
      uneImage.image[i][j][0] = (GLubyte) c;
      fichier >> c;
      uneImage.image[i][j][1] = (GLubyte) c;
      fichier >> c;
      uneImage.image[i][j][2] = (GLubyte) c;
      uneImage.image[i][j][3] = (GLubyte) 255;
    }
  }

  }

  glBindTexture  ( GL_TEXTURE_2D, tableauDeTexNom[k] );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexImage2D   ( GL_TEXTURE_2D, 0, GL_RGBA, largeur,hauteur, 0, GL_RGBA, GL_UNSIGNED_BYTE,vecteurDimage[k] );

}
}


/**
* Initialisation de l'eclairage
*/
void initLights( ) {

  GLfloat ambient[] = {0.4, 0.2, 0.1, 1.0};
  GLfloat position[] = {5.0, -6.0, 2.5, 1.0};
  GLfloat mat_diffuse[] = {0.6, 0.6, 0.6, 1.0};
  GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_shininess[] = {50.0};
  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );
  glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );
  glLightfv( GL_LIGHT0, GL_POSITION, position );
  glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse );
  glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
  glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess );

}


/**
 * Quelques Initialisations : entre autre : la couleur de fond, de la profondeur, du shading, des lumiere ( appel d'un fonction ),
 * des textures leur chargements ( via une fonction ).
 */
void init( ) {

  glClearColor( 0.0, 0.6, 1,1 );
  glClearDepth( 1.0 );
  glEnable( GL_DEPTH_TEST );
  glShadeModel( GL_FLAT );
  initLights();
  glEnable(GL_COLOR_MATERIAL);
  glGenTextures( nombreDeTextures , tableauDeTexNom );
  chargerTexture( );
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

}


/**
* Recoit en entree un cadre contenant un liste de point3f correcpondants au sont contour ( un cadre de tronc ou de branche )
* ce cadre sera deplacer et une forme sera cree avec l'extrusion
* @param : le_cadre : vecteur de point3f : cadre servant a creer la forme par extrusion
* @retour: copie_cadre : le cadre a sa nouvelle position
*/
vector<point3f> extrusion( vector<point3f> le_cadre ){

	unsigned int i = 0;

  vector<point3f> copie_cadre = deplacer_cadre(le_cadre, 0, 1, 0);// on creer une copie de la base situe a y + 1

  glEnable( GL_TEXTURE_2D );
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  glBindTexture( GL_TEXTURE_2D,3);

  glBegin( GL_TRIANGLES );

  for( i = 0; i < le_cadre.size() ; ++i)
  {

	if( i < le_cadre.size() - 1)
	{

		 glTexCoord2f(1,1); glVertex3f(  le_cadre[i].x        , le_cadre[i].y,  le_cadre[i].z );
		 glTexCoord2f(0,1); glVertex3f(  le_cadre[i + 1].x    , le_cadre[i + 1].y,  le_cadre[i + 1].z );
		 glTexCoord2f(0,0); glVertex3f( copie_cadre[i].x      , copie_cadre[i].y, copie_cadre[i].z );

		 glTexCoord2f(1,1); glVertex3f(  copie_cadre[i].x     , copie_cadre[i].y,  copie_cadre[i].z  );
		 glTexCoord2f(0,1); glVertex3f(  copie_cadre[i + 1].x , copie_cadre[i + 1].y,  copie_cadre[i + 1].z );
		 glTexCoord2f(0,0); glVertex3f( le_cadre[i + 1].x     , le_cadre[i + 1].y, le_cadre[i + 1].z  );

	}
	else
	{

		glTexCoord2f(1,1); glVertex3f( le_cadre[i].x    , le_cadre[i].y, le_cadre[i].z );
		glTexCoord2f(0,1); glVertex3f( le_cadre[0].x    , le_cadre[0].y, le_cadre[0].z );
		glTexCoord2f(0,0); glVertex3f( copie_cadre[i].x , copie_cadre[i].y, copie_cadre[i].z );

		glTexCoord2f(1,1); glVertex3f( copie_cadre[i].x , copie_cadre[i].y, copie_cadre[i].z );
		glTexCoord2f(0,1); glVertex3f( copie_cadre[0].x , copie_cadre[0].y, copie_cadre[0].z );
		glTexCoord2f(0,0); glVertex3f( le_cadre[0].x    , le_cadre[0].y, le_cadre[0].z );

	}

  }

  glEnd();
  glFlush();

  return copie_cadre;

}


/**
 * Dessine une branche ar extrusion a partir de donnees aleatoires ( pour creer des braches differantes d'une fois a l'autre )
 * @param     : vecteur de point3f : le cadre de la branche
 * @retour    : copie_branche : le cadre de branche a sont nouvel emplacement
 * @procedure : appel : deplacer_cadre(la_branche, x, y, z)
 */
vector<point3f> extrusion_branche( vector<point3f> la_branche ){

  glEnable( GL_TEXTURE_2D );
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  glBindTexture( GL_TEXTURE_2D, 2   );

	unsigned int i = 0;

	int x,y,z = 0;
	int variante;

  // 7 variante de deplacement  varie l'apparence de la branche
  // 001,010,100,110,011,101,111
  variante = rand()%8;

  switch(variante)
  {

  case 1: x = 0;
          y = 0;
          z = 1;
          break;
  case 2: x = 0;
          y = 1;
          z = 0;
          break;
  case 3: x = 1;
	      y = 0;
          z = 0;
          break;
  case 4: x = 1;
          y = 1;
          z = 0;
          break;
  case 5: x = 0;
          y = 1;
          z = 1;
          break;
  case 6: x = 1;
          y = 0;
          z = 1;
          break;
  default:x = 1;
          y = 1;
          z = 1;
          break;

  }

  vector<point3f> copie_branche = deplacer_cadre(la_branche, x, y, z);// on creer une copie de la base situe a y + 1

  glBegin( GL_TRIANGLES );

  for( i = 0; i < la_branche.size() ; ++i)
  {

	if( i < la_branche.size() - 1)
	{


	  glTexCoord2f(1,1); glVertex3f(  la_branche[i].x ,   la_branche[i].y,  la_branche[i].z );
	  glTexCoord2f(0,1); glVertex3f(  la_branche[i + 1].x,   la_branche[i + 1].y,  la_branche[i + 1].z );
	  glTexCoord2f(0,0); glVertex3f( copie_branche[i].x,  copie_branche[i].y, copie_branche[i].z );

	  glTexCoord2f(1,1); glVertex3f(  copie_branche[i].x , copie_branche[i].y, copie_branche[i].z  );
	  glTexCoord2f(0,1); glVertex3f(  copie_branche[i + 1].x , copie_branche[i + 1].y, copie_branche[i + 1].z );
	  glTexCoord2f(0,0); glVertex3f( la_branche[i + 1].x , la_branche[i + 1].y, la_branche[i + 1].z  );

	}
	else
	{
	  glTexCoord2f(1,1); glVertex3f( la_branche[i].x , la_branche[i].y, la_branche[i].z );
	  glTexCoord2f(0,1); glVertex3f( la_branche[0].x , la_branche[0].y, la_branche[0].z );
	  glTexCoord2f(0,0); glVertex3f( copie_branche[i].x , copie_branche[i].y, copie_branche[i].z );

	  glTexCoord2f(1,1); glVertex3f( copie_branche[i].x , copie_branche[i].y, copie_branche[i].z );
	  glTexCoord2f(0,1); glVertex3f( copie_branche[0].x , copie_branche[0].y, copie_branche[0].z );
	  glTexCoord2f(0,0); glVertex3f( la_branche[0].x , la_branche[0].y, la_branche[0].z );

	}

  }

  glEnd();
  glFlush();

  return copie_branche;

}


/**
* Procedure qui cree le sol
*/
void sol(){

    glEnable( GL_TEXTURE_2D );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glBindTexture( GL_TEXTURE_2D, 1);

          glBegin( GL_QUADS );

          //Quadrant 1

          glTexCoord2f(1,1); glVertex3f(    0,0, 0 );
          glTexCoord2f(1,0); glVertex3f(  1000,0, 0 );
          glTexCoord2f(0,0); glVertex3f(  1000,0,-500 );
          glTexCoord2f(0,1); glVertex3f(     0,0,-500 );


          //Quadrant 2
          glTexCoord2f(1,1); glVertex3f( -1000,0, 0 );
          glTexCoord2f(1,0); glVertex3f(  0,0, 0 );
          glTexCoord2f(0,0); glVertex3f(  0,0,-500 );
          glTexCoord2f(0,1); glVertex3f( -1000,0,-500 );
          //Quadrant 3

          glTexCoord2f(1,1); glVertex3f( -1000,0, 500 );
          glTexCoord2f(1,0); glVertex3f(     0,0, 500 );
          glTexCoord2f(0,0); glVertex3f(     0,0, 0.0 );
          glTexCoord2f(0,1); glVertex3f( -1000,0, 0.0 );
          //Quadrant 4
          glTexCoord2f(1,1); glVertex3f( 0,0,0 );
          glTexCoord2f(1,0); glVertex3f( 0,0, 500 );
          glTexCoord2f(0,0); glVertex3f(  1000,0, 500 );
          glTexCoord2f(0,1); glVertex3f(  1000,0,0 );

    glEnd();
    glFlush();



}


/**
 * Procedure qui va placer la branche dans l'arbre
 * @param     : hauteur_branche  : entier : la hauteur de la branche dans l'arbre
 * @param     : longueur_branche : entier : la longueur de la branche
 * @param     : angle_rotation   : entier : angle de rotation de la branche
 * @param     : corpulence_tronc : DIMENSSION_ARBRE : tour de taille de la branche
 * @procedure : appel dessiner_branche(hauteur_branche,longueur_branche, corpulence_tronc);
 */
void placer_branche(int hauteur_branche, int longueur_branche, int angle_rotation_sur_y, DIMENSION_ARBRE corpulence_tronc)
{
	 glPushMatrix();
	 glRotatef(angle_rotation_sur_y,0,1,0);
	 dessiner_branche(hauteur_branche,longueur_branche, corpulence_tronc);
	 glPopMatrix();
}


/**
 * Dessine la branche
 * @param     : hauteur_branche    : entier : la hauteur de la branche
 * @param     : longueur_branche   : entier : la hauteur de la branche
 * @param     : corpulence_tronc   : DIMENSSION_ARBRE : le tour de taille du tronc est necessaire afin de determiner les proportions de la branche
 * @procedure : appel creer_cadre_branche( ), extrusion_branche(  branche )
 */
void dessiner_branche(int hauteur_branche, int longueur_branche,  DIMENSION_ARBRE corpulence_branche )
{

	vector<point3f> branche = creer_cadre_branche( );
	float x = 0;
	float y = 0;
	float z = 0;
	int axe_rotation = rand()%4;

	switch (axe_rotation)
	{
	case 1 : x = 1;
	         break;
	case 2:  y =1;
	         break;
	default: z = 1;
	         break;
    }

	  for(int i = 0; i < longueur_branche; ++i)
	  {

		  glPushMatrix();

		  glTranslatef(0,hauteur_branche,0);
		  glRotatef(i ,x,y,z);
		  if( corpulence_branche == MASSIF)
		  {
		  glScalef(2,2,2);
		  }
		  branche =  extrusion_branche(  branche );

		  glPopMatrix();


	  }

}


/**
* Fonction d'affichage.
* Affiche la scene suivante :
* Un arbre avec des branche sur un sol sous un ciel bleu:
* L'apparence de l'arbre change en fonction de sa hauteur, de son tour de taille, du nombre de branche qu'il possede ainsi que de la saison.
* @Procedure : appel : tronc( parametre_hauteur, parametre_corpulence_arbre )
* @Procedure : appel : placer_branche( hauteur_branche,longueur_branche, angle_branche,parametre_corpulence_arbre )
* @procedure : rand_a_b(a,b);
* @procedure : sol(), tronc( parametre_hauteur, parametre_corpulence_arbre ), placer_branche( hauteur_branche,longueur_branche, angle_branche,parametre_corpulence_arbre )
*/
void display( ) {

  unsigned int parametre_hauteur = HAUTEUR_DE_ARBRE;
  DIMENSION_ARBRE parametre_corpulence_arbre = CORPULENCE_ARBRE;
  int nombre_branche = QUANTITE_DE_BRANCHE;

  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  sol();

  int hauteur_branche;
  int longueur_branche;
  int angle_branche;

  tronc( parametre_hauteur, parametre_corpulence_arbre );


  for(int i = i; i < nombre_branche; ++i)
  {

		 hauteur_branche = rand_a_b((int)floor(parametre_hauteur * 0.3),parametre_hauteur);

		 if( hauteur_branche/parametre_hauteur >= 0.3 && hauteur_branche < 0.55 )
		 {
			 longueur_branche = rand_a_b((int)floor(parametre_hauteur * 0.4),(int) floor(parametre_hauteur * 0.65));
		 }
		 else if( hauteur_branche/parametre_hauteur >= 0.45 && hauteur_branche < 0.65 )
		 {
			 longueur_branche = rand_a_b((int)floor(parametre_hauteur * 0.25),(int) floor(parametre_hauteur * 0.35));
		}
		 else if( hauteur_branche/parametre_hauteur >= 0.60 && hauteur_branche < 0.85 )
		 {
			 longueur_branche = rand_a_b((int)floor(parametre_hauteur * 0.25),(int) floor(parametre_hauteur * 0.40) );
		 }
		 else
		 {
			 longueur_branche = rand_a_b((int)floor(parametre_hauteur * 0.20), (int)(parametre_hauteur * 0.30));
		 }

		 angle_branche = rand()%360;
	     placer_branche( hauteur_branche,longueur_branche, angle_branche,parametre_corpulence_arbre );

  }


  glFlush();

}


/**
 * Creation de la scene.
 */
void reshape( int w, int h ) {

  glViewport( 0, 0, ( GLsizei ) w, ( GLsizei ) h );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  double angleOuverture = 150.0;
  double ratioEcran = (GLfloat)w/(GLfloat)h;
  double near = 1.0;
  double far = 5000.0;

  gluPerspective( angleOuverture, ratioEcran, near, far );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  gluLookAt( -50,40,-50, 0, 2, 5, 0, 1, 0 );

}

/**
 * Charge les nom des textures en fonction de la saison
 * @param : la_saison : SAISON : AUTOMNE, HIVER, PRINTEMPS ou ETE
 */
void charger_vecteur_global_texture( SAISON la_saison )
{

	if( la_saison == AUTOMNE)
	{
    	vecteurNomTexture.push_back( "sol_automne.ppm" );
		vecteurNomTexture.push_back( "branche_automne.ppm" );
		vecteurNomTexture.push_back( "tronc.ppm" );
	}
	else if( la_saison ==  HIVER)
	{
		vecteurNomTexture.push_back( "sol_hiver.ppm" );
		vecteurNomTexture.push_back( "branche_hiver.ppm" );
		vecteurNomTexture.push_back( "tronc_glace.ppm" );
	}
	else if( la_saison == PRINTEMPS)
	{
		vecteurNomTexture.push_back( "sol_printemps.ppm" );
		vecteurNomTexture.push_back( "branche_printemps.ppm" );
		vecteurNomTexture.push_back( "tronc_glace.ppm" );
	}
	else//ETE
	{
		vecteurNomTexture.push_back( "sol_ete.ppm" );
		vecteurNomTexture.push_back( "branche_ete.ppm" );
		vecteurNomTexture.push_back( "tronc.ppm" );
	}

    vecteurNomTexture.push_back( "buton.ppm" );
	nombreDeTextures = (GLsizei)vecteurNomTexture.size();

}


int main( int argc, char ** argv ) {

  const char * nom_de_la_fenetre = " UQAM :: Hiver2012 :: INF5071 :: TP2 :: Arbre par extrusion ";
  SAISON la_saison = LA_SAISON;

  charger_vecteur_global_texture( la_saison);

  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB );
  glutInitWindowSize( 10000, 10000);
  glutInitWindowPosition( 100, 150 );
  glutCreateWindow( nom_de_la_fenetre );
  init();
  glutReshapeFunc( reshape );
  glutDisplayFunc( display );
  glutMainLoop();

  return 0;
}


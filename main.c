#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>
#include <GL4D/gl4dp.h>
#include <stdlib.h>

#define MAX_OBJ 24000
#define WIDTH 1280
#define HEIGHT 720
#define STEP 0.015f


static void init( void );
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw( void );
static void quit( void );

static GLuint _wW = 1280, _wH = 720;
static GLuint _cubeId = 0;
static GLuint _quadId = 0;
static GLuint _pId = 0;
static GLuint _vao = 0;
static GLuint _buffer[2] = { 0 };
static GLuint _tex = 0;
/* indices pour réaliser le maillage des géométrie, envoyés dans le
* VBO ELEMENT_ARRAY_BUFFER */
static GLuint idata[MAX_OBJ * 2] = {
    /* le triangle-strip rouge-vert-bleu-jaune */

};
/* données-sommets envoyée dans le VBO ARRAY_BUFFER */
static GLfloat data[MAX_OBJ * 16] = {
    /* 4 coordonnées de sommets en 3D chacune suivie de sa couleur
     * pour un triangle */
     // coord position x y z | couleur rgb      | coord tex

};


int main( int argc, char** argv ) {
    if ( !gl4duwCreateWindow( argc, argv, "Eternal Fractal", GL4DW_POS_CENTERED, GL4DW_POS_CENTERED,
        _wW, _wH, GL4DW_OPENGL | GL4DW_RESIZABLE | GL4DW_SHOWN ) ) {
        fprintf( stderr, "Erreur lors de la création de la fenêtre\n" );
        return 1;
    }
    init();
    atexit( quit );
    gl4duwDisplayFunc( draw );
    gl4duwMainLoop();
    return 0;
}

void initIdata() {
    for ( int i = 0; i < ( MAX_OBJ * 2 ); i++ ) {
        idata[i] = i;
    }

}


void initData() {
    int nb_col = (int)(2/STEP);
    for ( int i = 0; i < ( MAX_OBJ * 16 ); i += 8 ) {
        int x = i/16 % nb_col;
        int y = (i/16)/nb_col;
        data[i] = (-1.0 + (STEP/2)) + STEP * x; // 1 sur 2 
        data[i + 1] = (HEIGHT/(float)WIDTH-(STEP/2)) - STEP * y;
        data[i + 2] = -1.0f;
        data[i + 3] = 0.0f;
        data[i + 4] = 0.0f;
        data[i + 5] = 1.0f;
        data[i + 6] = 0.0f;
        data[i + 7] = 1.0f;
        //printf( "I : %d\n", i );
        //printf( "data[I] : %f\n", data[i] );
        //16 donnnées == 1 seg 
    }
}


void animeData( float angle, float radius ) {
    int nb_cols = 2.0 / STEP;
    int stepInPixels = (STEP /2) * WIDTH;
    //printf("stepInPixels=%d\n", stepInPixels);
    for ( int i = 8; i < ( MAX_OBJ * 16 ); i += 16 ) {
        int x = ((i - 8) / 16) % nb_cols;
        int y = ((i - 8) / 16) / nb_cols;
        //if (i == 8 + 16 * 32) printf("x=%d;y=%d\n", x, y);
        float currentAngle = (cos(x * stepInPixels * 0.005) + sin(y * stepInPixels * 0.005)) * M_PI * angle;

        data[i] =  data[i - 8] + cos( currentAngle ) * radius;
        data[i + 1] = data[i + 1 - 8] + sin( currentAngle ) * radius;
    }
}


void init( void ) {
    float a = 1;


    //SDL_GL_SetSwapInterval( 1 );


    glEnable( GL_DEPTH_TEST );
    glEnable( GL_LINE_SMOOTH );

    _pId = gl4duCreateProgram( "<vs>shaders/hello.vs", "<fs>shaders/hello.fs", NULL );

    glClearColor( 0.9f, 0.9f, 1.0f, 1.0f );

    gl4duGenMatrix( GL_FLOAT, "modelView" );
    gl4duGenMatrix( GL_FLOAT, "proj" );

    gl4duBindMatrix( "proj" );
    gl4duLoadIdentityf();

    // Boîte qui délimite les objets à dessiner
    //   gauche , droite , bas , haut , proche , éloignement
    gl4duFrustumf( -1.0f, 1.0f, ( -1.0f * _wH ) / _wW, ( 1.0f * _wH ) / _wW, 1.0f, 1000.0f );
    glViewport( 0, 0, _wW, _wH );
    glGenVertexArrays( 1, &_vao );
    glBindVertexArray( _vao );

    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );
    glEnableVertexAttribArray( 2 );

    // 2 -> nombre de buffer à instancier
    glGenBuffers( 2, _buffer );

    initIdata();
    initData();

    glBindBuffer( GL_ARRAY_BUFFER, _buffer[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW );

    // channel , vec3 , float, normalize,  step (data) , ptr sur debut (offset)
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof * data, ( const void* ) 0 );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof * data, ( const void* ) ( 3 * sizeof * data ) );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof * data, ( const void* ) ( 6 * sizeof * data ) );

    // selectionne un buffer
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _buffer[1] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof idata, idata, GL_STATIC_DRAW );

    // Nettoyage du plan de travail
    glBindVertexArray( 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    // Texture manuelle
    GLuint tex[] = { 0, -1, -1, ( 0xFF << 24 ) | 0xFF };
    glGenTextures( 1, &_tex );
    glBindTexture( GL_TEXTURE_2D, _tex );

    // Comportement d'une texture lors de son redimensionnement
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    // Mode texture , 0 -> niveau de texture , rgba ordre donnée , largeur, hauteur , defaut, rgba, 0 - 255 ,  id tex
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex );

    glBindTexture( GL_TEXTURE_2D, 0 );
}

static double get_dt( void ) {
    static double t0 = 0.0f;
    double t = gl4dGetElapsedTime(), dt = ( t - t0 ) / 1000.0;
    t0 = t;
    return dt;
    //return 0.0;
}

void draw( void ) {
    static GLfloat angle = 0.0f;
    static float radius = STEP;
    //glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram( _pId );

    gl4duBindMatrix( "modelView" );
    gl4duLoadIdentityf();
    //gl4duTranslatef( sin( a ), 0.0f, -2.5f );
    //gl4duRotatef( 180.0f * a / M_PI, 0.0f, 1.0f, 0.0f );
    gl4duSendMatrices();
    //gl4dgDraw( _cubeId );

    animeData( angle, radius );
    //data[8] = data[0] + cos( angle ) * 0.1f;
    //data[9] = data[1] + sin( angle ) * 0.1f;
    //        /* sommet  1 */ 0.0f, 0.5f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,


    glBindBuffer( GL_ARRAY_BUFFER, _buffer[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );


    // niveau texture
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, _tex );

    glBindVertexArray( _vao );

    glDrawElements( GL_LINES, MAX_OBJ * 2, GL_UNSIGNED_INT, ( const GLvoid* ) 0 );


    glBindVertexArray( 0 );
    glUseProgram( 0 );
    glBindTexture( GL_TEXTURE_2D, 0 );

    //glUniform1i( glGetUniformLocation( _pId, "myTex" ), 0 );
    //GLubyte r = rand() % 256, g = rand() % 256, b = rand() % 256;
    //angle += 0.4f * M_PI * get_dt();
    angle += 0.01f;
}



void quit( void ) {
    /* suppression du VAO _vao en GPU */
    if ( _vao )
        glDeleteVertexArrays( 1, &_vao );
    /* suppression du VBO _buffer en GPU, maintenant il y en a deux */
    if ( _buffer[0] )
        glDeleteBuffers( 2, _buffer );
    /* suppression de la texture en GPU */
    if ( _tex )
        glDeleteTextures( 1, &_tex );
    /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
    gl4duClean( GL4DU_ALL );
}
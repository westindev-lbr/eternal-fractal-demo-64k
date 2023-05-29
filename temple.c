#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dh.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "audioHelper.h"

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init( void );
static void draw( void );
static void quit( void );

/*!\brief opened window width */
/*!\brief opened window height */
extern GLfloat _dim[];
/*!\brief identifiant du GLSL program */
static GLuint _pId = 0;
static GLuint _quad = 0;

// Music 
/*!\brief nombre d'échantillons du signal sonore */
#define ECHANTILLONS 1024
/*!\brief amplitude des échantillons du signal sonore */
static Sint16 _hauteurs[ECHANTILLONS];
static GLint _amplitude_sonore = -1;
static int currentSampleIndex = 0;


/*!\brief initialise les paramètres OpenGL et les données. */
static void init( void ) {
    /* Création du programme shader (voir le dossier shader) */
    _pId = gl4duCreateProgram( "<vs>shaders/basic.vs", "<fs>shaders/temple.fs", NULL );
    /* Set de la couleur (RGBA) d'effacement OpenGL */
    glClearColor( 1.0f, 0.0f, 0.0f, 1.0f );
    _quad = gl4dgGenQuadf();
    /* Création des matrices GL4Dummies, une pour la projection, une pour la modélisation et une pour la vue */
    gl4duGenMatrix( GL_FLOAT, "projectionMatrix" );
    gl4duGenMatrix( GL_FLOAT, "modelMatrix" );
    gl4duGenMatrix( GL_FLOAT, "viewMatrix" );
}

/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw( void ) {
    glClearColor( 1, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /* activation du programme _pId */
    glUseProgram( _pId );
    gl4duBindMatrix( "projectionMatrix" );
    gl4duLoadIdentityf();
    gl4duOrthof( -1, 1, -_dim[1] / _dim[0], _dim[1] / _dim[0], 0, 100 );

    gl4duBindMatrix( "viewMatrix" );
    gl4duLoadIdentityf();
    gl4duLookAtf( 0, 0, 1, 0, 0, 0, 0, 1, 0 );
    gl4duBindMatrix( "modelMatrix" );
    gl4duLoadIdentityf();
    gl4duSendMatrices();


    // Envoi des données d'amplitude au shader
    float currentSample = (GLfloat)_hauteurs[currentSampleIndex];
    _amplitude_sonore = glGetUniformLocation( _pId, "amplitudeSon" );
    glUniform1f(_amplitude_sonore, currentSample);
    currentSampleIndex = (currentSampleIndex + 1) % ECHANTILLONS;

    float et = gl4dGetElapsedTime();
    glUniform1f( glGetUniformLocation(_pId, "u_time"), (et/1000));
    
    gl4dgDraw(_quad);
    glUniform2f( glGetUniformLocation(_pId, "resolution"), _dim[0], _dim[1]);

    /* désactiver le programme shader */
    glUseProgram( 0 );
}

static void quit( void ) {
    if ( _quad )
        gl4dgDelete(_quad);
    gl4duClean( GL4DU_ALL );
}

void temple( int state ) {
    static int h = 1;
    Sint16* audio_samples;
    switch ( state ) {
    case GL4DH_INIT:
        h = _dim[1];
        init();
        return;
    case GL4DH_FREE:
        quit();
        return;
    case GL4DH_UPDATE_WITH_AUDIO:
        audio_samples = ( Sint16* ) ahGetAudioStream(); 
        if ( ahGetAudioStreamLength() >= 2 * ECHANTILLONS ) {
            for ( int i = 0; i < ECHANTILLONS; i++ ){
                _hauteurs[i] = h / 2 + ( h / 2 ) * audio_samples[i] / ( ( 1 << 15 ) - 1.0 );
            }
        }
        return;
    default: /* GL4DH_DRAW */
        draw();
        return;
    }
}
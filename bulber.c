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

// Dimension Window
extern GLfloat _dim[];

/*!\brief identifiant du Vertex Array Object */
static GLuint _vao = 0;
/*!\brief identifiant des Vertex Buffer Objects */
static GLuint _buffer[2] = { 0 };
/*!\brief identifiant du GLSL program */
static GLuint _pId = 0;

// Music 
#define ECHANTILLONS 1024
static Sint16 _hauteurs[ECHANTILLONS];
static GLint _amplitude_sonore = -1;
static int currentSampleIndex = 0;

static void init( void ) {

    GLuint idata[] = { 0, 1, 3, 2 };
    GLfloat data[] = {
        /* sommet  0 */ -1, -1, 0, 1, 0, 0, 0, 0,
        /* sommet  1 */ 1, -1, 0, 0, 1, 0, 1, 0,
        /* sommet  2 */ 1, 1, 0, 0, 0, 1, 1, 1,
        /* sommet  3 */ -1, 1, 0, 1, 0, 0, 0, 1 };

    glGenVertexArrays( 1, &_vao );
    glBindVertexArray( _vao );
    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );
    glEnableVertexAttribArray( 2 );
    glGenBuffers( 2, _buffer );
    glBindBuffer( GL_ARRAY_BUFFER, _buffer[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof * data, ( const void* ) 0 );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof * data, ( const void* ) ( 3 * sizeof * data ) );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof * data, ( const void* ) ( 6 * sizeof * data ) );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _buffer[1] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof idata, idata, GL_STATIC_DRAW );


    // dé-lier le VAO puis les VAO
    glBindVertexArray( 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    // Création du programme shader
    _pId = gl4duCreateProgram( "<vs>shaders/basic.vs", "<fs>shaders/bulber.fs", NULL );

    glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );

    glEnable( GL_DEPTH_TEST );

    gl4duGenMatrix( GL_FLOAT, "projectionMatrix" );
    gl4duGenMatrix( GL_FLOAT, "modelMatrix" );
    gl4duGenMatrix( GL_FLOAT, "viewMatrix" );
    gl4duBindMatrix( "projectionMatrix" );
    gl4duLoadIdentityf();

}

static void draw( void ) {
    glClearColor( 0, 0.1, 0.1, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glUseProgram( _pId );
    gl4duBindMatrix( "viewMatrix" );
    gl4duLoadIdentityf();
    gl4duLookAtf( 0, 0, 1, 0, 0, 0, 0, 1, 0 );
    gl4duBindMatrix( "modelMatrix" );
    gl4duLoadIdentityf();
    gl4duSendMatrices();
    glBindVertexArray( _vao );
    glDrawElements( GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, ( const GLvoid* ) 0 );
    glBindVertexArray( 0 );

    // Envoi des données d'amplitude au shader
    float currentSample = ( GLfloat ) _hauteurs[currentSampleIndex];
    printf( "CURRENTSAMPLE = %f\n", currentSample );
    _amplitude_sonore = glGetUniformLocation( _pId, "amplitudeSon" );
    glUniform1f( _amplitude_sonore, currentSample );
    currentSampleIndex = ( currentSampleIndex + 1 ) % ECHANTILLONS;

    // Envoi l'info du temps au shader
    float et = gl4dGetElapsedTime();
    glUniform1f( glGetUniformLocation( _pId, "u_time" ), ( et / 10000 ) );

    // Envoi l'info des dimensions du viewport
    GLint viewportloc = glGetUniformLocation( _pId, "u_viewport" );
    glUniform2f( viewportloc, _dim[0], _dim[1] );

    // Désactiver le programme shader
    glUseProgram( 0 );
}

static void quit( void ) {
    if ( _vao )
        glDeleteVertexArrays( 1, &_vao );
    if ( _buffer[0] )
        glDeleteBuffers( 2, _buffer );
    gl4duClean( GL4DU_ALL );
}


void bulber( int state ) {
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
            for ( int i = 0; i < ECHANTILLONS; i++ ) {
                _hauteurs[i] = h / 2 + ( h / 2 ) * audio_samples[i] / ( ( 1 << 15 ) - 1.0 );
            }
        }
        return;
    default: /* GL4DH_DRAW */
        draw();
        return;
    }
}
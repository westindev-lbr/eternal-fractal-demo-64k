#include <stdlib.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dh.h>
#include <GL4D/gl4duw_SDL2.h>
#include "animation.h"
#include "audioHelper.h"


/* Prototypes des fonctions statiques contenues dans ce fichier C. */
static void init( void );
static void quit( void );
static void resize( int w, int h );
static void keydown( int keycode );

static GL4DHanime _animations[] = {
    { 4000, mandelbrot, NULL, NULL },
    { 10000,mandelbrot, pulse, transition_fondu },
    { 6000, pulse, NULL, NULL },
    { 3000, pulse, julia, transition_fondu },
    { 6000, julia, NULL, NULL },
    { 8000,julia, rhombic, transition_fondu },
    { 10000, rhombic, NULL, NULL },
    { 2000, rhombic, temple, transition_fondu },
    { 10000, temple, NULL, NULL },
    { 2000, temple, rhombic, transition_fondu },
    { 10000, rhombic, NULL, NULL },
    { 2000, rhombic, temple, transition_fondu },
    { 10000, temple, NULL, NULL },
    { 2000, temple, bulber, transition_fondu },
    { 10000, bulber, NULL, NULL },
    { 2000, bulber, credit, transition_fondu },
    { 15000, credit, NULL, NULL },
    {    0, NULL, NULL, NULL } //Toujours laisser à la fin
};

/*!\brief dimensions initiales de la fenêtre */
extern GLfloat _dim[] = { 1280, 720 };

int main( int argc, char** argv ) {

    if ( !gl4duwCreateWindow( argc, argv, "GL4Dummies Axel Labarre Demo",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        _dim[0], _dim[1],
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN ) )
        return 1;
    init();
    atexit( quit );
    gl4duwResizeFunc( resize );
    gl4duwKeyDownFunc( keydown );
    gl4duwDisplayFunc( gl4dhDraw );

    // Audio
    ahInitAudio( "audio/xion.mid");
    gl4duwMainLoop();
    
    return 0;
}

static void init( void ) {
    glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
    gl4dhInit( _animations, _dim[0], _dim[1], animationsInit );
    resize( _dim[0], _dim[1] );
}

/**
 * * resize(largeur,hauteur)
 * paramétre la vue (viewPort) OpenGL en fonction des dimensions de la fenêtre.
 */
static void resize( int w, int h ) {
    //printf( "resize window.c (%d, %d)\n", w, h );
    _dim[0] = w;
    _dim[1] = h;
    glViewport( 0, 0, _dim[0], _dim[1] );
    transition_fondu(NULL,NULL,0,0,4);
    gl4dhResize(w, h);
}

static void keydown( int keycode ) {
    switch ( keycode ) {
    case SDLK_ESCAPE:
    case 'q':
        exit( 0 );
    default:
        break;
    }
}

static void quit( void ) {
    ahClean();
    gl4duClean( GL4DU_ALL );
}

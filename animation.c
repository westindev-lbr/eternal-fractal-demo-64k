#include <GL4D/gl4dh.h>
#include <assert.h>
#include <stdlib.h>
#include <GL4D/gl4dg.h>
#include <SDL_image.h>
#include "animation.h"

static GLuint _quadId = 0;

void transition_fondu( void ( *a0 )( int ), void ( *a1 )( int ), Uint32 t, Uint32 et, int state ) {
    int vp[4], i;
    GLint tId;
    static GLuint tex[2], pId;

    switch ( state ) {
    case GL4DH_INIT:
        glGetIntegerv( GL_VIEWPORT, vp );
        glGenTextures( 2, tex );
        for ( i = 0; i < 2; i++ ) {
            glBindTexture( GL_TEXTURE_2D, tex[i] );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, vp[2], vp[3], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
        }
        pId = gl4duCreateProgram( "<vs>shaders/mix.vs", "<fs>shaders/mix.fs", NULL );
        return;

    case GL4DH_FREE:
        /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
        if ( tex[0] ) {
            glDeleteTextures( 2, tex );
            tex[0] = tex[1] = 0;
        }
        return;

    case GL4DH_UPDATE_WITH_AUDIO:
        /* METTRE A JOUR LES DEUX ANIMATIONS EN FONCTION DU SON */
        if ( a0 )
            a0( state );
        if ( a1 )
            a1( state );
        return;
    case 4: // RESIZE 
        printf("RESIZE TR FONDU \n");
        glGetIntegerv( GL_VIEWPORT, vp );
        for ( i = 0; i < 2; i++ ) {
            glBindTexture( GL_TEXTURE_2D, tex[i] );
            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, vp[2], vp[3], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
        }
        glBindTexture( GL_TEXTURE_2D, 0 );
        return;
    default: /* GL4DH_DRAW */
        /* RECUPERER L'ID DE LA DERNIERE TEXTURE ATTACHEE AU FRAMEBUFFER */
        glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &tId );

        /* JOUER LES DEUX ANIMATIONS */
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex[0], 0 );
        if ( a0 )
            a0( state );
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex[1], 0 );
        if ( a1 )
            a1( state );

        /* MIXER LES DEUX ANIMATIONS */
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tId, 0 );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_BLEND );
        glUseProgram( pId );
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, tex[0] );
        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, tex[1] );

        if ( et / ( GLfloat ) t > 1 ) {
            fprintf( stderr, "%d-%d -- %f\n", et, t, et / ( GLfloat ) t );
            exit( 0 );
        }

        glUniform1f( glGetUniformLocation( pId, "dt" ), et / ( GLfloat ) t );
        glUniform1i( glGetUniformLocation( pId, "tex0" ), 0 );
        glUniform1i( glGetUniformLocation( pId, "tex1" ), 1 );
        gl4dgDraw( _quadId );
        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, 0 );
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, 0 );

        return;
    }
}

void animation_vide( int state ) {
    switch ( state ) {
    case GL4DH_DRAW:
        glClearColor( 0, 0, 0, 1 );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        return;
    default:
        return;
    }
}

void animation_videR( int state ) {
    switch ( state ) {
    case GL4DH_DRAW:
        glClearColor( 1, 0, 0, 1 );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        return;
    default:
        return;
    }
}

void animation_videG( int state ) {
    switch ( state ) {
    case GL4DH_DRAW:
        glClearColor( 0, 1, 0, 1 );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        return;
    default:
        return;
    }
}

void animationsInit( void ) {
    if ( !_quadId )
        _quadId = gl4dgGenQuadf();
}
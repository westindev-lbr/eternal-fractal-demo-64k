#ifndef _ANIMATION_H

#define _ANIMATION_H

#ifdef __cplusplus
extern "C"
{
#endif

    extern void transition_fondu(void (*a0)(int), void (*a1)(int), Uint32 t, Uint32 et, int state);
    extern void animation_vide(int state);
    extern void animation_videR(int state);
    extern void animation_videG(int state);
    extern void animationsInit(void);

    /* Scenes */
    extern void credit(int state);
    extern void julia(int state);
    extern void mandelbrot( int state );
    extern void rhombic( int state );
    extern void bulber( int state );
    extern void pulse( int state );
    extern void temple( int state );

#ifdef __cplusplus
}
#endif

#endif
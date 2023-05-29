#version 330

uniform float u_time;
uniform vec2 resolution;
uniform float amplitudeSon;

/* sortie du frament shader : une couleur */
out vec4 fragColor;

const float MAX_ITER = 256.0;

float mandelbrot(vec2 uv, float zoom) {
    vec2 c = (uv - 0.5)/ zoom - vec2(0.7, 0.0);
    c = c / pow(u_time * 0.9, 3.5) - vec2(0.65, 0.45);
    vec2 z = vec2(0.0);
    float iter = 0.0;
    for(float i = 0; i < MAX_ITER; i++) {
        z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
        if(dot(z, z) > 4.0)
            return iter / MAX_ITER;
        iter++;
    }
    return 0.0;
}

vec3 hash13(float m ){
    float x = fract(sin(m) * 6.246);
    float y = fract(sin(m + x) * 6.486);
    float z = fract(sin(x + y) * 6.352);
    return vec3(x,y,z);
}
vec3 hashTime(float m, float time ){
    float x = fract( sin(m+ time) * 0.1);
    float y = fract(sin(m + x + time) * 3.5);
    float z = fract(sin(x + y+ m+ time) * 5.5);
    return vec3(x,y,z);
}

vec3 hash14(float m) {
    float x = fract(sin(m) * 43758.5453);
    float y = fract(sin(m + x) * 23421.6312);
    float z = fract(sin(x + y) * 87412.9843);
    return vec3(x, y, z);
}

vec3 colorPalette(float t) {
    // Dégradé de couleurs personnalisé
    vec3 c1 = vec3(0.8, 0.1, 0.1);
    vec3 c2 = vec3(0.9, 0.6, 0.8);
    vec3 c3 = vec3(0.7, 0.8, 0.9);
    
    // Formules pour mélanger les couleurs
    vec3 col = c1 * (1.0 - t) + c2 * t;
    col = mix(col, c3, t);

    return col;
}

void main(void) {

    vec2 uv = (gl_FragCoord.xy - 0.5 * resolution.xy) / resolution.y;

    vec3 col = vec3(0.0);

    float amp = amplitudeSon * 0.001;

    // Facteur de zoom initial et final
    float zoomStart = 1.0;
    float zoomEnd = 2.0;

    float zoom = mix(zoomStart,zoomEnd,amp);

    float m = mandelbrot(uv, zoom);

    // col = colorPalette(m);
    col += hashTime(m, u_time);
    col = pow(col, vec3(0.45));

    fragColor = vec4(col, 1.0); 
}
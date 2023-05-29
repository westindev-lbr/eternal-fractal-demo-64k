#version 330

uniform float u_time;
uniform vec2 u_viewport; 

uniform float amplitudeSon;

in vec2 vsoPosition;

/* sortie du frament shader : une couleur */
out vec4 fragColor;

#define MAX_ITER 8
#define POWER 6.0 
#define PI 3.1415926538
#define MARCH_ITERATIONS 200

float mandelbulb(vec3 position, float amplitude) {
    vec3 z = position;
    float r = 0.0, dr = 1.0, zr, theta, phi;
    for(int i = 0; i < MAX_ITER; i += 1) {
        r = length(z);
        if(r > 2.0)
            break;
        theta = acos(z.z / r) + u_time;
        phi = atan(z.y, z.x) + u_time;
        dr = pow(r, POWER - 1.) * POWER * dr + 1.;
        zr = pow(r, POWER);
        theta *= POWER;
        phi *= POWER;

        // Essai d'ajoute amplitude : 
        //float dis = amplitude * sin(2.0 * PI * r * amplitude);

        z = vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta)) * (zr ) + position;
    }
    return log(r) * r / dr * 0.5;
}



void main(void) {

    // float amp = amplitudeSon * 0.0015;
    float amp = amplitudeSon * 0.0012 ;

    vec3 cam_position = vec3(0.0, sin(u_time) * 1.5, cos(u_time) * 1.5);
    vec3 cam_direction = normalize(cam_position);

    vec3 ray_direction = normalize(vec3(0.0, -cam_direction.z, cam_direction.y) * vsoPosition.x + vec3(vsoPosition.y, 0.0, 0.0) - cam_direction);

    vec3 ray = cam_position;


    vec3 m = vec3(sin(u_time) * 1.5,cos(u_time) *2.5, 1.0);
    
    float d;


    for(int i = 0; i < MARCH_ITERATIONS; i += 1) {
        d = mandelbulb(ray, amp )  ;
        ray += ray_direction * d;
        //m *= 1.0 - amp;
         //m -= vec3(0.005, 0.002, 0.002); // Values of this vec3 can't be higher than (1 / MARCH_ITERATIONS).
         m -= vec3(0.02, 0.01, 0.01); // Values of this vec3 can't be higher than (1 / MARCH_ITERATIONS).
        if(d < 0.002)
            break;
    }

    fragColor = vec4(m, 1.0);
    //fragColor = vec4(normalize(amplitudeSon),0.0, 0.0, 1.0);

}

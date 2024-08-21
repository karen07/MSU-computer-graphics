#version 430
in vec3 Position;
in vec3 Normal;

uniform vec4 LightPosition;
uniform vec3 LightIntensity;
uniform vec3 Kd; 
uniform vec3 Ka; 
uniform vec3 Ks;
uniform float Shininess;
uniform int Set_fog;
uniform float Fog_y;
uniform float Time;
uniform vec2 Resolution;

layout( location = 0 ) out vec4 FragColor;
vec3 ads( )
{
    vec3 n = normalize( Normal );
    vec3 s = normalize( vec3(LightPosition) - Position );
    vec3 v = normalize(vec3(-Position));
    vec3 r = reflect( -s, n );
    return LightIntensity * (Ka + Kd * max(dot(s, n), 0.0 ) + Ks * pow(max( dot(r,v), 0.0 ), Shininess ));
}

float random (in vec2 st) { 
    return fract(sin(dot(st.xy ,
                         vec2(12.9898 , 78.233 + Time)))* 
        43758.5453123);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) + 
            (c - a)* u.y * (1.0 - u.x) + 
            (d - b) * u.x * u.y;
}

#define OCTAVES 5
float fbm (in vec2 st) {
    // Initial values
    float value = 0.0;
    float amplitud = .5;
    float frequency = 0.;
    //
    // Loop of octaves
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitud * noise(st);
        st *= 2.;
        amplitud *= .5;
    }
    return value;
}

void main() {
	if((Set_fog == 1) && (Position.y < -0.4)){
		float dist_z = abs(Position.z);
		float fogFactor = pow((7 - dist_z),2) / 36;
		float fogFactor_y =  max( 0.01/pow(abs(Position.y) - 0.4, 2), 1.0);

		vec2 st = gl_FragCoord.xy / Resolution.xy;
		st.x *= Resolution.x / Resolution.y;

		fogFactor = clamp(fbm(vec2( st * 3)) * fogFactor * fogFactor_y, 0.0, 1.0 );
		vec3 color = mix(vec3(1, 1, 1), ads(),  fogFactor );
		FragColor = vec4(color, 1.0);
	}else{
		FragColor = vec4(ads(), 1.0);
	}
}
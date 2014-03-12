
uniform sampler2DRect tex;

varying vec2 texcoord0;
vec4 colour;
uniform vec2 pos;
float distance;
int dist;
int modulo;

float radius = .5;

void main()
{
    colour = texture2DRect(tex, texcoord0);

    distance = length(pos-texcoord0);
    
    colour = colour / (distance/1000.0);
    
    dist = int(distance);
    modulo = mod(30.0,dist);
    
    gl_FragColor = colour;
}
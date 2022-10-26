#version 330

// from https://github.com/raysan5/raylib/blob/master/examples/shaders/shaders_basic_lighting.c

in vec3 fragPosition;
in vec2 fragTexCoord;
//in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// Input lighting values
uniform vec4 ambient;
uniform vec3 viewPos;

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec3 lightDot = vec3(0.0);
    vec3 normal = normalize(fragNormal);
    vec3 viewD = normalize(vec3(126,50,0) - fragPosition);
    vec3 specular = vec3(0.0);

    vec3 pos = vec3(50,25,2);

    vec3 light = -normalize(vec3(50,0,2) - vec3(50,25,2));
    
    float NdotL = 0.4;
    lightDot = vec3(0.8,1,1) *NdotL;

    finalColor = vec4(lightDot, 1);

    float specCo = 0.0;
    if (NdotL > 0.0) 
	    specCo = pow(max(0.0, dot(viewD, reflect(-(light), normal))), 16.0); // 16 refers to shine
    specular += specCo;
    
    finalColor = (texelColor*((colDiffuse + vec4(specular, 1.0))*vec4(lightDot, 1.0)));
    finalColor += texelColor*(ambient/10.0)*colDiffuse;
    // Gamma correction
    finalColor = pow(finalColor, vec4(1.0/2.2));
}

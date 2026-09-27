#version 300 es
precision mediump float;

// Material property
uniform vec3    MaterialAmbient;
uniform vec3    MaterialSpecular;
uniform vec3    MaterialDiffuse;

// Light property
uniform vec3    LightAmbient;
uniform vec3    LightSpecular;
uniform vec3    LightDiffuse;

uniform vec3    LightPosition;
uniform float   ShininessFactor;

in vec3    normalCoord;
in vec3    eyeCoord;
in vec2    vertexPos;

out vec4 FragColor;

vec3 normalizeNormal, normalizeEyeCoord, normalizeLightVec, V, R, ambient, diffuse, specular;
float sIntensity, cosAngle;

vec3 PhongShading(float tileClr)
{
    normalizeNormal   = normalize( normalCoord );
    normalizeEyeCoord = normalize( eyeCoord );
    normalizeLightVec = normalize( LightPosition - eyeCoord );
    
    // Diffuse Intensity
    cosAngle = max( 0.0, dot( normalizeNormal, normalizeLightVec ));

    // Specular Intensity
    V = -normalizeEyeCoord; // Viewer's vector
    R = reflect( -normalizeLightVec, normalizeNormal );
    sIntensity = pow( max( 0.0, dot( R, V ) ), ShininessFactor ); // Reflectivity

    // ADS color as result of Material & Light interaction
    ambient    = tileClr * MaterialAmbient  * LightAmbient;
    diffuse    = tileClr * MaterialDiffuse  * LightDiffuse;
    specular   = MaterialSpecular * LightSpecular;

    return ambient + ( cosAngle * diffuse ) + ( sIntensity * specular );
    //return ambient + ( cosAngle * diffuse );
}

float checker(vec2 worldXZ, float tileSize) 
{
    vec2 cell = floor(worldXZ / tileSize); 
    return mod(cell.x + cell.y, 2.0);
}

void main()
{
    float clr = checker(vertexPos, 0.05);
    FragColor = vec4( PhongShading(clr), 1.0 );
     //FragColor = vec4(clr, clr, clr, 1.0);
}

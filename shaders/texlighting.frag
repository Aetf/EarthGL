uniform vec4 fAmbientColor;
uniform vec4 fDiffuseColor;
uniform vec4 fSpecularColor;
uniform float fAmbientReflection;
uniform float fDiffuseReflection;
uniform float fSpecularReflection;
uniform float fShininess;
uniform sampler2D tex;

varying vec3 normal;
varying vec3 lightDir;
varying vec3 viewerDir;
varying vec2 texCoord;

void main(void)
{
    vec3 nNormal = normalize(normal);
    vec3 nLightDir = normalize(lightDir);
    vec3 nViewerDir = normalize(viewerDir);
    vec4 ambientIllumination = fAmbientReflection * fAmbientColor;
    vec4 diffuseIllumination = fDiffuseReflection * max(0.0, dot(nLightDir, nNormal)) * fDiffuseColor;
    vec4 specularIllumination = fSpecularReflection * pow(max(0.0,
                                                              dot(-reflect(nLightDir, nNormal), nViewerDir)
                                                              ), fShininess) * fSpecularColor;

    gl_FragColor = texture2D(tex, texCoord) * (ambientIllumination + diffuseIllumination) + specularIllumination;
}


varying vec2 fragmentUV;
varying vec3 fragmentNorm;
varying vec3 fragmentPos;

struct DirectionalLight {
  vec3 direction;

  vec3 color;
  float ambientIntensity;
  float diffuseIntensity;
  float specularIntensity; // for debug purposes, should be set to 1.0
};

uniform sampler2D textureUniform;
uniform vec3 cameraPos;
uniform float materialSpecularFactor; // should be >= 1.0
uniform vec3 materialEmission;
uniform DirectionalLight directionalLight;

vec4 calcDirectionalLight(vec3 normal, vec3 fragmentToCamera, DirectionalLight light) {
  vec4 ambientColor = vec4(light.color, 1) * light.ambientIntensity;

  float diffuseFactor = max(0.0, dot(normal, -light.direction));
  vec4 diffuseColor = vec4(light.color, 1) * light.diffuseIntensity * diffuseFactor;

  vec3 lightReflect = normalize( reflect( light.direction, normal ) );
  float specularFactor = pow(max(0.0, dot(fragmentToCamera, lightReflect)), materialSpecularFactor);
  vec4 specularColor = light.specularIntensity * vec4(light.color, 1)  * specularFactor;

  return ambientColor + diffuseColor + specularColor;
}

void main() {
  vec3 normal = normalize( fragmentNorm ); // normal should be corrected after interpolation
  vec3 fragmentToCamera = normalize(cameraPos - fragmentPos);

  vec4 directColor = calcDirectionalLight(normal, fragmentToCamera, directionalLight);

  vec4 linearColor = texture2D( textureUniform, fragmentUV ) * ( vec4(materialEmission, 1) + directColor );
  gl_FragColor = linearColor;
}

#version 410 core

in vec3 posViewSpace;
in vec2 tc;
in vec3 normalViewSpace;
in vec3 tangentViewSpace;
in vec3 binormalViewSpace;

uniform vec3 ka = vec3(0.05f, 0.05f, 0.05f); // material ambient color
uniform vec3 kd = vec3(0.0f, 0.0f, 0.8f); // material diffuse color
uniform vec3 ks = vec3(1.0f, 1.0f, 1.0f); // material specular color
uniform float shininess = 50.0f;

uniform vec3 la = vec3(0.9f, 0.9f, 0.9f); // light ambient color
uniform vec3 ld = vec3(0.9f, 0.9f, 0.9f); // light diffuse color
uniform vec3 ls = vec3(0.9f, 0.9f, 0.9f); // light specular color

uniform vec4 lightPosition;
uniform sampler2D tn;

out vec4 color;

void main() {
  vec3 N = normalize(normalViewSpace);
  vec3 T = normalize(tangentViewSpace);
  T = normalize(T - dot(T, N) * N);
  float handedness = (dot(cross(N, T), normalize(binormalViewSpace)) < 0.0) ? -1.0 : 1.0;
  vec3 B = normalize(cross(N, T) * handedness);
  
  mat3 TBN = mat3(T, B, N);

  vec3 normalTS = normalize((2 * texture(tn, tc) - 1).xyz);
  vec3 normalVS = normalize(TBN * normalTS);
  vec3 lightVecVS = normalize(lightPosition.xyz - posViewSpace);

  // ambient color
  vec3 ambient = ka * la;

  // diffuse color
  float d = max(0, dot(normalVS, lightVecVS));
  vec3 diffuse = d * kd * ld;

  // specular color
  vec3 viewVecVS =  normalize(-posViewSpace);
  vec3 reflected =  reflect(-lightVecVS, normalVS);
  float s = pow(max(0, dot(viewVecVS, reflected)), shininess);
  vec3 specular = s * ks * ls;

  color = vec4(ambient + diffuse + specular, 1);
}

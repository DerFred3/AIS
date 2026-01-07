#version 410 core

uniform sampler2D td;
uniform sampler2D ts;
uniform sampler2D tn;

uniform vec4 lightPosition;

uniform vec3 ka = vec3(0.05f, 0.05f, 0.05f); // material ambient color
uniform float shininess = 50.0f;
uniform vec3 la = vec3(0.9f, 0.9f, 0.9f); // light ambient color
uniform vec3 ld = vec3(0.9f, 0.9f, 0.9f); // light diffuse color
uniform vec3 ls = vec3(0.9f, 0.9f, 0.9f); // light specular color

in vec2 tc;
in vec3 posViewSpace;
in vec3 vertNormal;
in vec3 vertTangent; 
in vec3 vertBinormal;

out vec4 color;

void main() {
  mat3 TBN = transpose(mat3(vertTangent, vertBinormal, vertNormal));

  vec3 normal_tangentSpace = (2 * texture(tn, tc) - 1).xyz;
  vec3 lightVec_viewSpace = normalize(lightPosition.xyz - posViewSpace);

  vec3 lightDirection_tangentSpace = TBN * lightVec_viewSpace;

  // ambient color
  vec3 ambient = ka * la;

  // diffuse color
  vec3 kd = texture(td, tc).xyz;
  float d = max(0, dot(normal_tangentSpace, lightDirection_tangentSpace));
  vec3 diffuse = d * kd * ld;

  // specular color
  vec3 ks = texture(ts, tc).xyz;
  vec3 viewVec =  normalize(-posViewSpace);
  vec3 eyeDirection_tangentSpace = TBN * viewVec;
  vec3 reflected =  reflect(-lightDirection_tangentSpace, normal_tangentSpace);
  float s = pow(max(0, dot(eyeDirection_tangentSpace, reflected)), shininess);
  vec3 specular = s * ks * ls;

  color = vec4(ambient + diffuse + specular, 1);
}

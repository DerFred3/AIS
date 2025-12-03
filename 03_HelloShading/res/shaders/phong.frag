#version 410 core

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;
in vec3 DiffuseColor;
in vec3 ViewPos;

out vec4 color;

void main()
{
  vec3 norm = normalize(Normal);

  vec3 lightColor = vec3(1.0, 1.0, 1.0);
  vec3 specularColor = vec3(1.0, 1.0, 1.0);
  float shininess = 50.0f;

  vec3 lightDir = normalize(LightPos - FragPos);
  vec3 viewDir = normalize(ViewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  
  vec3 ambient = vec3(0.05, 0.05, 0.05);
  vec3 diffuse = max(dot(norm, lightDir), 0.0) * DiffuseColor * lightColor;
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
  vec3 specular = spec * specularColor * lightColor;
  
  vec3 result = (ambient + diffuse + specular) * 0.9;
  color = vec4(result, 1.0);
}
#version 410 core

layout(location = 0) in vec3 vertexPosition;  // vertex position in object/model space
layout(location = 1) in vec3 vertexNormal;    // vertex normal in object/model space

uniform mat4 MVP; // model-view-projection Matrix

uniform vec3 kd; // material diffuse color

uniform mat4 model;
uniform vec3 lightPos; // light position
uniform vec3 viewPos;

out vec3 Color;

void main()
{
	vec3 vertPos = vec3(model * vec4(vertexPosition, 1.0));
	vec3 normal = normalize(vertexNormal);

	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 specularColor = vec3(1.0, 1.0, 1.0);
	float shininess = 50.0f;

	vec3 lightDir = normalize(lightPos - vertPos);
	vec3 viewDir = normalize(viewPos - vertPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	vec3 ambient = vec3(0.05, 0.05, 0.05);
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * kd * lightColor;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = spec * specularColor * lightColor;

	Color = (ambient + diffuse + specular) * 0.9;

	gl_Position = MVP * vec4(vertexPosition, 1);
}

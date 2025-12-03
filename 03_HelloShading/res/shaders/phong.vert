#version 410 core

layout(location = 0) in vec3 vertexPosition;  // vertex position in object/model space
layout(location = 1) in vec3 vertexNormal;    // vertex normal in object/model space

uniform mat4 MVP; // model-view-projection Matrix

uniform vec3 kd; // material diffuse color

uniform mat4 model;
uniform vec3 lightPos; // light position
uniform vec3 viewPos;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;
out vec3 DiffuseColor;
out vec3 ViewPos;

void main()
{
	FragPos = vec3(model * vec4(vertexPosition, 1.0));
	Normal = vertexNormal;
	LightPos = lightPos;
	DiffuseColor = kd;
	ViewPos = viewPos;

	gl_Position = MVP * vec4(vertexPosition, 1);
}

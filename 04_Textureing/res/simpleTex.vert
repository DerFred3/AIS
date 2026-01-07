#version 410 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBinormal;

uniform mat4 MVP; // model-view-projection Matrix
uniform mat4 MV; // model-view Matrix
uniform mat4 MVit; 

out vec2 tc;
out vec3 posViewSpace;
out vec3 vertNormal;
out vec3 vertTangent;
out vec3 vertBinormal;

void main() {
  gl_Position = MVP * vec4(vertexPosition, 1);
  tc = texCoords;
  posViewSpace = vec3(MV * vec4(vertexPosition, 1));

  vertNormal = normalize((MVit * vec4(vertexNormal, 0)).xyz);;
  vertTangent = vertexTangent;
  vertBinormal = vertexBinormal;
}

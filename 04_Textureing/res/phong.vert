#version 410 core

layout(location = 0) in vec3 vertexPosition;  // vertex position in object/model space
layout(location = 1) in vec3 vertexNormal;    // vertex normal in object/model space
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBinormal;

uniform mat4 MVP; // model-view-projection Matrix
uniform mat4 MV; // model-view Matrix
uniform mat4 MVit; // model-view inverse transpose Matrix

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 posViewSpace;
out vec2 tc;
out vec3 normalViewSpace;
out vec3 tangentViewSpace;
out vec3 binormalViewSpace;

void main()
{
  gl_Position = MVP * vec4(vertexPosition, 1);
  tc = texCoords;
  posViewSpace = vec3(MV * vec4(vertexPosition, 1));

  normalViewSpace = normalize((MVit * vec4(vertexNormal, 0)).xyz);
  tangentViewSpace = normalize((MVit * vec4(vertexTangent, 0)).xyz);
  binormalViewSpace = normalize((MVit * vec4(vertexBinormal, 0)).xyz);
}

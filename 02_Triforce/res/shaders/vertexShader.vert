#version 410 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
out vec3 outColor;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 toOriginMatrix;
uniform mat4 animationMatrix;
uniform mat4 scalingMatrix;
uniform mat4 toPosMatrix;

void main()
{
	gl_Position = toPosMatrix * scalingMatrix * animationMatrix * projectionMatrix * modelViewMatrix * toOriginMatrix * vec4(vertexPosition, 1.0f);
	outColor = vertexColor;
}

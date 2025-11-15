#version 410 core

out vec4 fragmentColor;
in vec3 outColor;

void main()
{
	fragmentColor = vec4(outColor, 1.0f);
}

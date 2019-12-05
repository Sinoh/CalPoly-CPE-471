#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertColor;
layout(location = 3) in vec3 vertColor2;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 vertex_color;
out vec3 vertex_color2;
void main()
{
	vertex_color = vertColor;
	vertex_color2 = vertColor2;
	gl_Position = P * V * M * vec4(vertPos, 1.0);
}

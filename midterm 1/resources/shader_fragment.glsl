#version 330 core
out vec3 color;
in vec3 vertex_color;
in vec3 vertex_color2;
uniform float Changer;
void main()
{
	color = vertex_color + Changer;
}

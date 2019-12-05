#version 330 core
layout(location = 0) in vec3 vertPos;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec2 center;

out vec3 vertex_to_fragment_color;
out vec2 center_of_screen;


void main()
{
	center_of_screen = center; // center of the white circle
	gl_Position = P * V * M * vec4(vertPos, 1.0);
	
}

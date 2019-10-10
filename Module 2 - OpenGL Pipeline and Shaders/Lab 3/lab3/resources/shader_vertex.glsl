#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertColor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec2 center;
uniform float time;


out vec3 vertex_to_fragment_color;
out vec2 center_of_screen;
out vec2 current_time;



void main()
{

	center_of_screen = center; // center of the white circle
	vertex_to_fragment_color = vertColor; // Color the Triangles
	center_of_screen.x += cos(time) * 45; // Add the shift to x-axis
	center_of_screen.y += sin(time) * 45; // Add the shift to y-axis

	gl_Position = P * V * M * vec4(vertPos, 1.0);
	
}

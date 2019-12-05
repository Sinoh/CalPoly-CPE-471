#version 330 core
layout(location = 0) in vec3 vertPos;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec2 center;
uniform float time;


out vec3 vertex_to_fragment_color;
out vec2 center_of_screen;




void main()
{
	center_of_screen = center;
	vec3 vertex_position = vertPos;
	if (vertex_position.z > 0.1){
		vertex_position *= 1.7 + cos(time) * 0.7;
	}else{
		vertex_position *= 1.7+ sin(time) * 0.7;
	}

	gl_Position = P * V * M * vec4(vertex_position, 1.0);
	
}

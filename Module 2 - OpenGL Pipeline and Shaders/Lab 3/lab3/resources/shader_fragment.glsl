#version 330 core
out vec3 color;

in vec3 vertex_to_fragment_color;
in vec2 center_of_screen;
in vec4 gl_FragCoord;



void main()
{
	
	if(distance(vec2(gl_FragCoord.x, gl_FragCoord.y), center_of_screen) < 20 )
	{
		color = vec3(1.0,1.0,1.0);
	}else{
		color = vertex_to_fragment_color;
		float dist_away = distance(vec2(gl_FragCoord.x, gl_FragCoord.y), center_of_screen) / 150.0;
		color += dist_away;

	}
}

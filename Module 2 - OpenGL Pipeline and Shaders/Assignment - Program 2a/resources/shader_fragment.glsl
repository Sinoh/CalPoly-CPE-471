#version 330 core
out vec3 color;

in vec3 vertex_to_fragment_color;
in vec2 center_of_screen;
in vec4 gl_FragCoord;



void main()
{

	if(distance(vec2(gl_FragCoord.x, gl_FragCoord.y), center_of_screen) < 50 )
	{
		color = vec3(1.0f, 1.0f, 1.0f);
		float dist_away = distance(vec2(gl_FragCoord.x, gl_FragCoord.y), center_of_screen) / min(center_of_screen.x, center_of_screen.y) * 4;
		color.z -= dist_away;

	}else{
		color = vec3(1.0f, 0.2f, 0.0f);
		float dist_away = distance(vec2(gl_FragCoord.x, gl_FragCoord.y), center_of_screen) / min(center_of_screen.x, center_of_screen.y);
		color.y += dist_away;


	}
}


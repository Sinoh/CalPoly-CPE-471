#version 330 core
out vec3 color;

in vec2 center_of_screen;
in vec4 gl_FragCoord;

void main()
{
	color = vec3(0.0f, 0.0f, 1.0f);
	float dist_away = distance(vec2(gl_FragCoord.x, gl_FragCoord.y), center_of_screen) / max(center_of_screen.x, center_of_screen.y) ;
	color -= dist_away;

}

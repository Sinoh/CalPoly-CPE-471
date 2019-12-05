#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
uniform vec3 campos;
void main()

{

vec3 n = normalize(vertex_normal);
vec3 lp=vec3(-10,0,-100);

//Diffuse
vec3 ld = normalize(vertex_pos - lp);
float diffuse = dot(n,ld);
color.rgb = vec3(0.5,0.5,0.5)*diffuse * 0.7f;


//Specular
vec3 cd = normalize(vertex_pos - campos);
vec3 refl = normalize(reflect(ld,n));

float spec = dot(cd,refl);
spec = clamp(spec,0,1);
spec = pow(spec,20);
color.rgb += vec3(0.5,0.5,0.5)*spec * 0.5;


//Ambiant
color.rgb += vec3(0.5, 0.5, 0.5) * .3f;
color.a = 1.0f;
}

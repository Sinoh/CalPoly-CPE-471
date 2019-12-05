#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
uniform vec3 campos;
void main()

{
vec3 n = normalize(vertex_normal);
vec3 lp=vec3(20, 0,-100);
vec3 ld = normalize(vertex_pos - lp);
float diffuse = dot(n,ld);
color.rgb = vec3(0.2f,1,0.2f);

vec3 cd = normalize(vertex_pos - campos);
vec3 refl = normalize(reflect(ld,n));

//vec3 h = normalize(cd+ld);
float spec = dot(cd,refl);
spec = clamp(spec,0,1);
spec = pow(spec,20);

color.rgb += vec3(1,1,1)*spec * 0.9;
color.rgb += vec3(0.2f, 1,0.2f) * .2;
color.a = 0.5f;

}

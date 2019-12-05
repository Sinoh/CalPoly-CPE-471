out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
uniform vec3 campos;
void main()

{
vec3 n = normalize(vertex_normal);
vec3 lp=vec3(-20,0,-100);

vec3 ld = normalize(vertex_pos - lp);
float diffuse = dot(n,ld);
color.rgb = vec3(0.2f,0.47f,1)*diffuse * 2;




vec3 cd = normalize(vertex_pos - campos);
vec3 refl = normalize(reflect(ld,n));


float spec = dot(cd,refl);

spec = clamp(spec,0,1);
spec = pow(spec,40);
color.rgb += vec3(1,1,1)*spec * 2;

color.rgb += vec3(0.2f, 0.47f,1) * 0.2f;
color.a = 1f;
#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;
uniform vec2 texOffset;
uniform sampler2D tex;
uniform sampler2D tex2;

void main()
{
vec3 n = normalize(vertex_normal);
vec3 lp=vec3(10,-20,-100);
vec3 ld = normalize(vertex_pos - lp);
float diffuse = dot(n,ld);

vec2 texCoords = vertex_tex / 6.;
texCoords += vec2(1./6. *texOffset.x, 1./6. * texOffset.y);
vec4 tcol = texture(tex, texCoords);

color = tcol;
color.a = (color.r + color.g + color.b) / 3;



}

#version 330 core
out vec3 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform vec3 campos;
uniform sampler2D tex;
uniform sampler2D tex2;

void main()
{

// Inits
color = texture(tex, vertex_tex).rgb;
vec3 normal = normalize(vertex_normal);
vec3 lightPosition = vec3(0,100, -500);
vec3 lightDirection = normalize(vertex_pos - lightPosition);
vec3 viewDirection = normalize(campos - vertex_pos);
vec3 halfwayDirection = normalize(lightDirection + viewDirection);

//Ambient
float ambient = 0;
vec3 ambientColor = ambient * color;


//Diffuse
float diffuse = dot(normal, lightDirection);
vec3 diffuseColor = diffuse * color;



//Specular
float shininess = 5;
float specularStrength = 15;
vec3 reflectionDirection = normalize(reflect(lightDirection, normal));

float specular = pow(clamp(dot(viewDirection, reflectionDirection),0 , 1), shininess);


vec3 specularColor = specularStrength * specular * color;

color = (ambientColor + diffuseColor + specularColor) * color ;
}


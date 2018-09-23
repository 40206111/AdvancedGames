#version 430

layout (location = 0) in vec3 position;

in vec3 normal;
out vec4 color;

struct DirectionalLight
{
    vec3 color;
    vec3 direction;
};


/*
This shader applies a Lambert shading model and uses a directional light as single light source.
*/

void main()
{
	// directional light properties
	DirectionalLight light;
	light.color = vec3(0.5f, 0.5f, 0.5f);
	light.direction = vec3(-2.0f, 0.5f, 3.7f);

    // ambient 
    float ambientIntensity = .4f;
    vec3 ambient = light.color * ambientIntensity;

    // diffuse
    float diffuseIntensity = 1.0f;
	vec3 diffuse = .2f * max(dot(light.direction, normal),0) * light.color * diffuseIntensity;
	
	// final colour
    color = vec4(ambient + diffuse, 1.0);
}
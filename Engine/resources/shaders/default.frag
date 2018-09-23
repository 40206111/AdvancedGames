#version 430

layout (location = 0) in vec3 position;

in vec3 normal;
out vec4 color;

// light
struct DirectionalLight
{
    vec3 color;
    vec3 direction;
};

vec3 getLightDirection(){
	return vec3(-2.0f, 0.5f, 3.7f);
}

vec3 getLightColour(){
	return vec3(.5f, .5f, .7f);
}


// material
vec4 computeColour(){
	
    // ambient 
    float ambientIntensity = .4f;
    vec3 ambient = getLightColour() * ambientIntensity;

    // diffuse
    float diffuseIntensity = 1.0f;
	vec3 diffuse = .2f * max(dot(getLightDirection(), normal),0) * getLightColour() * diffuseIntensity;
	
	// final colour
    return vec4(ambient + diffuse, 1.0);
}

// return fragment colour
void main()
{
    color = computeColour();
}


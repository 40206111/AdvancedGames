#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 UVs;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;

void main(){
	texCoord = UVs;
	gl_Position = projection * view * model * vec4(pos, 1.0);
	

}

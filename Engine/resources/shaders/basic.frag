#version 330 core

in vec4 col;
in vec2 texCoord;

out vec4 color;

uniform sampler2D myTexture;

void main(){
	color = texture(myTexture, texCoord);
	//color = vec4 (.5, .5, .5, 1.0);
}
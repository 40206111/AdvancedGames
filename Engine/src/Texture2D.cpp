#pragma once
#include <iostream>

#include "Texture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture2D::Texture2D(): m_texture(0) {

}

Texture2D::~Texture2D() {

}

// accessors


// functions
bool Texture2D::loadTexture(const string& filename, bool generateMipMaps) {
	int width, height, components;

	unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &components, STBI_rgb_alpha);
	if (imageData == NULL) {
		std::cerr << "Error loading texture image: '" << filename << "'" << std::endl;
		return false;

	}

	// flip image vertically
	 

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	if (generateMipMaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// free memory taken by imageData
	stbi_image_free(imageData);
	// unbind image
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Texture2D::bind(GLuint var) {
	glBindTexture(GL_TEXTURE_2D, m_texture);

}
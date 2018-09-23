#include <iostream>

#include "PolyMesh.h"
#include "ShaderProgram.h"


PolyMesh::PolyMesh() {}

PolyMesh::PolyMesh(PrimitiveType type)
{
	m_creationMode = SIMPLE;

	glm::vec3 vertices[36];
	glm::vec3 normals[36];
	glm::vec2 UVs[36];

	switch (type)
	{
	case TRIANGLE:
		std::cout << "create triangle polymesh" << std::endl;

		// Create triangle
		vertices[0] = glm::vec3(-1.0, -1.0, 0.0);
		vertices[1] = glm::vec3(0, 1.0, 0.0);
		vertices[2] = glm::vec3(1.0, -1.0, 0.0);
		normals[0] = glm::vec3(0.0f, 1.0f, 0.0f);
		normals[1] = glm::vec3(0.0f, 1.0f, 0.0f);
		normals[2] = glm::vec3(0.0f, 1.0f, 0.0f);

		// number of vertices
		m_vertexNum = 3;

		break;

	case QUAD:
		std::cout << "create quad polymesh" << std::endl;

		// create quad vertices
		vertices[0] = glm::vec3(-1.0f, 0.0f, -1.0f);
		vertices[1] = glm::vec3(1.0f, 0.0f, -1.0f);
		vertices[2] = glm::vec3(-1.0f, 0.0f, 1.0f);
		vertices[3] = glm::vec3(1.0f, 0.0f, -1.0f);
		vertices[4] = glm::vec3(-1.0f, 0.0f, 1.0f);
		vertices[5] = glm::vec3(1.0f, 0.0f, 1.0f);

		// create texture coordinates
		UVs[0] = glm::vec2(0.0f, 0.0f);
		UVs[1] = glm::vec2(1.0f, 0.0f);
		UVs[2] = glm::vec2(1.0f, 1.0f);
		UVs[3] = glm::vec2(0.0f, 0.0f);
		UVs[4] = glm::vec2(1.0f, 1.0f);
		UVs[5] = glm::vec2(0.0f, 1.0f);

		// create normals
		normals[0] = glm::vec3(0.0f, 1.0f, 0.0f);
		normals[1] = glm::vec3(0.0f, 1.0f, 0.0f);
		normals[2] = glm::vec3(0.0f, 1.0f, 0.0f);
		normals[3] = glm::vec3(0.0f, 1.0f, 0.0f);
		normals[4] = glm::vec3(0.0f, 1.0f, 0.0f);
		normals[5] = glm::vec3(0.0f, 1.0f, 0.0f);

		// number of vertices
		m_vertexNum = 6;

		break;

	case CUBE:
		std::cout << "create cube polymesh" << std::endl;

		//// create cube
		//vertices[0] = glm::vec3(-1.0f, -1.0f, -1.0f);
		//vertices[1] = glm::vec3(1.0f, -1.0f, -1.0f);
		//vertices[2] = glm::vec3(1.0f, 1.0f, -1.0f);
		//vertices[3] = glm::vec3(-1.0f, -1.0f, -1.0f);
		//vertices[4] = glm::vec3(1.0f, 1.0f, -1.0f);
		//vertices[5] = glm::vec3(-1.0f, 1.0f, -1.0f);
		//vertices[6] = glm::vec3(-1.0f, -1.0f, 1.0f);
		//vertices[7] = glm::vec3(1.0f, -1.0f, 1.0f);
		//vertices[8] = glm::vec3(1.0f, 1.0f, 1.0f);
		//vertices[9] = glm::vec3(-1.0f, -1.0f, 1.0f);
		//vertices[10] = glm::vec3(1.0f, 1.0f, 1.0f);
		//vertices[11] = glm::vec3(-1.0f, 1.0f, 1.0f);
		//vertices[12] = glm::vec3(-1.0f, -1.0f, -1.0f);
		//vertices[13] = glm::vec3(1.0f, -1.0f, -1.0f);
		//vertices[14] = glm::vec3(1.0f, -1.0f, 1.0f);
		//vertices[15] = glm::vec3(-1.0f, -1.0f, -1.0f);
		//vertices[16] = glm::vec3(1.0f, -1.0f, 1.0f);
		//vertices[17] = glm::vec3(-1.0f, -1.0f, 1.0f);
		//vertices[18] = glm::vec3(-1.0f, 1.0f, -1.0f);
		//vertices[19] = glm::vec3(1.0f, 1.0f, -1.0f);
		//vertices[20] = glm::vec3(1.0f, 1.0f, 1.0f);
		//vertices[21] = glm::vec3(-1.0f, 1.0f, -1.0f);
		//vertices[22] = glm::vec3(1.0f, 1.0f, 1.0f);
		//vertices[23] = glm::vec3(-1.0f, 1.0f, 1.0f);
		//vertices[24] = glm::vec3(-1.0f, -1.0f, -1.0f);
		//vertices[25] = glm::vec3(-1.0f, 1.0f, -1.0f);
		//vertices[26] = glm::vec3(-1.0f, 1.0f, 1.0f);
		//vertices[27] = glm::vec3(-1.0f, -1.0f, -1.0f);
		//vertices[28] = glm::vec3(-1.0f, 1.0f, 1.0f);
		//vertices[29] = glm::vec3(-1.0f, -1.0f, 1.0f);
		//vertices[30] = glm::vec3(1.0f, -1.0f, -1.0f);
		//vertices[31] = glm::vec3(1.0f, 1.0f, -1.0f);
		//vertices[32] = glm::vec3(1.0f, 1.0f, 1.0f);
		//vertices[33] = glm::vec3(1.0f, -1.0f, -1.0f);
		//vertices[34] = glm::vec3(1.0f, 1.0f, 1.0f);
		//vertices[35] = glm::vec3(1.0f, -1.0f, 1.0f);

		////normals
		//normals[0] = glm::vec3(0.0f, 0.0f, -1.0f);
		//normals[1] = glm::vec3(0.0f, 0.0f, -1.0f);
		//normals[2] = glm::vec3(0.0f, 0.0f, -1.0f);
		//normals[3] = glm::vec3(0.0f, 0.0f, -1.0f);
		//normals[4] = glm::vec3(0.0f, 0.0f, -1.0f);
		//normals[5] = glm::vec3(0.0f, 0.0f, -1.0f);
		//normals[6] = glm::vec3(0.0f, 0.0f, 1.0f);
		//normals[7] = glm::vec3(0.0f, 0.0f, 1.0f);
		//normals[8] = glm::vec3(0.0f, 0.0f, 1.0f);
		//normals[9] = glm::vec3(0.0f, 0.0f, 1.0f);
		//normals[10] = glm::vec3(0.0f, 0.0f, 1.0f);
		//normals[11] = glm::vec3(0.0f, 0.0f, 1.0f);
		//normals[12] = glm::vec3(0.0f, -1.0f, 0.0f);
		//normals[13] = glm::vec3(0.0f, -1.0f, 0.0f);
		//normals[14] = glm::vec3(0.0f, -1.0f, 0.0f);
		//normals[15] = glm::vec3(0.0f, -1.0f, 0.0f);
		//normals[16] = glm::vec3(0.0f, -1.0f, 0.0f);
		//normals[17] = glm::vec3(0.0f, -1.0f, 0.0f);
		//normals[18] = glm::vec3(0.0f, 1.0f, 0.0f);
		//normals[19] = glm::vec3(0.0f, 1.0f, 0.0f);
		//normals[20] = glm::vec3(0.0f, 1.0f, 0.0f);
		//normals[21] = glm::vec3(0.0f, 1.0f, 0.0f);
		//normals[22] = glm::vec3(0.0f, 1.0f, 0.0f);
		//normals[23] = glm::vec3(0.0f, 1.0f, 0.0f);
		//normals[24] = glm::vec3(-1.0f, 0.0f, 0.0f);
		//normals[25] = glm::vec3(-1.0f, 0.0f, 0.0f);
		//normals[26] = glm::vec3(-1.0f, 0.0f, 0.0f);
		//normals[27] = glm::vec3(-1.0f, 0.0f, 0.0f);
		//normals[28] = glm::vec3(-1.0f, 0.0f, 0.0f);
		//normals[29] = glm::vec3(-1.0f, 0.0f, 0.0f);
		//normals[30] = glm::vec3(1.0f, 0.0f, 0.0f);
		//normals[31] = glm::vec3(1.0f, 0.0f, 0.0f);
		//normals[32] = glm::vec3(1.0f, 0.0f, 0.0f);
		//normals[33] = glm::vec3(1.0f, 0.0f, 0.0f);
		//normals[34] = glm::vec3(1.0f, 0.0f, 0.0f);
		//normals[35] = glm::vec3(1.0f, 0.0f, 0.0f);

		//// number of vertices
		//m_vertexNum = 36;

		break;
	}
	
	initMesh(vertices, UVs, normals);
}

void PolyMesh::initMesh(glm::vec3* vertices, glm::vec2* UVs, glm::vec3* normals) {
	// generate unique vertex vector (no duplicates)
	//m_vertices = std::vector<glm::vec3>(std::begin(vertices), std::end(vertices));
	//createUniqueVertices();

	std::cout << "Vertex cordinates: " << sizeof(vertices) << std::endl;
	std::cout << "Vertex cordinates: " << glm::to_string(vertices[0]) << std::endl;

	std::cout << "normals cordinates: " << sizeof(normals) << std::endl;
	std::cout << "normals cordinates: " << glm::to_string(normals[0]) << std::endl;

	// vertex buffer
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * m_vertexNum, &vertices[0], GL_STATIC_DRAW);

	// vertex array
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, NULL);
	glEnableVertexAttribArray(0);

	// texture coordinates buffer
	glGenBuffers(1, &m_tbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_tbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UVs[0]) * m_vertexNum, &UVs[0], GL_STATIC_DRAW);


	// texture coordinates array
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, (void*)0);
	glEnableVertexAttribArray(1);


	// normals buffer
	glGenBuffers(1, &m_nbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_nbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * m_vertexNum, &normals[0], GL_STATIC_DRAW);


	// normals array
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, (void*)0);
	glEnableVertexAttribArray(2);

}

void PolyMesh::initMesh(GLfloat* vertices, glm::vec2* UVs, glm::vec3* normals) {
	// generate unique vertex vector (no duplicates)
	//m_vertices = std::vector<glm::vec3>(std::begin(vertices), std::end(vertices));
	//createUniqueVertices();

	std::cout << "Vertex cordinates: " << sizeof(vertices) << std::endl;
	std::cout << "Vertex cordinates: " << &vertices[0] << std::endl;

	// vertex buffer
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * m_vertexNum, &vertices[0], GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * m_vertexNum, &vertices[0], GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// vertex array
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, NULL);
	glEnableVertexAttribArray(0);
}

PolyMesh::PolyMesh(GLfloat* vertices, GLuint num, GLbyte method) {
	m_vertexNum = num;
	m_creationMode = SIMPLE;

	switch (method)
	{
	case 1:
		std::cout << "create geometry from list of vertex positions" << std::endl;

		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, 12 * num, &vertices[0], GL_STATIC_DRAW);

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, NULL);
		glEnableVertexAttribArray(0);

		break;

	case 2:
		std::cout << "create geometry from list of vertex positions and colours" << std::endl;

		// generate vertex buffer object (allocates memory on Graphics card)
		glGenBuffers(1, &m_vbo);
		// make vbo the current (active) buffer (there can only be one at a time)
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		// allocate data to buffer. We use a static buffer, meaning that the geometry data will not change
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// vertex array object, which holds information about the buffer. This is what is used to draw the genoetry, not the vertex buffer.
		glGenVertexArrays(1, &m_vao);
		// bind buffer
		glBindVertexArray(m_vao);

		// tell the graphics card what kind of data our vertex array contains
		// position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, NULL);
		glEnableVertexAttribArray(0);
		// colour
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)(sizeof(GLfloat) * 3));
		glEnableVertexAttribArray(1);

		break;

	case 3:
		std::cout << "create geometry from list of vertex positions and UVs" << std::endl;
		// generate vertex buffer object (allocates memory on Graphics card)
		glGenBuffers(1, &m_vbo);
		// make vbo the current (active) buffer (there can only be one at a time)
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		// allocate data to buffer. We use a static buffer, meaning that the geometry data will not change
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 5 * num, &vertices[0], GL_STATIC_DRAW);

		// vertex array object, which holds information about the buffer. This is what is used to draw the genoetry, not the vertex buffer.
		glGenVertexArrays(1, &m_vao);
		// bind buffer
		glBindVertexArray(m_vao);

		// tell the graphics card what kind of data our vertex array contains
		// position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, NULL);
		glEnableVertexAttribArray(0);
		// UV
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)(sizeof(GLfloat) * 3));
		glEnableVertexAttribArray(1);
		break;

	case 4:
		std::cout << "create geometry from list of vertex positions, colours and UVs (not implemented)" << std::endl;

		break;
	}
}

// utility method: creates a vector of unique vertices (no duplicates) from array of vertices
void PolyMesh::createUniqueVertices() {
	std::vector<glm::vec3> temp_vertices = m_vertices;

	unsigned int i = 0;
	while (i < m_vertices.size()) {
		unsigned int j = i + 1;
		bool duplicateFound = false;
		while (j < m_vertices.size() && !duplicateFound) {
			if (m_vertices.at(i) == m_vertices.at(j)) {
				duplicateFound = true;
			}
			j++;
		}
		if (duplicateFound) {
			m_vertices.erase(m_vertices.begin() + i);
		}
		else {
			++i;
		}
	}
}

void PolyMesh::deleteMesh(){
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

void PolyMesh::bindGeometry() {
	glBindVertexArray(getVAO());

}

void PolyMesh::draw(const Application &app){
	// draw geometry
	switch (getCreationMode()) {
	case PolyMesh::SIMPLE:
		//std::cout << "drawing, simple mode" << std::endl;
		glDrawArrays(GL_TRIANGLES, 0, getVertexNum());
		break;
	case PolyMesh::INDEXED:
		//std::cout << "drawing, indexed mode" << std::endl;
		glDrawElements(GL_TRIANGLES, getVertexNum(), GL_UNSIGNED_INT, 0);
		break;
	}
	glBindVertexArray(0);

}




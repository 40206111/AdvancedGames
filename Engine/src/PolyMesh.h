#pragma once

#include "Geometry.h"
#include "Transform.h"

// GLM
#include <glm/glm.hpp>
#include "glm/ext.hpp"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

#include <vector>

using std::vector;

class PolyMesh : public Geometry {
public:

	enum CreationMode {
		SIMPLE, //uses a single vertex array
		INDEXED // used and indexed array, which requires a vertex array and an index array
	};

	enum PrimitiveType
	{
		TRIANGLE,
		QUAD,
		CUBE
	};

	// default constructor. Not implmented.
	PolyMesh();
		
	// create Mesh from array of data, number of vertices, and method number.
	// the method number specifies the information present in the array
	// method = 1: vertex coordinates only
	// method = 2: v and colour
	// method = 3: v and uv
	// method = 4: v and colour and uv 
	PolyMesh(GLfloat* vertices, GLuint vertexNum, GLbyte method);

	// create mesh from primitive type (triangle, quad, cube)
	PolyMesh(PrimitiveType type);

	// create geometry buffers
	void initMesh(glm::vec3* vertices, glm::vec2* UVs, glm::vec3* normals);

	void initMesh(GLfloat* vertices, glm::vec2* UVs, glm::vec3* normals);


	// accessors
	GLuint getVBO() const { return m_vbo; }
	GLuint getVAO() const { return m_vao; }
	GLuint getShaderProgram() const { return m_shaderProgram; }
	GLuint getVertexNum() const { return m_vertexNum; }
	CreationMode getCreationMode() const { return m_creationMode; }

	// utility
	// create vector of unique vertices
	void createUniqueVertices();

	// other methods
	void deleteMesh();
	void bindGeometry() override;
	void draw(const Application &app) override;


private:
	// generic variables
	CreationMode m_creationMode;
	GLuint m_vertexNum; // number of vertices
	GLuint m_vertexNumUnique; // number of unique vertices i.e., without duplicates.
	std::vector<glm::vec3> m_vertices; // unique vertices (no duplicates)

	// openGL specific variables
	GLuint m_vao; // vertex array object
	GLuint m_vbo; // vertex buffer object
	GLuint m_nao; // normal array object
	GLuint m_nbo; // normal buffer object
	GLuint m_tao; // texture coordinates array object
	GLuint m_tbo; // texture coordinates buffer object

	GLuint m_shaderProgram; // shader program used to draw Mesh

};
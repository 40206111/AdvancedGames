#pragma once

#include <iostream>
#include <vector>

// GLM
#include <glm/glm.hpp>
#include "glm/ext.hpp"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Application.h"
#include "GLFWApplication.h"
#include "PolyMesh.h"
#include "Texture2D.h"
#include "Material.h"
#include "Phong.h"
#include "Model.h"
#include "Scene.h"
#include "Rasterizer/Rasterizer.h"
#include "Raytracer/Raytracer.h"


int main(int argc, char *argv[])
{
	/*
		CREATE APP CONTAINER
	*/
	GLFWApplication app = GLFWApplication::GLFWApplication("GLFW engine test", 800, 600);
	app.init();

	/*
		CREATE SCENE
	*/
	//PolyMesh mesh = PolyMesh::PolyMesh(vertices, 6, 3); // create model from list of vertices and associated UVs
	PolyMesh mesh = PolyMesh::PolyMesh(PolyMesh::QUAD);
	// material
	Phong phong = Phong::Phong(glm::vec3(.2f,.2f,.2f), glm::vec3(.5f, .2f, .2f), glm::vec3(.2f, .2f, .2f), 12);
	// Model
	Model quad = Model::Model(&mesh, phong);


	// transformations
	mesh.scale(glm::vec3(.5f, .5f, .5f));

	// load texture
	Texture2D texture;
	texture.loadTexture("./resources/textures/marbl01.jpg", true);
		
	// compute view matrix using glm::lookat
	glm::mat4  view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	app.setView(view);

	// projection matrix
	glm::mat4 projection = glm::perspective(45.0f, 1.0f * app.getWidth() / app.getHeight(), 0.001f, 200.0f);
	app.setProjection(projection);

	Scene scene = Scene::Scene();

	/*
	CREATE RENDERER
	*/
	Rasterizer renderer = Rasterizer::Rasterizer();
	//Raytracer renderer = Raytracer::Raytracer();

	// game loop
	while (app.shouldRun())
	{
		// show frame rate
		app.showFPS();

		// manage interaction
		app.pollEvents();

		// animate
		mesh.rotate(.001f, glm::vec3(1.0f, 2.0f, 3.0f));


		/*
		**	RENDER
		*/
		
		// clear buffer
		app.clear();

		// draw geometry to buffer
		renderer.draw(quad, app); // works for rasterizer
		//renderer.draw(scene); // works for raytracer

		// display image buffer
		app.display();

	}

	app.terminate();
	mesh.deleteMesh();
	return EXIT_SUCCESS;
}


#pragma once

#include <iostream>
#include <vector>
#include <chrono>

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
#include "TerrainGraph.h"

using namespace std;

int main(int argc, char *argv[])
{
	/*
		CREATE APP CONTAINER
	*/
	GLFWApplication app = GLFWApplication::GLFWApplication("GLFW engine test", 1500, 1500);
	app.init();

	/*
		CREATE SCENE
	*/
	//PolyMesh mesh = PolyMesh::PolyMesh(vertices, 6, 3); // create model from list of vertices and associated UVs
	//detail cliff tris.obj
	//coral_hard_02.obj
	//stabbs-8k.obj
	// mesh1-30k-tris.obj
	// mesh1-130k-tris.obj
	// bumps.obj
	// maybe saddle.obj
	// peak.obj
	// valley.obj
	// terrain2b.obj
	// terrain3a.obj
	string source = "./resources/models/stabbs-8k.obj";
	float scale = 0.1f;
	int doTerrain = 5;
	switch (doTerrain)
	{
	case (0):
		source = "./resources/models/stabbs-8k.obj";
		scale = 0.1f;
		break;
	case (1):
		source = "./resources/models/terrain2b.obj";
		scale = 7.0f;
		break;
	case (2):
		source = "./resources/models/terrain3a.obj";
		scale = 7.0f;
		break;
	case (4):
		source = "./resources/models/mesh1-30k-tris.obj";
		scale = 0.1f;
		break;
	case (5):
		source = "./resources/models/mesh1-130k-tris.obj";
		scale = 0.1f;
		break;
	default:
		break;
	}
	PolyMesh mesh = PolyMesh::PolyMesh(source);
	mesh.rotate(-3.0f * 3.1415f / 4.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	// material
	Phong phong = Phong::Phong(glm::vec3(.2f, .2f, .2f), glm::vec3(.5f, .2f, .2f), glm::vec3(.2f, .2f, .2f), 12);
	// Model
	Model quad = Model::Model(&mesh, phong);


	// transformations
	mesh.scale(glm::vec3(scale));

	TerrainGraph graph;
	graph.SetPolyMesh(&mesh);
	cout << "Pre-creation" << endl;
	graph.CreateGraph();
	cout << "Pre-analysis" << endl;
	graph.AnalyseGraph();
	cout << "Pre-colouring" << endl;
	graph.ColourWaterVals();
	cout << "Graph done" << endl;

	// load texture
	Texture2D texture;
	texture.loadTexture("./resources/textures/marbl01.jpg", true);

	// compute view matrix using glm::lookat
	glm::mat4  view = glm::lookAt(glm::vec3(7.0f, 7.0f, 7.0f), glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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
		//mesh.rotate(.0001f, glm::vec3(.0f, 1.0f, .0f));

		static bool cHeld = false;
		if (!cHeld && glfwGetKey(app.getWindow(), GLFW_KEY_C) == GLFW_PRESS) {
			graph.ColourShapeResults();
			cHeld = true;
		}
		if (cHeld && glfwGetKey(app.getWindow(), GLFW_KEY_C) == GLFW_RELEASE) {
			cHeld = false;
		}
		static bool vHeld = false;
		if (!vHeld && glfwGetKey(app.getWindow(), GLFW_KEY_V) == GLFW_PRESS) {
			graph.ColourWaterGroup();
			vHeld = true;
		}
		if (vHeld && glfwGetKey(app.getWindow(), GLFW_KEY_V) == GLFW_RELEASE) {
			vHeld = false;
		}
		static bool zHeld = false;
		if (!zHeld && glfwGetKey(app.getWindow(), GLFW_KEY_Z) == GLFW_PRESS) {
			graph.ColourGradients();
			zHeld = true;
		}
		if (zHeld && glfwGetKey(app.getWindow(), GLFW_KEY_Z) == GLFW_RELEASE) {
			zHeld = false;
		}
		static bool xHeld = false;
		if (!xHeld && glfwGetKey(app.getWindow(), GLFW_KEY_X) == GLFW_PRESS) {
			graph.ColourWaterEdges();
			xHeld = true;
		}
		if (xHeld && glfwGetKey(app.getWindow(), GLFW_KEY_X) == GLFW_RELEASE) {
			xHeld = false;
		}
		if (glfwGetKey(app.getWindow(), GLFW_KEY_B) == GLFW_PRESS) {
			graph.ColourWaterBodies();
		}
		if (glfwGetKey(app.getWindow(), GLFW_KEY_G) == GLFW_PRESS) {
			graph.ColourRainfallBodies();
		}
		if (glfwGetKey(app.getWindow(), GLFW_KEY_N) == GLFW_PRESS) {
			graph.ColourWaterVals();
		}

		float rotSpeed = 0.002f;
		if (glfwGetKey(app.getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
			mesh.rotate(-rotSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (glfwGetKey(app.getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
			mesh.rotate(rotSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (glfwGetKey(app.getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
			mesh.rotate(-rotSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (glfwGetKey(app.getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
			mesh.rotate(rotSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		if (glfwGetKey(app.getWindow(), GLFW_KEY_Q) == GLFW_PRESS) {
			mesh.rotate(-rotSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		if (glfwGetKey(app.getWindow(), GLFW_KEY_E) == GLFW_PRESS) {
			mesh.rotate(rotSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
		}


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


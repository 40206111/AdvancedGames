#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "OBJLoader.h"
#include "PolyMesh.h"

enum TerrainShape { PEAK, PIT, RIDGE_V, TROUGH_V, HILLTOP, HILLBASE, SLOPE, FLAT, SADDLE };
enum EdgeType { DEFAULT, RIDGE_E, TROUGH_E, WATERSHED };

class TerrainEdge;

class TerrainVertex {
public:
	TerrainVertex();
	~TerrainVertex();

	glm::vec3 GetPos() { return m_pos; }
	glm::vec3 GetNormal() { return m_normal; }
	float GetGradient() { return m_gradient; }
	std::vector<TerrainEdge*> GetEdges() { return m_edges; }
	TerrainShape GetShape() { return m_shape; }
	int GetFlowGroup() { return m_waterShedID; }

	void SetPos(glm::vec3 p) { m_pos = p; }
	void SetNormal(glm::vec3 n) { m_normal = n; CalculateGradient(); }
	void AddEdge(TerrainEdge* e) { m_edges.push_back(e); }

	// Find the shape of the vertex
	void CalculateShape();
	bool CalculateFlow();
	void MakeFlowGroup(std::vector<TerrainVertex*> &visited, int id);

	void AddFlowSource(TerrainVertex* source);

private:
	// Calculate gradient of vertex normal
	void CalculateGradient();
	// Put edges in anti-clockwise order from x axis
	void OrderEdges();
	// Are edges order by rotation
	bool m_ordered = false;

	glm::vec3 m_pos;
	glm::vec3 m_normal;
	float m_gradient;
	std::vector<TerrainEdge*> m_edges;
	TerrainShape m_shape;
	std::vector<std::vector<TerrainEdge*>> m_groups;
	std::vector<TerrainEdge*> m_steepestUp;
	std::vector<TerrainEdge*> m_steepestDown;
	float m_simVal = 0.1f;
	std::vector<TerrainVertex*> m_flowFrom;
	int m_waterShedID = -1;
};

class TerrainEdge {
public:
	TerrainEdge();
	~TerrainEdge();

	EdgeType GetType() { return m_type; }
	float GetGradient(TerrainVertex* root);
	TerrainVertex* GetPoint(int p) { return m_points[p]; }
	TerrainVertex* GetOtherPoint(TerrainVertex* tv);

	void SetPoints(TerrainVertex* v1, TerrainVertex* v2);
	void SetType(EdgeType t);

	// Return the angle in radians between a given vector and this edge (from the given root node)
	// Ignores the y axis
	float GetXZRotatation(TerrainVertex* root, glm::vec3 origin);

	void FlowDown(TerrainVertex* from);

private:
	// Calculate gradient between points
	void CalculateGradient();

	TerrainVertex* m_points[2]{ nullptr, nullptr };
	float m_gradient;
	EdgeType m_type;
	glm::vec3 m_normDir;
	glm::vec3 m_normDirFlat;
};

class TerrainGraph {
public:
	TerrainGraph();
	~TerrainGraph();

	void SetPolyMesh(PolyMesh* pm) { m_pm = pm; }

	void CreateGraph();
	void AnalyseGraph();
	void ColourWaterGroup();
	void ColourShapeResults();

private:
	PolyMesh* m_pm;
	std::vector<TerrainVertex*> m_verts;
	std::vector<TerrainEdge*> m_edges;
	std::vector<glm::vec4> m_uniqueColours;
	std::vector<glm::vec4> m_nonUniqueColours;

	TerrainEdge* FindExistingEdge(int first, int second);
	std::vector<std::pair<int, int>> m_prevEdgePairs;
	std::vector<TerrainEdge*> m_prevEdges;
	std::vector<TerrainVertex*> m_flowless;
};

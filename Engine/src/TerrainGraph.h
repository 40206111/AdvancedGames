#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "OBJLoader.h"
#include "PolyMesh.h"

enum TerrainShape { DEFAULT_V, PEAK, PIT, RIDGE_V, TROUGH_V, HILLTOP, HILLBASE, SLOPE, FLAT, SADDLE };
enum EdgeType { DEFAULT_E, RIDGE_E, TROUGH_E, WATERSHED };
enum WaterType { NONE, RIVER, LAKE };

class TerrainEdge;
class TerrainFace;

class TerrainVertex {
public:
	TerrainVertex();
	~TerrainVertex();

	glm::vec3 GetPos() { return m_pos; }
	glm::vec3 GetNormal() { return m_normal; }
	float GetGradient() { return m_gradient; }
	std::vector<TerrainEdge*> GetEdges() { return m_edges; }
	TerrainShape GetShape() { return m_shape; }
	TerrainVertex* GetFlowTo() { return m_flowTo; }
	int GetFlowGroup() { return m_waterShedID; }
	bool IsFlowEdge() { return m_flowEdge; }
	bool IsFlowEnd() { return m_flowEnd; }
	bool IsBridge() { return m_bridge; }
	WaterType GetWaterType() { return m_water; }

	void SetPos(glm::vec3 p) { m_pos = p; }
	void SetFlowGroup(int id) { m_waterShedID = id; }
	void SetAsBridge() { m_bridge = true; }
	void SetWaterType(WaterType w);
	void AddNormal(glm::vec3 n) { m_normal += n; }
	// Add an edge to the vertex
	// Returns false if edge was already added
	bool AddEdge(TerrainEdge* e);
	void AddFace(TerrainFace* f) { m_faces.push_back(f); }

	// Find the shape of the vertex
	void CalculateShape();
	// Find the vertices this vertex flows to
	// Returns false if this vertex does not flow
	bool CalculateFlow();
	void MakeFlowGroup(std::vector<TerrainVertex*> &visited, int id);
	bool CalculateFlowEdge();
	void MakeBridge();
	void FollowSteepUp(std::vector<TerrainVertex*> &visited, int id);

	void AddFlowSource(TerrainVertex* source);

private:
	// Calculate gradient of vertex normal
	void CalculateGradient();
	// Put edges in anti-clockwise order from x axis
	void OrderEdges();
	// Are edges order by rotation
	bool m_ordered;

	glm::vec3 m_pos;
	glm::vec3 m_normal;
	float m_gradient;
	std::vector<TerrainEdge*> m_edges;
	std::vector<TerrainFace*> m_faces;
	TerrainShape m_shape;
	std::vector<std::vector<TerrainEdge*>> m_groups;
	std::vector<TerrainEdge*> m_steepestUp;
	std::vector<TerrainEdge*> m_steepestDown;
	float m_simVal;
	TerrainVertex* m_flowTo;
	std::vector<TerrainVertex*> m_flowFrom;
	int m_waterShedID;
	bool m_flowEdge;
	bool m_flowEnd;
	bool m_bridge;
	WaterType m_water;
	bool m_graphEdge;
};

class TerrainEdge {
public:
	TerrainEdge();
	~TerrainEdge();

	EdgeType GetType() { return m_type; }
	float GetGradient(TerrainVertex* root);
	TerrainVertex* GetPoint(int p) { return m_points[p]; }
	TerrainVertex* GetOtherPoint(TerrainVertex* tv);
	// Return normalised edge direction starting from root
	glm::vec3 GetDirection(TerrainVertex* root);

	void SetPoints(TerrainVertex* v1, TerrainVertex* v2);
	void SetType(EdgeType t);
	void AddFace(TerrainFace* f) { m_faces.push_back(f); }

	// Return the angle in radians between a given vector and this edge (from the given root node)
	// Ignores the y axis
	float GetXZRotatation(TerrainVertex* root, glm::vec3 origin);

	void FlowDown(TerrainVertex* from);

private:
	// Calculate gradient between points
	void CalculateGradient();

	TerrainVertex* m_points[2]{ nullptr, nullptr };
	std::vector<TerrainFace*> m_faces;
	float m_gradient;
	EdgeType m_type;
	glm::vec3 m_normDir;
	glm::vec3 m_normDirFlat;
};

class TerrainFace {
public:
	TerrainFace();
	~TerrainFace();

	glm::vec3 GetNormal() { return m_normal; }

	void SetVerts(vector<TerrainVertex*> vs);
	void SetEdges(vector<TerrainEdge*> es);

	bool ContainsWaterfall(glm::vec3 origin);
	// Return edges attached to a vertex in anti-clockwise order
	std::vector<TerrainEdge*> AttachedEdges(TerrainVertex* pivot);
	// Return anti-clockwise edge.  Returns nullptr if given the anti-clockwise edge.
	TerrainEdge* GetNextEdge(TerrainEdge* first, TerrainVertex* pivot);
	// Return clockwise edge.  Returns nullptr if given the clockwise edge.
	TerrainEdge* GetPrevEdge(TerrainEdge* second, TerrainVertex* pivot);

private:
	float m_grad;
	std::vector<TerrainVertex*> m_verts;
	std::vector<TerrainEdge*> m_edges;
	glm::vec3 m_normal;
	// Maximum values of all vertex positions
	glm::vec3 m_max;
	// Minimum values of all vertex positions
	glm::vec3 m_min;
};

class TerrainWaterShed {
public:
	TerrainWaterShed();
	~TerrainWaterShed();

	// Get the ID of this watershed group
	int GetID() { return m_id; }
	// Set ID for this group and all stored vertices
	void SetID(int id);
	// Adds a list of vertices to this watershed group
	void AddMembers(std::vector<TerrainVertex*> newMembers);
	// Add a bridge into this group, from another group
	void AddBridge(TerrainVertex* in);
	// Find the bridges of this group. Returns the bridge vertices for the other groups to add.
	std::vector<TerrainVertex*> FindBridges();
	// Designate lakes
	void MakeLakes();

private:
	bool m_complete;
	int m_id;
	float m_exitHeight;
	std::vector<TerrainVertex*> m_bridges;
	std::vector<TerrainVertex*> m_edges;
	std::vector<TerrainVertex*> m_members;
};

class TerrainGraph {
public:
	TerrainGraph();
	~TerrainGraph();

	void SetPolyMesh(PolyMesh* pm) { m_pm = pm; }

	void CreateGraph();
	void AnalyseGraph();
	void ColourWaterGroup();
	void ColourWaterEdges();
	void ColourWaterBodies();
	void ColourShapeResults();
	void ColourGradients();

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

	std::vector<TerrainWaterShed*> m_watersheds;
};

#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "OBJLoader.h"
#include "PolyMesh.h"

enum TerrainShape { DEFAULT_V, PEAK, PIT, RIDGE_V, TROUGH_V, HILLTOP, HILLBASE, SLOPE, FLAT, SADDLE };
enum EdgeType { DEFAULT_E, RIDGE_E, TROUGH_E, WATERSHED };
enum WaterType { NONE, RIVER, LAKE };
enum RainfallType { RF_NONE, RF_IRRIGATED, RF_SWAMP, RF_RIVER, RF_FASTRIVER, RF_LAKE };
enum WaterShedTier { BASE, SUPER };

class TerrainEdge;
class TerrainFace;

class TerrainVertex {
public:
	static float GreatestWaterVal;
	static float IrrigationThreshold;
	static float RiverThreshold;
	static float RiverSpreadThreshold;
	static int ID;

	TerrainVertex();
	~TerrainVertex();

	glm::vec3 GetPos() { return m_pos; }
	glm::vec3 GetNormal() { return m_normal; }
	float GetGradient() { return m_gradient; }
	std::vector<TerrainEdge*> GetEdges() { return m_edges; }
	TerrainShape GetShape() { return m_shape; }
	TerrainVertex* GetFlowTo() { return m_flowTo; }
	int* GetFlowGroup(WaterShedTier t) { return (t == BASE ? &m_waterShedID : &m_superID); }
	bool IsFlowEdge(WaterShedTier t) { return (t == BASE ? m_flowEdge : m_superFlowEdge); }
	bool IsFlowEnd() { return m_flowEnd; }
	bool IsBridge() { return m_bridge; }
	TerrainVertex* GetBridgeEnd() { return m_bridgeTo; }
	WaterType GetWaterType() { return m_water; }
	bool IsGraphEdge() { return m_graphEdge; }
	float GetWaterVal() { return m_waterVal; }
	RainfallType GetRainType() { return m_rainType; }

	void SetPos(glm::vec3 p) { m_pos = p; }
	void SetFlowGroup(int id) { m_superID = m_waterShedID = id; }
	void SetSuperFlowGroup(int id) { m_superID = id; }
	void SetAsBridge() { m_bridge = true; }
	void AddBridgeSource(TerrainVertex* from) { m_bridgeSources.push_back(from); }
	void SetWaterType(WaterType w);
	void SetRainfallType(RainfallType r);
	void SetRegionBridge(TerrainVertex* b) { m_regionBridge = b; }
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
	float MakeFlowGroup(std::vector<TerrainVertex*> &visited, int id);
	bool CalculateFlowEdge(WaterShedTier t);
	void MakeBridge(TerrainVertex* bridgeEnd);
	void FollowSteepUp(std::vector<TerrainVertex*> &visited, int id);

	void AddFlowSource(TerrainVertex* source);
	void AddWater(float water);
	void AddFlowingWater(float water);

private:
	// Calculate gradient of vertex normal
	void CalculateGradient();
	// Put edges in anti-clockwise order from x axis
	void OrderEdges();

	void SetFlowEdge(WaterShedTier t, bool state);
	// Are edges order by rotation
	bool m_ordered;

	int m_id;
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
	int m_superID;
	bool m_flowEdge;
	bool m_superFlowEdge;
	bool m_flowEnd;
	bool m_bridge;
	std::vector<TerrainVertex*> m_bridgeSources;
	TerrainVertex* m_bridgeTo;
	TerrainVertex* m_regionBridge;
	WaterType m_water;
	bool m_graphEdge;
	float m_waterVal;
	float m_waterRemaining;
	std::vector<int> m_flatestEdges;
	std::vector<int> m_lowestGradients;
	RainfallType m_rainType;
};

class TerrainEdge {
public:
	TerrainEdge();
	~TerrainEdge();

	EdgeType GetType() { return m_type; }
	float GetGradient(TerrainVertex* root);
	float GetAbsGradient() { return m_gradient; }
	TerrainVertex* GetPoint(int p) { return m_points[p]; }
	TerrainVertex* GetOtherPoint(TerrainVertex* tv);
	// Return normalised edge direction starting from root
	glm::vec3 GetDirection(TerrainVertex* root);
	float GetLength() { return m_length; }
	float GetFlatLength() { return m_flatLength; }

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
	float m_length;
	float m_flatLength;
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
	// Split face area to vertices that make it
	void DistributeWater();

private:

	float m_grad;
	float m_area;
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
	// Get the volume of water leaving this area
	float GetWaterVal() { return m_lowestFlowless->GetWaterVal(); }
	// Number of other regions to bridge into this section
	std::vector<int*> GetOtherBridges() { return m_otherBridges; }
	// Set ID for this group and all stored vertices
	void SetID(int id);
	// Set the flowless vertex
	void SetFlowless(TerrainVertex* v) { m_lowestFlowless = v; }
	// Adds a list of vertices to this watershed group
	void AddMembers(std::vector<TerrainVertex*> newMembers);
	// Add a bridge into this group, from another group
	void AddBridge(TerrainVertex* in, int* idFrom);
	// Find the bridges of this group. Returns the bridge vertices for the other groups to add.
	std::vector<TerrainVertex*> FindBridges();
	// Get bridges from here to elsewhere
	std::vector<TerrainVertex*> GetBridges() { return m_thisBridges; }
	// Designate lakes
	void MakeLakes();
	// Designate lakes
	void MakeRainfallLakes();
	// True if the watershed leaves the graph region
	bool IsComplete() { return m_complete; }
	// Recieve overflow
	void SendBridgeWater(TerrainVertex* bridge, float water, int* idFrom);
	// Moves this regions details into the given region
	void MergeInto(TerrainWaterShed* ws);

private:
	bool m_complete;
	int m_id;
	float m_exitHeight;
	TerrainVertex* m_lowestFlowless;
	std::vector<TerrainVertex*> m_thisBridges;
	std::vector<int*> m_otherBridges;
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
	void ColourRainfallBodies();
	void ColourShapeResults();
	void ColourGradients();
	void ColourWaterVals();

private:
	void SendColours();

	PolyMesh* m_pm;
	std::vector<TerrainVertex*> m_verts;
	std::vector<TerrainEdge*> m_edges;
	std::vector<TerrainFace*> m_faces;
	std::vector<glm::vec4> m_uniqueColours;
	std::vector<glm::vec4> m_nonUniqueColours;

	TerrainEdge* FindExistingEdge(int first, int second);
	std::vector<std::pair<int, int>> m_prevEdgePairs;
	std::vector<TerrainEdge*> m_prevEdges;
	std::vector<TerrainVertex*> m_flowless;

	std::map<int, TerrainWaterShed*> m_watersheds;
	std::map<int, TerrainWaterShed*> m_superWatersheds;
};

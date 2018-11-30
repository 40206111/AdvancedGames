#define _USE_MATH_DEFINES

#include "TerrainGraph.h"
#include <math.h>
#include <algorithm>
#include <map>

using namespace std;

// HELP FUNCTIONS ---------------------------------------------------------------------------

bool SimilarGradients(float simVal, float grad1, float grad2) {
	if ((grad1 > simVal && grad2 > simVal)
		|| (grad1 < -simVal && grad2 < -simVal)
		|| (fabsf(grad1) <= simVal && fabsf(grad2) <= simVal)) {
		// Set similar to true
		return true;
	}
	else {
		return false;
	}
}

float CalculateGradientHelp(glm::vec3 diff) {
	// If diff is a zero vector
	if (diff == glm::vec3(0.0f)) {
		return 0;
	}
	// Calculate dot product
	float diffDot = glm::dot(glm::normalize(diff), glm::vec3(0.0f, 1.0f, 0.0f));
	// Get angle from dot product
	float diffAng = acosf(diffDot);
	// Make range from 1(up) to -1 (down)
	diffAng = 1 - (2 * diffAng / M_PI);
	return diffAng;
}

// TERRAIN VERTEX -------------------------------------------------------------------------

TerrainVertex::TerrainVertex() {
	m_ordered = false;
	m_pos = glm::vec3(0.0f);
	m_normal = glm::vec3(0.0f);
	m_shape = DEFAULT_V;
	m_simVal = 0.2f;
	m_flowTo = nullptr;
	m_waterShedID = -1;
	m_flowEdge = false;
	m_flowEnd = false;
	m_bridge = false;
	m_water = NONE;
}
TerrainVertex::~TerrainVertex() {}

void TerrainVertex::SetWaterType(WaterType w) {
	// If value of stored watertype is less than new one
	if (m_water < w) {
		// Set new watertype
		m_water = w;
	}
}

void TerrainVertex::CalculateShape() {
	// If the edges are not ordered already, order them
	if (!m_ordered) {
		OrderEdges();
	}
	CalculateGradient();
	// Gradients of edges
	vector<float> grads;
	for (int i = 0; i < m_edges.size(); ++i) {
		grads.push_back(m_edges[i]->GetGradient(this));
	}
	// If there are gradients
	if (grads.size() > 0) {
		// Add first group of edges
		m_groups.push_back(vector<TerrainEdge*>());
		// Add edge to first group of edges
		m_groups.back().push_back(m_edges[0]);
		// For each edge/gradient
		for (int i = 1; i < grads.size(); ++i) {
			// If gradients are similar
			if (SimilarGradients(m_simVal, grads[i], grads[i - 1])) {
				// Add to current group of edges
				m_groups.back().push_back(m_edges[i]);
			}
			else {
				// Add new group of edges
				m_groups.push_back(vector<TerrainEdge*>());
				// Add to new group of edges
				m_groups.back().push_back(m_edges[i]);
			}
		}
	}
	// If more than 2 groups exist
	if (m_groups.size() >= 3) {
		// Check if first and last group could wrap into each other
		if (SimilarGradients(m_simVal, m_groups.front().front()->GetGradient(this),
			m_groups.back().front()->GetGradient(this))) {
			// Add last list to first list
			for (TerrainEdge* te : m_groups.back()) {
				m_groups.front().push_back(te);
			}
			// Remove extra group
			m_groups.erase(m_groups.begin() + m_groups.size() - 1);
		}
	}
	// Find steepest gradients up/down
	// For each group of gradients
	for (vector<TerrainEdge*> &vte : m_groups) {
		// Variables to store information after loop
		float greatest = 0.0f;
		TerrainEdge* greatEdge;
		// For each edge in the group
		for (TerrainEdge* te : vte) {
			// If this edge is steeper than others
			if (fabsf(te->GetGradient(this)) > fabsf(greatest)) {
				// Save the new steepest gradient
				greatest = te->GetGradient(this);
				// Save the edge associated
				greatEdge = te;
			}
		}
		// If gradient is negative
		if (greatest < 0.0f) {
			// Add to steepest descending gradients
			m_steepestDown.push_back(greatEdge);
		}
		else {
			// Add to steepest ascending gradients
			m_steepestUp.push_back(greatEdge);
		}
	}
	// Find shape of terrain
	switch (m_groups.size())
	{
		// If only one item exists in simplify
	case(1):
		// Edges slope up from vertex
		if (m_groups[0].front()->GetGradient(this) > m_simVal) {
			m_shape = PIT;
		}
		// Edges slope down to vertex
		else if (m_groups[0].front()->GetGradient(this) < -m_simVal) {
			m_shape = PEAK;
		}
		// Edges do not experience large change around vertex
		else {
			m_shape = FLAT;
		}
		break;
		// If two distinct edge groups
	case(2):
	{
		// Stores gradient of a non-flat group if a flat group exists
		float nonFlat = 0.0f;
		// For both groups
		for (int i = 0; i < 2; ++i) {
			// If this pair is a flat group
			if (fabsf(m_groups[i].front()->GetGradient(this)) <= m_simVal) {
				// Store other gradient in nonFlat
				nonFlat = m_groups[(i + 1) % 2].front()->GetGradient(this);
				// Exit for loop
				break;
			}
		}
		// If steep group slopes up
		if (nonFlat > m_simVal) {
			m_shape = HILLBASE;
		}
		// If steep group sloped down
		else if (nonFlat < -m_simVal) {
			m_shape = HILLTOP;
		}
		// If no flat group exists
		else {
			m_shape = SLOPE;
		}
		break;
	}
	// If all groups exist (-,=,+)
	case(3):
		m_shape = SLOPE;
		break;
	case(4):
	{
		// Counting variables
		int countPos = 0;
		int countNeg = 0;
		int countEq = 0;
		// Go through groups and increment count variables
		for (vector<TerrainEdge*> vte : m_groups) {
			// Use first gradient in group as reference
			float grad = vte.front()->GetGradient(this);
			// If slopes up, add positive
			if (grad > m_simVal) {
				countPos++;
			}
			// If slopes down, add negative
			else if (grad < -m_simVal) {
				countNeg++;
			}
			// Else, add flat
			else {
				countEq++;
			}
		}

		// If opposing + groups
		if (countPos == 2.0f) {
			// If opposing - groups
			if (countNeg == 2.0f) {
				m_shape = SADDLE;
			}
			// If opposing - and = group
			else {
				m_shape = TROUGH_V;
			}
		}
		// If opposing - groups (saddle taken care of above)
		else if (countNeg == 2.0f) {
			m_shape = RIDGE_V;
		}
		// If opposing = groups
		else {
			m_shape = SLOPE;
		}
		break;
	}
	// If more than 4 groups (or no groups)
	default:
		m_shape = SADDLE;
		break;
	}
}

bool TerrainVertex::CalculateFlow() {
	/*if (m_steepestDown.size() == 0) {
		return false;
	}*/
	float grad = 0 + 0.00001f;
	// Find steepest downwards gradient
	for (TerrainEdge* te : m_edges) {
		if (te->GetGradient(this) < grad) {
			grad = (te->GetGradient(this));
		}
	}
	vector<TerrainEdge*> tes;
	// Find edges with gradients matching saved gradient
	for (TerrainEdge* te : m_edges) {
		if (te->GetGradient(this) <= grad + 0.00001f) {
			tes.push_back(te);
		}
	}
	// If no edges were found
	if (tes.size() == 0) {
		// Return false for no-flowing
		return false;
	}
	// Output boolean, false = no-flow, true = flows
	bool outBool = false;
	// For each downward edge
	for (TerrainEdge* te : tes) {
		// If the other point doesn't flow into this point (avoid cyclical flow on flat surfaces)
		if (find(m_flowFrom.begin(), m_flowFrom.end(), te->GetOtherPoint(this)) == m_flowFrom.end()) {
			// Tell other vertex that this vertex flows into them
			te->FlowDown(this);
			// Set outbool to true to say that this vertex flows
			outBool = true;
			// Save target vertex for later
			m_flowTo = te->GetOtherPoint(this);
		}
	}
	return outBool;
}

void TerrainVertex::OrderEdges() {
	vector<float> angles;
	vector<int> indices;
	// For each edge get the angle from x axis to it
	for (int i = 0; i < m_edges.size(); ++i) {
		// Add to angle vector
		angles.push_back((m_edges[i]->GetXZRotatation(this, glm::vec3(1.0f, 0.0f, 0.0f))));
		// Add to index vector
		indices.push_back(i);
	}
	bool changed = true;
	int maxI = angles.size() - 1;
	// While changes are made to the list
	while (changed) {
		// Prepare to break loop
		changed = false;
		// From the end of the list to the start
		for (int i = maxI; i > 0; --i) {
			// If later value is smaller than earlier value, swap values
			if (angles[indices[i]] < angles[indices[i - 1]]) {
				// Swap index values
				int tempi = indices[i];
				indices[i] = indices[i - 1];
				indices[i - 1] = tempi;
				// Stay in while loop, changes may still be needed
				changed = true;
			}
		}
	}
	// Loop through ordered indices to rearrange edges
	vector<TerrainEdge*> newEdges;
	for (int i : indices) {
		newEdges.push_back(m_edges[i]);
	}
	// Assign new ordered edge list
	m_edges = newEdges;
	// Set ordered boolean
	m_ordered = true;
}

void TerrainVertex::CalculateGradient() {
	// If normal is non-zero
	if (m_normal != glm::vec3(0.0f)) {
		// Normalise the normal
		m_normal = glm::normalize(m_normal);
	}
	// Assign value of gradient (not absolute)
	m_gradient = CalculateGradientHelp(m_normal);
}

void TerrainVertex::MakeFlowGroup(vector<TerrainVertex*> &visited, int id) {
	if (m_steepestDown.size() == 0 && this->m_waterShedID != -1) {
		return;
	}
	// If already part of the group being made, return
	if (id == m_waterShedID) {
		return;
	}
	// If this vertex in in its own list of things that flow to it ??????????
	// Should maybe be visited.back instead of this ?????
	if (find(m_flowFrom.begin(), m_flowFrom.end(), this) != m_flowFrom.end()) {
		return;
	}
	// If nothing in visited list, this is the end of the flow
	if (visited.size() == 0) {
		m_flowEnd = true;
	}
	else {
		m_flowEnd = false;
	}
	// Add this vertex to visited list
	visited.push_back(this);
	// If not part of any group
	if (m_waterShedID == -1) {
		// Set id to current group id
		m_waterShedID = id;
	}
	// If already part of a group
	else {
		// Set id to -2 (meaning in multiple groups)
		m_waterShedID = -2;
	}
	// For each vertex that flows into this
	for (TerrainVertex* v : m_flowFrom) {
		// Perform this function on that vertex
		v->MakeFlowGroup(visited, id);
	}
}

bool TerrainVertex::CalculateFlowEdge() {
	// Initialise flow edge to false
	m_flowEdge = false;
	// For each edge
	for (TerrainEdge* te : m_edges) {
		// If the edge leads to a vertex in a different watershed-group
		if (te->GetOtherPoint(this)->GetFlowGroup() != m_waterShedID) {
			// Set this vertex as a border of its group
			m_flowEdge = true;
			// Exit function (further testing unneccessary)
			break;
		}
	}
	return m_flowEdge;
}

void TerrainVertex::MakeBridge() {
	// Edges that lead to other flow groups
	vector<TerrainEdge*> others;
	// For each edge of this vertex
	for (TerrainEdge* te : m_edges) {
		// If in a different flow group
		if (te->GetOtherPoint(this)->GetFlowGroup() != m_waterShedID) {
			// Add to list of others
			others.push_back(te);
		}
	}
	// Tracks lowest gradient
	float grad = 1.1f;
	// Edge to bridge to
	TerrainEdge* bridge = nullptr;
	// For each edge not in flow group
	for (TerrainEdge* te : others) {
		// If new lowest gradient is found
		if (te->GetGradient(this) < grad) {
			grad = te->GetGradient(this);
			bridge = te;
		}
	}
	// If a bridge vertex was found
	if (bridge != nullptr) {
		// Swap group id's
		/*int myID = m_waterShedID;
		m_waterShedID = bridge->GetOtherPoint(this)->GetFlowGroup();
		bridge->GetOtherPoint(this)->SetFlowGroup(myID);*/
		bridge->GetOtherPoint(this)->SetAsBridge();
		SetAsBridge();
	}
}

void TerrainVertex::FollowSteepUp(vector<TerrainVertex*> &visited, int id) {
	if (m_steepestUp.size() == 0 && this->m_waterShedID != -1) {
		return;
	}
	// If already part of this group return
	if (id == m_waterShedID) {
		return;
	}
	// Add this vertex to visited list
	visited.push_back(this);
	// If not part of any group
	if (m_waterShedID == -1) {
		// Make this vertex id the given id
		m_waterShedID = id;
	}
	// Else if already in a group
	else {
		// Make id -2 (for multiple groups)
		m_waterShedID = -2;
	}
	// Record of highest angle
	float steepest = 0.0f;
	// For each edge
	for (TerrainEdge* te : m_edges) {
		// Get edge gradient
		float grad = te->GetGradient(this);
		// If edge-gradient in steeper than previously recorded
		if (grad > steepest) {
			// Replace old steepest value with new steepest
			steepest = grad;
		}
	}
	// For each edge
	for (TerrainEdge* te : m_edges) {
		// If edge gradient is equal, or very close to steepest
		if (te->GetGradient(this) >= steepest - 0.00001) {
			// Get point at other end of the edge
			TerrainVertex* goTo = te->GetOtherPoint(this);
			// If vertex is not in visited list
			if (find(visited.begin(), visited.end(), goTo) == visited.end()) {
				// Perform this function on other vertex
				goTo->FollowSteepUp(visited, id);
			}
		}
	}
}

void TerrainVertex::AddFlowSource(TerrainVertex* source) {
	m_flowFrom.push_back(source);
}

bool TerrainVertex::AddEdge(TerrainEdge* edge) {
	// If this edge has not been added to this vertex before
	if (find(m_edges.begin(), m_edges.end(), edge) == m_edges.end()) {
		m_edges.push_back(edge);
		// Return true for successful add
		return true;
	}
	else {
		// Return false for edge not added
		return false;
	}
}

// TERRAIN EDGE ----------------------------------

TerrainEdge::TerrainEdge() {
	m_gradient = 0.0f;
	m_type = DEFAULT_E;
}
TerrainEdge::~TerrainEdge() {}

float TerrainEdge::GetGradient(TerrainVertex* root) {
	// Y value of other vertex
	float testY = 0.0f;
	// If index 0 is this vertex
	if (root == m_points[0]) {
		// Save index 1 y value
		testY = m_points[1]->GetPos().y;
	}
	// If index 1 is this vertex
	else {
		// Save index 0 y value
		testY = m_points[0]->GetPos().y;
	}
	// If this position is higher than other position, line slopes down
	if (root->GetPos().y > testY) {
		// Return negative gradient
		return -m_gradient;
	}
	// Return positive gradient
	return m_gradient;
}

TerrainVertex* TerrainEdge::GetOtherPoint(TerrainVertex* te) {
	if (m_points[0] != te) {
		return m_points[0];
	}
	else {
		return m_points[1];
	}
}

void TerrainEdge::SetPoints(TerrainVertex* v1, TerrainVertex* v2) {
	// Assign array values
	m_points[0] = v1;
	m_points[1] = v2;
	// Find normalized vector from 0 to 1
	m_normDir = glm::normalize(v2->GetPos() - v1->GetPos());
	// Find normalized projection on the y axis
	glm::vec3 flat = m_normDir;
	// If not already a y projection
	if (flat.y != 0.0f) {
		// Make y value 0
		flat.y = 0.0f;
		// Length of projected vector
		float flatLen2 = glm::dot(flat, flat);
		// If not a zero vector, normalise it
		if (flatLen2 != 0.0f) {
			flat = glm::normalize(flat);
		}
		// Else set to zero vector
		else {
			flat = glm::vec3(0.0f);
		}
	}
	// Assign normalized y projection
	m_normDirFlat = flat;
	// Calculate gradient of edge
	CalculateGradient();
}

void TerrainEdge::SetType(EdgeType t) {
	if (t > m_type) {
		m_type = t;
	}
}

float TerrainEdge::GetXZRotatation(TerrainVertex* root, glm::vec3 origin) {
	// Make sure origin vector is flat
	if (origin.y != 0.0f) {
		origin.y = 0.0f;
	}
	// Make sure origin vector is normalised
	float oLength = glm::dot(origin, origin);
	if (oLength != 1.0f) {
		origin = glm::normalize(origin);
	}
	// Get smallest angle between
	float cosAngle = 0.0f;
	cosAngle = glm::dot(origin, m_normDir);
	float radAngle = acosf(cosAngle);
	// Cross product of vectors
	glm::vec3 cross = glm::cross(origin, m_normDir);
	// If normdir clockwise of origin find larger angle
	if (cross.y < 0) {
		radAngle = (M_PI * 2.0f) - radAngle;
	}
	// If root is not the first vertex in array flip angle 
	if (root != m_points[0]) {
		radAngle += M_PI;
		if (radAngle > M_PI * 2.0f) {
			radAngle -= M_PI * 2.0f;
		}
	}
	// Return final value
	return radAngle;
}

void TerrainEdge::CalculateGradient() {
	// Vector between points
	glm::vec3 diff = m_points[1]->GetPos() - m_points[0]->GetPos();
	// Assign abolute value of gradient
	m_gradient = fabsf(CalculateGradientHelp(diff));
}

void TerrainEdge::FlowDown(TerrainVertex* source) {
	// If first point is not the source
	if (m_points[0] != source) {
		// Add the source
		m_points[0]->AddFlowSource(source);
	}
	else {
		// If first point was the source, add flow to the second point
		m_points[1]->AddFlowSource(source);
	}
}

// TERRAIN FACE ------------------------------------------------------------------

TerrainFace::TerrainFace() {}
TerrainFace::~TerrainFace() {}

void TerrainFace::SetVerts(vector<TerrainVertex*> vs) { 
	// Save vertex vector to face
	m_verts = vs;
	// Initialise max and min as coords of first vertex
	m_max = m_min = m_verts.front()->GetPos();
	// Ignoring the first vertex, go through face's vertices
	for (int v = 1; v < m_verts.size(); ++v) {
		// For each of x, y and z
		for (int i = 0; i < 3; ++i) {
			// If this vertex has a lower axis position than the minimum
			if (m_verts[v]->GetPos()[i] < m_min[i]) {
				// Set new minimum on this axis
				m_min[i] = m_verts[v]->GetPos()[i];
			}
			// If this vertex has a higher axis position than the maximum
			else if (m_verts[v]->GetPos()[i] > m_max[i]) {
				// Set new maximum on this axis
				m_max[i] = m_verts[v]->GetPos()[i];
			}
		}
	}
}

// TERRAIN WATERSHED ------------------------------------------------------------------

TerrainWaterShed::TerrainWaterShed() {
	m_id = -1;
	m_complete = false;
}

TerrainWaterShed::~TerrainWaterShed() {}

void TerrainWaterShed::SetID(int id) {
	// Set own ID
	m_id = id;
	// Set ID of the vertices
	for (TerrainVertex* v : m_members) {
		v->SetFlowGroup(id);
	}
}

void TerrainWaterShed::AddMembers(vector<TerrainVertex*> newMembers) {
	// If this group has no members
	if (m_members.size() == 0) {
		// Set this groups id to match vertex id
		m_id = newMembers.front()->GetFlowGroup();
	}
	// For each new vertex
	for (TerrainVertex* v : newMembers) {
		// Update vertex's id
		v->SetFlowGroup(m_id);
		// Add to members
		m_members.push_back(v);
	}
	// Re-evaluate current edges
	for (int e = 0; e < m_edges.size(); ++e) {
		// If edge vertex at e is no longer an edge
		if (!(m_edges[e]->CalculateFlowEdge())) {
			// Remove vertex from edge list
			m_edges.erase(m_edges.begin() + e);
			// Decrement e to avoid skipping vertices
			e--;
		}
	}
	// Find new watershed edges
	// For each member
	for (TerrainVertex* v : m_members) {
		// If v finds itself on an edge
		if (v->CalculateFlowEdge()) {
			// Add v to edges list
			m_edges.push_back(v);
		}
	}
}

void TerrainWaterShed::AddBridge(TerrainVertex* in) {
	// Vertex is already a bridge
	bool isHere = false;
	// For each bridge
	for (TerrainVertex* v : m_bridges) {
		// If the in vertex is already a bridge
		if (in == v) {
			isHere = true;
			break;
		}
	}
	// If in is not here already
	if (!isHere) {
		// Add in to bridges
		m_bridges.push_back(in);
		// Tell vertex it's a bridge
		in->SetAsBridge();
	}
	// Vertex to flow to
	TerrainVertex* next = in;
	// While there's vertices to flow to
	while (next != nullptr) {
		// If this vertex is already a river or lake stop flowing
		if (next->GetWaterType() >= RIVER) {
			break;
		}
		// Set vertex as a river
		next->SetWaterType(RIVER);
		// Set next as the next vertex in the steepest path
		next = next->GetFlowTo();
	}
}

std::vector<TerrainVertex*> TerrainWaterShed::FindBridges() {
	// Empty list of bridges
	m_bridges.clear();
	// Return list of bridges in other groups
	vector<TerrainVertex*> lowOthers;
	// If no edges exist
	if (m_edges.size() == 0) {
		return lowOthers;
	}
	// Lowest height found
	float lowest;
	// Lowest vertex found
	TerrainVertex* lowV = nullptr;
	// No bridge details are saved yet
	bool firstBridge = true;
	// For each vertex in edges
	for (TerrainVertex* v : m_edges) {
		// If edge vertex is lower than other vertices
		if (firstBridge || v->GetPos().y < lowest) {
			// Get edges v has
			vector<TerrainEdge*> vEdges = v->GetEdges();
			// Tracking for the other vertex
			float otherLowest;
			TerrainVertex* otherLowV;
			// The first vertex to be found from a different group
			bool firstOther = true;
			// For each edge
			for (TerrainEdge* e : vEdges) {
				// Other vertex
				TerrainVertex* vOther = e->GetOtherPoint(v);
				// If other vertex is in a different group
				if (vOther->GetFlowGroup() != v->GetFlowGroup()) {
					// If other variables are uninitialised
					if (firstOther) {
						otherLowest = vOther->GetPos().y;
						otherLowV = vOther;
					}
					else {
						// If the other vertex is also lower than lowest
						if (vOther->GetPos().y < otherLowest) {
							otherLowest = vOther->GetPos().y;
							otherLowV = vOther;
						}
					}
				}
			}
			// If the edge vertex, and its potential bridges, are both lower than last lowest vertex
			if (firstBridge || (v->GetPos().y < lowest && otherLowest < lowest)) {
				// Replace lowest info with the heighest of either this vertex, or its lowest bridge vertex
				lowest = (v->GetPos().y > otherLowest ? v->GetPos().y : otherLowest);
				// Save this side of bridge
				lowV = v;
				// Save other side of bridge
				lowOthers.clear();
				lowOthers.push_back(otherLowV);
				// Cancel first bridge
				firstBridge = false;
			}
		}
	}
	// If a vertex has been found
	if (lowV != nullptr) {
		// Save bridge details
		lowV->SetAsBridge();
		m_bridges.push_back(lowV);
		m_exitHeight = lowest;
	}
	// Return bridge vertices in other watersheds
	return lowOthers;
}

void TerrainWaterShed::MakeLakes() {
	// For each memeber vertex
	for (TerrainVertex* v : m_members) {
		// If below the height of water exit
		if (v->GetPos().y <= m_exitHeight) {
			v->SetWaterType(LAKE);
		}
	}
}

// TERRAIN GRAPH ------------------------------------------------------------------

TerrainGraph::TerrainGraph() {}
TerrainGraph::~TerrainGraph() {}

TerrainEdge* TerrainGraph::FindExistingEdge(int first, int second) {
	// Search through un-paired edges
	for (int e = 0; e < m_prevEdgePairs.size(); ++e) {
		// If pair matches in current order
		if ((m_prevEdgePairs[e].first == first && m_prevEdgePairs[e].second == second)
			// Or if pair matches in reverse order
			|| (m_prevEdgePairs[e].first == second && m_prevEdgePairs[e].second == first)) {
			// Remove pair from pair list
			m_prevEdgePairs.erase(m_prevEdgePairs.begin() + e);
			// Copy pointer to terrainEdge
			TerrainEdge* te = m_prevEdges[e];
			// Remove edge from list
			m_prevEdges.erase(m_prevEdges.begin() + e);
			// Return edge
			return te;
		}
	}
	// If nothing found return nullptr
	return nullptr;
}

void TerrainGraph::CreateGraph() {
	const OBJModel& obj = m_pm->getOBJModel();
	// For each vertex of the indexed model create a vertex
	for (int v = 0; v < obj.vertices.size(); ++v) {
		// New terrain vertex
		TerrainVertex* tv = new TerrainVertex();
		// Set position from indexed model positions
		tv->SetPos(obj.vertices[v]);
		// Set normal from indexed model normals
		//tv->SetNormal(obj.normals[v]);
		// Add terrainVertex to graph list of vertices
		m_verts.push_back(tv);
	}
	// For each vertex, add its normal
	for (int n = 0; n < obj.OBJIndices.size(); ++n) {
		// Local copy of index object for clearer array indexing
		OBJIndex indexSet = obj.OBJIndices[n];
		// Set normal for a given vertex using OBJIndex details
		m_verts[indexSet.vertexIndex]->AddNormal(obj.normals[indexSet.normalIndex]);
	}
	// For each vertex index in the indexed model 
	// Increment by three to step in triangles
	for (int e = 0; e < obj.OBJIndices.size(); e += 3) {
		// For each of the three indexes in a triangle
		for (int i = 0; i < 3; ++i) {
			// pair of indices (either: 0/1, 1/2, 2/0)
			int index1 = obj.OBJIndices[e + i].vertexIndex;
			int index2 = obj.OBJIndices[(e + ((i + 1) % 3))].vertexIndex;
			// New terrainEdge
			TerrainEdge* te = FindExistingEdge(index1, index2);
			// If no details for edge exist
			if (te == nullptr) {
				// Initialise terrain edge
				te = new TerrainEdge();
				// Set edge points to vertices at indices
				te->SetPoints(m_verts[index1], m_verts[index2]);
				// Add edge to list
				m_edges.push_back(te);
				// Add pair of vertex indices to list
				m_prevEdgePairs.push_back(pair<int, int>(index1, index2));
				// Add pointer to list
				m_prevEdges.push_back(te);
			}
			// Add edge to vertices (if it hasn't been added before)
			if (m_verts[index1]->AddEdge(te)) {
				m_verts[index2]->AddEdge(te);
			}
			/*	m_verts[index1]->AddEdge(te);
				m_verts[index2]->AddEdge(te);*/
		}
	}
}

void TerrainGraph::AnalyseGraph() {
	for (TerrainVertex* v : m_verts) {
		v->CalculateShape();
	}
	for (TerrainVertex* v : m_verts) {
		// If a vertex does not flow to another vertex add to flowless list
		if (!v->CalculateFlow()) {
			m_flowless.push_back(v);
		}
	}
	// Watershed ID
	int i = 0;
	for (TerrainVertex* v : m_flowless) {
		// Vertices in the flow group
		vector<TerrainVertex*> visited;
		// Recursively find vertices in group
		v->MakeFlowGroup(visited, i++); // ID increments after function call
		// Make new terrain watershed
		TerrainWaterShed* tws = new TerrainWaterShed();
		// Add vertices to it
		tws->AddMembers(visited);
		// Add watershed to list
		m_watersheds.push_back(tws);
	}
	// For each watershed group
	for (TerrainWaterShed* ws : m_watersheds) {
		// Find bridges
		vector<TerrainVertex*> bridges = ws->FindBridges();
		// For each vertex that is a bridge from this watershed
		for (TerrainVertex* v : bridges) {
			// Find watershed group it is part of 
			for (TerrainWaterShed* ws : m_watersheds) {
				if (ws->GetID() == v->GetFlowGroup()) {
					// Add it as a bridge to the group
					ws->AddBridge(v);
				}
			}
		}
		// Create lakes
		ws->MakeLakes();
	}
}

void TerrainGraph::ColourWaterGroup() {
	m_uniqueColours.clear();
	m_nonUniqueColours.clear();
	int max = m_flowless.size();
	if (m_flowless.size() == 0) {
		max = 2;
	}
	static int lim = 0;
	// For each vertex
	for (TerrainVertex* v : m_verts) {
		// ID of the group the vertex is part of
		int flowGroup = v->GetFlowGroup();
		switch (flowGroup)
		{
			// Multiple flow groups
		case(-2):
			m_uniqueColours.push_back(glm::vec4(0.9f, 0.9f, 0.9f, 1.0f)); // White-ish
			break;
			// No flow groups
		case(-1):
			m_uniqueColours.push_back(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)); // Grey
			break;
		default:
			// Colour to be added to buffer
			glm::vec4 colour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			// If this group should not be rendered
			if (flowGroup > lim) {
				// Colour dark grey
				colour = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
			}
			else {
				// For each value: red, green, and blue
				for (int i = 0; i < 3; ++i) {
					float c;
					// Get value from sin wave, displaced by group id and r/g/b index
					c = sin(((float)i / 3.0f + (float)flowGroup / (float)max) * 2.0f * M_PI) * 0.25 + 0.75;
					// Set specific value
					colour[i] = c;
				}
				// If end of the flow
				if (v->IsFlowEnd()) {
					// Get inverse of colour
					glm::vec4 inv = glm::vec4(1.0f) - colour;
					// Make colour its inverse
					colour = inv;
					// Maximise alpha
					colour.a = 1.0f;
				}
				// If group bridge
				else if (v->IsBridge()) {
					// Make full white
					colour = glm::vec4(1.0f);
				}
				// If in a lake
				else if (v->GetWaterType() == LAKE) {
					// Colour darker by 40%
					colour = colour * 0.6f;
					// Maximise alpha
					colour.a = 1.0f;
				}
				// If vertex is a river
				else if (v->GetWaterType() == RIVER) {
					// Invert colour
					colour = glm::vec4(1.0f) - colour;
					// Increase intensity
					colour += glm::vec4(0.5f);
					// Max alpha
					colour.a = 1.0f;
				}
				// Else if edge vertex
				else if (v->IsFlowEdge()) {
					// Colour darker by 20%
					colour = colour * 0.8f;
					// Maximise alpha
					colour.a = 1.0f;
				}
			}
			// Add colour to list
			m_uniqueColours.push_back(colour);
			break;
		}
	}
	// Make non-indexed colour list for buffer
	vector<OBJIndex> indices = m_pm->getOBJModel().OBJIndices;
	for (int c = 0; c < indices.size(); ++c) {
		m_nonUniqueColours.push_back(m_uniqueColours[indices[c].vertexIndex]);
	}
	// Add non-indexed colours to buffer
	m_pm->addColourBuffer(m_nonUniqueColours);
	++lim;
	lim = lim % max;
}

void TerrainGraph::ColourWaterEdges() {
	m_uniqueColours.clear();
	m_nonUniqueColours.clear();
	// For all vertices
	for (TerrainVertex* v : m_verts) {
		// If flow end point, colour dark purple
		if (v->IsFlowEnd()) {
			m_uniqueColours.push_back(glm::vec4(0.4f, 0.0f, 0.4f, 1.0f));
		}
		// If vertex is bridge, colour full white
		else if (v->IsBridge()) {
			m_uniqueColours.push_back(glm::vec4(1.0f));
		}
		// If edge colour orange
		else if (v->IsFlowEdge()) {
			m_uniqueColours.push_back(glm::vec4(0.7f, 0.2f, 0.1f, 1.0f));
		}
		// Colour other vertices grey
		else {
			m_uniqueColours.push_back(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
		}
	}
	// Make non-indexed colour list for buffer
	vector<OBJIndex> indices = m_pm->getOBJModel().OBJIndices;
	for (int c = 0; c < indices.size(); ++c) {
		m_nonUniqueColours.push_back(m_uniqueColours[indices[c].vertexIndex]);
	}
	// Add non-indexed colours to buffer
	m_pm->addColourBuffer(m_nonUniqueColours);
}

void TerrainGraph::ColourWaterBodies() {
	m_uniqueColours.clear();
	m_nonUniqueColours.clear();
	// For all vertices
	for (TerrainVertex* v : m_verts) {
		switch (v->GetWaterType()) {
		case(LAKE):
			m_uniqueColours.push_back(glm::vec4(0.3f, 0.3f, 0.6f, 1.0f)); // Dark blue
			break;
		case(RIVER):
			m_uniqueColours.push_back(glm::vec4(0.5f, 0.5f, 1.0f, 1.0f)); // Light blue
			break;
		default:
			m_uniqueColours.push_back(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f)); // Grey
			break;
		}
	}
	// Make non-indexed colour list for buffer
	vector<OBJIndex> indices = m_pm->getOBJModel().OBJIndices;
	for (int c = 0; c < indices.size(); ++c) {
		m_nonUniqueColours.push_back(m_uniqueColours[indices[c].vertexIndex]);
	}
	// Add non-indexed colours to buffer
	m_pm->addColourBuffer(m_nonUniqueColours);
}

void TerrainGraph::ColourShapeResults() {
	m_uniqueColours.clear();
	m_nonUniqueColours.clear();
	static int lim = 0;
	// For all vertices
	for (TerrainVertex* v : m_verts) {
		// Vertex shape
		TerrainShape test = v->GetShape();
		// If type matches limit value
	/*	if (!(test == HILLTOP || test == FLAT)) {
			test = DEFAULT_V;
		}*/
		if (test > lim/* || test < lim*/) {
			test = DEFAULT_V;
		}
		// Switch on vertex type
		switch (test) {
		case(PEAK):
			m_uniqueColours.push_back(glm::vec4(0.7, 0.1, 0.1, 1.0)); // Red
			break;
		case(PIT):
			m_uniqueColours.push_back(glm::vec4(0.1, 0.1, 0.7, 1.0)); // Blue
			break;
		case(RIDGE_V):
			m_uniqueColours.push_back(glm::vec4(0.7, 0.4, 0.1, 1.0)); // Greener red than peak (orange basically)
			break;
		case(TROUGH_V):
			m_uniqueColours.push_back(glm::vec4(0.1, 0.4, 0.7, 1.0)); // Greener blue than pit
			break;
		case(HILLTOP):
			m_uniqueColours.push_back(glm::vec4(0.7, 0.1, 0.4, 1.0)); // Red purple
			break;
		case(HILLBASE):
			m_uniqueColours.push_back(glm::vec4(0.4, 0.1, 0.7, 1.0)); // Blue purple
			break;
		case(SLOPE):
			m_uniqueColours.push_back(glm::vec4(0.5, 0.5, 0.5, 1.0)); // Grey
			break;
		case(FLAT):
			m_uniqueColours.push_back(glm::vec4(1.0, 1.0, 0.4, 1.0)); // Dark Yellow
			break;
		case(SADDLE):
			m_uniqueColours.push_back(glm::vec4(0.1, 0.7, 0.1, 1.0)); // Green
			break;
		default:
			m_uniqueColours.push_back(glm::vec4(1.0, 1.0, 1.0, 1.0)); // White
			break;
		}
	}
	// Make non-indexed colour list
	vector<OBJIndex> indices = m_pm->getOBJModel().OBJIndices;
	for (int c = 0; c < indices.size(); ++c) {
		m_nonUniqueColours.push_back(m_uniqueColours[indices[c].vertexIndex]);
	}
	// Add non-indexed colour list to buffer
	m_pm->addColourBuffer(m_nonUniqueColours);
	// Increment and loop limit
	lim++;
	lim = lim % (SADDLE + 1);
}

void TerrainGraph::ColourGradients() {
	m_uniqueColours.clear();
	m_nonUniqueColours.clear();
	// For each vertex
	for (TerrainVertex* v : m_verts) {
		// Add grey with value of vertex gradient
		m_uniqueColours.push_back(glm::vec4(glm::vec3(v->GetGradient()), 1.0f));
	}
	// Make non-indexed colour list
	vector<OBJIndex> indices = m_pm->getOBJModel().OBJIndices;
	for (int c = 0; c < indices.size(); ++c) {
		m_nonUniqueColours.push_back(m_uniqueColours[indices[c].vertexIndex]);
	}
	// Add non-indexed colour list to buffer
	m_pm->addColourBuffer(m_nonUniqueColours);
}

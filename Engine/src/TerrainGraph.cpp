#define _USE_MATH_DEFINES

#include "TerrainGraph.h"
#include <math.h>

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

TerrainVertex::TerrainVertex() {}
TerrainVertex::~TerrainVertex() {}

void TerrainVertex::CalculateShape() {
	// If the edges are not ordered already, order them
	if (!m_ordered) {
		OrderEdges();
	}
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
			if (SimilarGradients(simVal, grads[i], grads[i - 1])) {
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
		if (SimilarGradients(simVal, m_groups.front().front()->GetGradient(this),
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
		if (m_groups[0].front()->GetGradient(this) > simVal) {
			m_shape = PIT;
		}
		// Edges slope down to vertex
		else if (m_groups[0].front()->GetGradient(this) < -simVal) {
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
			if (fabsf(m_groups[i].front()->GetGradient(this)) <= simVal) {
				// Store other gradient in nonFlat
				nonFlat = m_groups[(i + 1) % 2].front()->GetGradient(this);
				// Exit for loop
				break;
			}
		}
		// If steep group slopes up
		if (nonFlat > simVal) {
			m_shape = HILLBASE;
		}
		// If steep group sloped down
		else if (nonFlat < -simVal) {
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
			float grad = vte.front()->GetGradient(this);
			if (grad > simVal) {
				countPos++;
			}
			else if (grad < -simVal) {
				countNeg++;
			}
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
	// Assign value of gradient (not absolute)
	m_gradient = CalculateGradientHelp(m_normal);
}

// TERRAIN EDGE ----------------------------------

TerrainEdge::TerrainEdge() {
	m_gradient = 0.0f;
	m_type = DEFAULT;
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
		m_verts[indexSet.vertexIndex]->SetNormal(obj.normals[indexSet.normalIndex]);
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
			// Add edge to vertices
			m_verts[index1]->AddEdge(te);
			m_verts[index2]->AddEdge(te);
		}
	}
}

void TerrainGraph::AnalyseGraph() {
	for (TerrainVertex* v : m_verts) {
		v->CalculateShape();
	}
}

void TerrainGraph::ColourResults() {
	m_uniqueColours.clear();
	m_nonUniqueColours.clear();
	static int lim = 0;
	for (TerrainVertex* v : m_verts) {
		TerrainShape test = v->GetShape();
		if (test > lim || test < lim) {
			test = FLAT;
		}
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
			m_uniqueColours.push_back(glm::vec4(0.5, 0.1, 0.5, 1.0)); // Balanced purple
			break;
		case(FLAT):
			m_uniqueColours.push_back(glm::vec4(0.6, 0.6, 0.6, 1.0)); // Light grey
			break;
		case(SADDLE):
			m_uniqueColours.push_back(glm::vec4(0.1, 0.7, 0.1, 1.0)); // Green
			break;
		default:
			m_uniqueColours.push_back(glm::vec4(1.0, 1.0, 1.0, 1.0)); // White
			break;
		}
	}
	vector<OBJIndex> indices = m_pm->getOBJModel().OBJIndices;
	for (int c = 0; c < indices.size(); ++c) {
		m_nonUniqueColours.push_back(m_uniqueColours[indices[c].vertexIndex]);
	}
	m_pm->addColourBuffer(m_nonUniqueColours);
	lim++;
	lim = lim % 9;
}
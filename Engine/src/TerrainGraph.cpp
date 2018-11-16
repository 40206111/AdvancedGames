#define _USE_MATH_DEFINES

#include "TerrainGraph.h"
#include <math.h>

using namespace std;

// HELP FUNCTIONS ---------------------------------------------------------------------------

bool SimilarGradients(float grad1, float grad2) {
	if ((grad1 > 1 && grad2 > 1)
		|| (grad1 < -1 && grad2 < -1)
		|| (fabsf(grad1) <= 1 && fabsf(grad2) <= 1)) {
		// Set similar to true
		return true;
	}
	else {
		return false;
	}
}

float CalculateGradientHelp(glm::vec3 diff) {
	// Difference in height
	float v = diff.y;
	// Squared difference in horizontal change
	float h2 = diff.x * diff.x + diff.z * diff.z;
	// Difference in horizontal change (initialised to very small value to avoid dividing by 0)
	float h = 0.0001f;
	// If horizontal difference is not 0 assign actual difference
	if (h2 > 0.0f) {
		h = sqrtf(h2);
	}
	// Calculate absolute value of gradient
	return (v / h);
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
	vector<pair<int, float>> simplify;
	if (grads.size() > 0) {
		simplify.push_back(pair<int, float>(1, grads[0]));
		for (int i = 1; i < grads.size(); ++i) {
			// If gradients are similar
			if (SimilarGradients(grads[i], simplify.back().second)) {
				// Get total from count * average
				float total = simplify.back().first * simplify.back().second;
				// Add new value to total
				total += grads[i];
				// Make pair with new count, and (new total/new count)
				pair<int, float> newPair = pair<int, float>(simplify.back().first + 1,
					total / ((float)simplify.back().first + 1));
				// Change last pair in simplify
				simplify.back() = newPair;
			}
			else {
				// Make a new simplify entry
				simplify.push_back(pair<int, float>(1, grads[i]));
			}
		}
	}
	// If more than 2 groups exist
	if (simplify.size() >= 3) {
		// Check if first and last group could wrap into each other
		if (SimilarGradients(simplify.front().second, simplify.back().second)) {
			// Local copies of pairs
			pair<int, float> p1 = simplify.front();
			pair<int, float> p2 = simplify.back();
			// New count for pair
			int newCount = p1.first + p2.first;
			// New total for pairs
			float newTotal = p1.second * (float)p1.first + p2.second * (float)p2.first;
			// New pair
			pair<int, float> newP = pair<int, float>(newCount, newTotal / ((float)newCount));
			// Replace first pair with joined pair
			simplify.front() = newP;
			// Remove last pair from list
			simplify.pop_back();
		}
	}

	switch (simplify.size())
	{
		// If only one item exists in simplify
	case(1):
		// Edges slope up from vertex
		if (simplify[0].second > 1.0f) {
			m_shape = PIT;
		}
		// Edges slope down to vertex
		else if (simplify[0].second < -1.0f) {
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
			if (fabsf(simplify[i].second) <= 1.0f) {
				// Store other gradient in nonFlat
				nonFlat = simplify[(i + 1) % 2].second;
				// Exit for loop
				break;
			}
		}
		// If steep group slopes up
		if (nonFlat > 1.0f) {
			m_shape = HILLBASE;
		}
		// If steep group sloped down
		else if (nonFlat < -1.0f) {
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
		// Go through groups and increment count groups
		for (pair<int, float> p : simplify) {
			if (p.second > 1.0f) {
				countPos++;
			}
			else if (p.second < -1.0f) {
				countNeg--;
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
				// Swap angle values
				/*float tempf = angles[i];
				angles[i] = angles[i - 1];
				angles[i - 1] = tempf;*/
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

void TerrainGraph::CreateGraph() {
	const OBJModel& obj = m_pm->getOBJModel();
	// For each vertex of the indexed model create a vertex
	for (int v = 0; v < obj.vertices.size(); ++v) {
		// New terrain vertex
		TerrainVertex* tv = new TerrainVertex();
		// Set position from indexed model positions
		tv->SetPos(obj.vertices[v]);
		// Set normal from indexed model normals
		tv->SetNormal(obj.normals[v]);
		// Add terrainVertex to graph list of vertices
		m_verts.push_back(tv);
	}
	// For each vertex index in the indexed model 
	// Increment by three to step in triangles
	for (int e = 0; e < obj.OBJIndices.size(); e += 3) {
		// For each of the three indexes in a triangle
		for (int i = 0; i < 3; ++i) {
			// New terrainEdge
			TerrainEdge* te = new TerrainEdge();
			// pair of indices (either: 0/1, 1/2, 2/0)
			int index1 = obj.OBJIndices[e + i].vertexIndex;
			int index2 = obj.OBJIndices[(e + ((i + 1) % 3))].vertexIndex;
			// Set edge points to vertices at indices
			te->SetPoints(m_verts[index1], m_verts[index2]);
			// Add endge to list
			m_edges.push_back(te);
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
	for (TerrainVertex* v : m_verts) {
		switch (v->GetShape()) {
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
}
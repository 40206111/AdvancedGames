#pragma once

#include "Geometry.h"
#include "PolyMesh.h"
#include "Material.h"

class Model {
public:

	// constructor
	Model(Geometry *geo, Material mat) : m_geometry(geo), m_material(mat) {}

	// accessors
	Geometry* getGeometry() const { return m_geometry; }
	Material getMaterial() const { return m_material; }

private:
	Geometry *m_geometry;
	Material m_material;

};
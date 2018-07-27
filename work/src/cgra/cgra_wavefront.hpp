
#pragma once

// std
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// project
#include "cgra_mesh.hpp"


namespace cgra {

	inline mesh_data load_wavefront_mesh_data(const std::string &filename) {

		// struct for storing wavefront index data
		struct wavefront_vertex {
			unsigned int p = 0, n = 0, t = 0;
		};

		// create reading buffers
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<wavefront_vertex> wv_vertices;

		// push back default values
		positions.push_back({ 0, 0, 0 });
		normals.push_back({ 0, 0, 1 });
		uvs.push_back({ 0, 0 });

		// open file
		std::ifstream objFile(filename);
		if (!objFile.is_open()) {
			std::cerr << "Error: could not open " << filename << std::endl;
			throw std::runtime_error("Error: could not open file.");
		}

		// good() means that failbit, badbit and eofbit are all not set
		while (objFile.good()) {

			// Pull out line from file
			std::string line;
			std::getline(objFile, line);
			std::istringstream objLine(line);

			// Pull out mode from line
			std::string mode;
			objLine >> mode;

			// Reading like this means whitespace at the start of the line is fine
			// attempting to read from an empty string/line will set the failbit
			if (!objLine.fail()) {


				if (mode == "v") {
					glm::vec3 v;
					objLine >> v.x >> v.y >> v.z;
					positions.push_back(v);
				}
				else if (mode == "vn") {
					glm::vec3 vn;
					objLine >> vn.x >> vn.y >> vn.z;
					normals.push_back(vn);

				}
				else if (mode == "vt") {
					glm::vec2 vt;
					objLine >> vt.x >> vt.y;
					uvs.push_back(vt);

				}
				else if (mode == "f") {

					std::vector<wavefront_vertex> face;
					while (objLine.good()) {
						wavefront_vertex v;

						// scan in position index
						objLine >> v.p; 
						// look ahead for a match
						if (objLine.peek() == '/') {	
							// ignore the '/' character
							objLine.ignore(1);			

							// scan in uv (texture coord) index (if it's there)
							if (objLine.peek() != '/') {
								objLine >> v.t;
							}

							// scan in normal index (if it's there)
							if (objLine.peek() == '/') {
								objLine.ignore(1);
								objLine >> v.n;
							}
						}

						face.push_back(v);
					}

					// IFF we have 3 vertices or more, construct a triangle
					if (face.size() >= 3) {
						for (int i = 0; i < 3; ++i) {
							wv_vertices.push_back(face[i]);
						}
					} 
				}
			}
		}

		// if we don't have any normals, create them naively
		if (normals.size() <= 1) {
			// Create the normals as 3d vectors of 0
			for (size_t i = 1; i < positions.size(); i++) {
				normals.push_back(glm::vec3());
			}

			// add the normal for every face to each vertex-normal
			for (size_t i = 0; i < wv_vertices.size()/3; i++) {
				wavefront_vertex &a = wv_vertices[i*3];
				wavefront_vertex &b = wv_vertices[i*3+1];
				wavefront_vertex &c = wv_vertices[i*3+2];

				// set the normal index to be the same as position index
				a.n = a.p;
				b.n = b.p;
				c.n = c.p;

				// calculate the face normal
				glm::vec3 ab = positions[b.p] - positions[a.p];
				glm::vec3 ac = positions[c.p] - positions[a.p];
				glm::vec3 face_norm = cross(ab, ac);

				// contribute the face norm to each vertex
				float l = length(face_norm);
				if (l > 0) {
					face_norm / l;
					normals[a.n] += face_norm;
					normals[b.n] += face_norm;
					normals[c.n] += face_norm;
				}
			}

			// normalize the normals
			for (size_t i = 0; i < normals.size(); i++) {
				normals[i] = normalize(normals[i]);
			}
		}
		
		// create mesh data
		std::vector<vertex_data> vertices;
		std::vector<unsigned int> indices;

		for (unsigned int i = 0; i < wv_vertices.size(); ++i) {
			indices.push_back(i);
			vertices.emplace_back(
				positions[wv_vertices[i].p],
				normals[wv_vertices[i].n],
				uvs[wv_vertices[i].t]
			);
		}

		return mesh_data(vertices, indices, GL_TRIANGLES);
	}
}
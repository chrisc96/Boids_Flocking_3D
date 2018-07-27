
// std
#include <stdexcept>

// project
#include "cgra_mesh.hpp"


namespace cgra {

	void mesh::draw(bool wireframe) {
		// set wireframe or fill polygon mode
		glPolygonMode(GL_FRONT_AND_BACK, (wireframe) ? GL_LINE : GL_FILL);
		// bind our VAO which sets up all our buffers and data for us
		glBindVertexArray(m_vao);
		// tell opengl to draw our VAO using the draw mode and how many vertices to render
		glDrawElements(m_mode, m_index_count, GL_UNSIGNED_INT, 0); // with indices
	}

	void mesh::destroy() {
		// delete the data buffers
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_vbo);
		glDeleteBuffers(1, &m_ibo);
	}


	mesh_data::mesh_data(
		const std::vector<vertex_data> &vertices,
		const std::vector<unsigned int> &indices,
		GLenum mode
	) :
		m_vertices(vertices),
		m_indices(indices),
		m_mode(mode)
	{ }


	mesh mesh_data::upload_mesh(mesh m) {

		// Create the buffers if they don't exist
		// VAO stores information about how the VBOs are set up
		if (!m.m_vao) glGenVertexArrays(1, &m.m_vao);
		// VBO(s) stores the vertices
		if (!m.m_vbo) glGenBuffers(1, &m.m_vbo);
		// IBO stores the indices that make up primitives
		if (!m.m_ibo) glGenBuffers(1, &m.m_ibo);

		// Compile the vertex data into a single vector
		int vertex_length = 3 + 3 + 2; // pos, norm, uv (8 floats)
		std::vector<float> vertex_data(m_vertices.size() * vertex_length);
		for (size_t i = 0; i < m_vertices.size(); ++i) {
			// positions
			vertex_data[(i*vertex_length) + 0] = m_vertices[i].pos[0];
			vertex_data[(i*vertex_length) + 1] = m_vertices[i].pos[1];
			vertex_data[(i*vertex_length) + 2] = m_vertices[i].pos[2];
			// normals
			vertex_data[(i*vertex_length) + 3] = m_vertices[i].norm[0];
			vertex_data[(i*vertex_length) + 4] = m_vertices[i].norm[1];
			vertex_data[(i*vertex_length) + 5] = m_vertices[i].norm[2];
			// uvs
			vertex_data[(i*vertex_length) + 6] = m_vertices[i].uv[0];
			vertex_data[(i*vertex_length) + 7] = m_vertices[i].uv[1];
		}


		// VAO
		//
		glBindVertexArray(m.m_vao);

		
		// VBO (single buffer, interleaved)
		//
		glBindBuffer(GL_ARRAY_BUFFER, m.m_vbo);
		// Upload ALL the data giving it the size (in bytes) and a pointer to the data
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_data.size(), &vertex_data[0], GL_STATIC_DRAW);

		// This buffer will use location=0 when we use our VAO
		glEnableVertexAttribArray(0);
		// Tell opengl how to treat data in location=0
		// the data is treated in lots of 3 (3 floats = vec3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_length, (void*)(0));

		// Do the same thing for Normals but bind it to location=1
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_length, (void*)(sizeof(float) * 3));

		// Do the same thing for UVs but bind it to location=2
		// Also, we are setting up an array for lots of 2 floats (vec2) instead of 3 floats (vec3)
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_length, (void*)(sizeof(float) * 6));


		// IBO
		//
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indices.size(), &m_indices[0], GL_STATIC_DRAW);


		// Set the index count and draw modes
		m.m_index_count = int(m_indices.size());
		m.m_mode = m_mode;

		// Clean up by binding 0, good practice
		// the GL_ELEMENT_ARRAY_BUFFER binding sticks to the VAO so we shouldn't unbind it
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return m;
	}
}
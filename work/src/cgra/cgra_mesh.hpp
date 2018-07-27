
#pragma once

// std
#include <vector>

// glm
#include <glm.hpp>

// project
#include <opengl.hpp>

namespace cgra {

	// A data structure for holding buffer IDs 
	// and other information related to drawing
	// also has helper functions for drawing, and
	// deleting the buffers (for cleanup)
	struct mesh {
		// GL buffer ids
		GLuint m_vao = 0;
		GLuint m_vbo = 0;
		GLuint m_ibo = 0;

		// index count (how much to draw)
		int m_index_count;

		// mode to draw in
		GLenum m_mode = 0;

		void draw(bool wireframe = false);
		void destroy();
	};


	struct vertex_data {
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 uv;
		vertex_data(glm::vec3 p = {}, glm::vec3 n = {}, glm::vec2 t = {})
			: pos(p), norm(n), uv(t){ }
		vertex_data(glm::vec3 p, glm::vec2 t)
			: pos(p), norm(0, 0, 1), uv(t) { }
	};

	class mesh_data {
		public:
			std::vector<vertex_data> m_vertices;
			std::vector<unsigned int> m_indices;
			GLenum m_mode;

			mesh_data(
				const std::vector<vertex_data> &vertices = {},
				const std::vector<unsigned int> &indices = {},
				GLenum mode = GL_TRIANGLES
			);

			mesh upload_mesh(mesh m = {});
	};

}


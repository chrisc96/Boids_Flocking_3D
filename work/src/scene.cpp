
// std
#include <random>

// stb
#include <stb_image.h>

// imgui
#include <imgui.h>

// glm
#include <gtc/matrix_transform.hpp>
#include <gtc/random.hpp>
#include <gtc/type_ptr.hpp>

// project
#include "scene.hpp"
#include "boid.hpp"
#include "cgra/cgra_wavefront.hpp"


Scene::Scene() {

	// load meshes
	cgra::mesh_data simple_boid_md = cgra::load_wavefront_mesh_data("../work/res/models/boid.obj");
	m_simple_boid_mesh = simple_boid_md.upload_mesh(m_simple_boid_mesh);

	cgra::mesh_data boid_md = cgra::load_wavefront_mesh_data("../work/res/models/spaceship_boid.obj");
	m_boid_mesh = boid_md.upload_mesh(m_boid_mesh);

	cgra::mesh_data predator_md = cgra::load_wavefront_mesh_data("../work/res/models/predator_boid.obj");
	m_predator_mesh = predator_md.upload_mesh(m_predator_mesh);

	cgra::mesh_data sphere_md = cgra::load_wavefront_mesh_data("../work/res/models/sphere.obj");
	m_sphere_mesh = sphere_md.upload_mesh(m_sphere_mesh);

	// load color shader
	cgra::shader_program color_sp;
	color_sp.set_shader(GL_VERTEX_SHADER, "../work/res/shaders/simple_color.glsl");
	color_sp.set_shader(GL_FRAGMENT_SHADER, "../work/res/shaders/simple_color.glsl");
	m_color_shader = color_sp.upload_shader();

	// load aabb shader
	cgra::shader_program aabb_sp;
	aabb_sp.set_shader(GL_VERTEX_SHADER, "../work/res/shaders/aabb.glsl");
	aabb_sp.set_shader(GL_GEOMETRY_SHADER, "../work/res/shaders/aabb.glsl");
	aabb_sp.set_shader(GL_FRAGMENT_SHADER, "../work/res/shaders/aabb.glsl");
	m_aabb_shader = aabb_sp.upload_shader();

	// load axis shader
	cgra::shader_program axis_prog;
	axis_prog.set_shader(GL_VERTEX_SHADER, "../work/res/shaders/axis.glsl");
	axis_prog.set_shader(GL_GEOMETRY_SHADER, "../work/res/shaders/axis.glsl");
	axis_prog.set_shader(GL_FRAGMENT_SHADER, "../work/res/shaders/axis.glsl");
	m_axis_shader = axis_prog.upload_shader();

	// load skymap shader
	cgra::shader_program skymap_sp;
	skymap_sp.set_shader(GL_VERTEX_SHADER, "../work/res/shaders/skymap.glsl");
	skymap_sp.set_shader(GL_GEOMETRY_SHADER, "../work/res/shaders/skymap.glsl");
	skymap_sp.set_shader(GL_FRAGMENT_SHADER, "../work/res/shaders/skymap.glsl");
	m_skymap_shader = skymap_sp.upload_shader();
}


void Scene::loadCore() {
	//-------------------------------------------------------------
	// [Assignment 3] (Core) :
	// Initialize the scene with 100-300 boids in random locations
	// inside the current bound size.
	//-------------------------------------------------------------

	m_boids.clear();

	for (int i = 0; i < m_numBoids; i++) {
		// this creates a boid with a random location in [-1, 1]^3 and random velocity (magnitude = 1)
		m_boids.push_back(Boid(glm::linearRand(glm::vec3(-1), glm::vec3(1)), glm::sphericalRand(1.0), 0, glm::vec3(0, 1, 0), 0));
	}
}

void Scene::loadCompletion() {
	//-------------------------------------------------------------
	// [Assignment 3] (Completion) :
	// Initialize the scene with 2 different flocks of boids,
	// 75-150 in each flock, in random locations inside the current
	// bound size. Additionally include at least one Predator.
	//-------------------------------------------------------------

	m_boids.clear();

	for (int i = 0; i < (int) m_numBoids; i++) {
		m_boids.push_back(Boid(glm::linearRand(glm::vec3(-1), glm::vec3(1)), glm::sphericalRand(1.0), 0, glm::vec3(0, 1, 0), 0));
		if (i % 2 == 0) {
			m_boids.push_back(Boid(glm::linearRand(glm::vec3(-1), glm::vec3(1)), glm::sphericalRand(1.0), 0, glm::vec3(0, 1, 0), 0));
		}
		else {
			m_boids.push_back(Boid(glm::linearRand(glm::vec3(-1), glm::vec3(1)), glm::sphericalRand(1.0), 1, glm::vec3(0, 0, 1), 0));
		}
		// this creates a boid with a random location in [-1, 1]^3 and random velocity (magnitude = 1)
		
	}

	for (int i = 0; i < m_numPredators; i++) {
		m_boids.push_back(Boid(glm::linearRand(glm::vec3(-20), glm::vec3(-20)), glm::sphericalRand(1.0), -1, glm::vec3(1, 0, 0), 1));
	}
}


void Scene::loadChallenge() {
	//-------------------------------------------------------------
	// [Assignment 3] (Challenge) :
	// Initialize the scene with 100-300 boids in random locations
	// inside the current bound size. Additionally add at least
	// three spheres with a large radius inside the bounds.
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...

}


void Scene::update(float timestep) {
	for (Boid &b : m_boids) {
		b.calculateForces(this);
	}

	for (Boid &b : m_boids) {
		b.update(timestep, this);
	}
}


void Scene::draw(const glm::mat4 &proj, const glm::mat4 &view) {

	// draw skymap (magically)
	//
	if (m_show_skymap) {
		static GLuint tex = 0;
		if (!tex) {
			int w, h, n;
			stbi_set_flip_vertically_on_load(true);
			unsigned char *img = stbi_load("../work/res/textures/sky.jpg", &w, &h, &n, 3);
			if (!img) throw std::runtime_error("Failed to load image ");
			else {
				glGenTextures(1, &tex);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
				glGenerateMipmap(GL_TEXTURE_2D);
				stbi_image_free(img);
			}
		}
		glUseProgram(m_skymap_shader);
		glUniformMatrix4fv(glGetUniformLocation(m_skymap_shader, "uProjectionMatrix"), 1, false, glm::value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(m_skymap_shader, "uModelViewMatrix"), 1, false, glm::value_ptr(view));
		glUniform1f(glGetUniformLocation(m_skymap_shader, "uZDistance"), 1000.0f);
		glActiveTexture(GL_TEXTURE0); // Set the location for binding the texture
		glBindTexture(GL_TEXTURE_2D, tex); // Bind the texture
		glUniform1i(glGetUniformLocation(m_skymap_shader, "uSkyMap"), 0);  // Set our sampler (texture0) to use GL_TEXTURE0 as the source
		draw_dummy(12);
	}

	// draw axis (magically)
	//
	if (m_show_axis) {
		// load shader and variables
		glUseProgram(m_axis_shader);
		glUniformMatrix4fv(glGetUniformLocation(m_axis_shader, "uProjectionMatrix"), 1, false, glm::value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(m_axis_shader, "uModelViewMatrix"), 1, false, glm::value_ptr(view));
		glUniform1f(glGetUniformLocation(m_axis_shader, "uAxisLength"), 1000.0f);
		draw_dummy(6);
	}

	// draw the aabb (magically)
	//
	if (m_show_aabb) {
		glUseProgram(m_aabb_shader);
		glUniformMatrix4fv(glGetUniformLocation(m_aabb_shader, "uProjectionMatrix"), 1, false, glm::value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(m_aabb_shader, "uModelViewMatrix"), 1, false, glm::value_ptr(view));
		glUniform3fv(glGetUniformLocation(m_aabb_shader, "uColor"), 1, glm::value_ptr(glm::vec3(0.8, 0.8, 0.8)));
		glUniform3fv(glGetUniformLocation(m_aabb_shader, "uMax"), 1, glm::value_ptr(m_bound_hsize));
		glUniform3fv(glGetUniformLocation(m_aabb_shader, "uMin"), 1, glm::value_ptr(-m_bound_hsize));
		draw_dummy(12);
	}


	// draw boids
	//
	for (const Boid &b : m_boids) {

		// get the boid direction (default to z if no velocity)
		glm::vec3 dir = normalize(b.velocity());
		if (dir.x != dir.x) dir = glm::vec3(0, 0, 1);

		// calculate the model matrix
		glm::mat4 model(1);

		// rotate the model to point it in the direction of its velocity

		// pitch rotation
		if (dir.y != 0) {
			float angle = -asin(dir.y);
			model = glm::rotate(glm::mat4(1), angle, glm::vec3(1, 0, 0)) * model;
		}

		// yaw rotation
		if (dir.x != 0 || dir.z != 0) {
			float angle = atan2(dir.x, dir.z);
			model = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * model;
		}

		// translate the model to its worldspace position

		// translate by m_position
		model = glm::translate(glm::mat4(1), b.position()) * model;

		// calculate the modelview matrix
		glm::mat4 modelview = view * model;

		// load shader and variables
		glUseProgram(m_color_shader);
		glUniformMatrix4fv(glGetUniformLocation(m_color_shader, "uProjectionMatrix"), 1, false, glm::value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(m_color_shader, "uModelViewMatrix"), 1, false, glm::value_ptr(modelview));
		glUniform3fv(glGetUniformLocation(m_color_shader, "uColor"), 1, glm::value_ptr(b.getColor()));

		// draw
		m_simple_boid_mesh.draw();
	}
}


void Scene::renderGUI() {

	if (ImGui::Button("Core", ImVec2(80, 0))) { loadCore(); }
	ImGui::SameLine();
	if (ImGui::Button("Completion", ImVec2(80, 0))) { loadCompletion(); }
	ImGui::SameLine();
	if (ImGui::Button("Challenge", ImVec2(80, 0))) { loadChallenge(); }

	ImGui::Checkbox("Draw Bound", &m_show_aabb);
	ImGui::Checkbox("Draw Axis", &m_show_axis);
	ImGui::Checkbox("Draw Skybox", &m_show_skymap);

	//-------------------------------------------------------------
	// [Assignment 3] :
	// Add ImGui sliders for controlling boid parameters :
	// Core :
	// - boid min speed
	// - boid max speed
	// - boid max acceleration
	// - boid neighbourhood size
	// - boid cohesion weight
	// - boid alignment weight
	// - boid avoidance weight
	// - bounding mode (optional)
	// Completion :
	// - boid anti-predator weight
	// - predator min speed
	// - predator max speed
	// - predator max acceleration
	// - predator neighbourhood size
	// - predator avoidance weight
	// - predator chase weight
	//-------------------------------------------------------------
	
	ImGui::SliderFloat3("Bound hsize", glm::value_ptr(m_bound_hsize), 0, 100.0, "%.0f");

	// YOUR CODE GOES HERE
	// ...
	const char * bounding[] = { "Wrap", "Bounce", "Force Bounce (best)" };
	static int boundMethod = 2;
	if (ImGui::Combo("Boid Bounding Methods", &boundMethod, bounding, ((int)(sizeof(bounding) / sizeof(*bounding))))) {
		setBoundWrapping(boundMethod);
	}

	static float minVel = 9.0f;
	if (ImGui::SliderFloat("Min Velocity", &minVel, 1, 25, "%.0f")) {
		for (Boid &b : m_boids) {
			b.setBoidMinVel(minVel);
		}
	}

	static float maxVel = 20.0f;
	if (ImGui::SliderFloat("Max Velocity", &maxVel, 1, 50, "%.0f")) {
		for (Boid &b : m_boids) {
			b.setBoidMaxVel(maxVel);
		}
	}

	static float maxAccel = 28.0f;
	if (ImGui::SliderFloat("Max Acceleration", &maxAccel, 5, 100, "%.0f")) {
		for (Boid &b : m_boids) {
			b.setBoidMaxAccel(maxAccel);
		}
	}

	static float mass = 1.0f;
	if (ImGui::SliderFloat("Mass of Boids", &mass, 1, 5, "%.0f")) {
		for (Boid &b : m_boids) {
			b.setBoidMass(mass);
		}
	}


	static float cohesionDist = 0.5f;
	if (ImGui::DragFloat("Cohesion sight dist", &cohesionDist, 0.1, 0, 50)) {
		for (Boid &b : m_boids) {
			b.setCoherenceDist(cohesionDist);
		}
	}

	static float avoidDist = 1.0f;
	if (ImGui::DragFloat("Avoid sight dist", &avoidDist, 1, 01, 50)) {
		for (Boid &b : m_boids) {
			b.setAvoidDist(avoidDist);
		}
	}

	static float alignDist = 1.0f;
	if (ImGui::DragFloat("Align sight dist", &alignDist, 1, 0, 50)) {
		for (Boid &b : m_boids) {
			b.setAlignmentDist(alignDist);
		}
	}

	static float boidSeePredDist = 1.0f;
	if (ImGui::DragFloat("Distance boid can see predators", &boidSeePredDist, 1, 0, 50)) {
		for (Boid &b : m_boids) {
			b.setBoidSeePredDist(boidSeePredDist);
		}
	}


	static float cohesionWeight = 0.5f;
	if (ImGui::DragFloat("Cohesions with boids Weight", &cohesionWeight, 0.1, 0, 20)) {
		for (Boid &b : m_boids) {
			b.setCoherenceWeight(cohesionWeight);
		}
	}

	static float avoidWeight = 1.0f;
	if (ImGui::DragFloat("avoid boids Weight", &avoidWeight, 0.1, 0, 100)) {
		for (Boid &b : m_boids) {
			b.setAvoidWeight(avoidWeight);
		}
	}

	static float alignWeight = 1.0f;
	if (ImGui::DragFloat("Align with boids weight", &alignWeight, 0.1, 0, 50)) {
		for (Boid &b : m_boids) {
			b.setAlignmentWeight(alignWeight);
		}
	}

	static float boidSeePredWeight = 1.0f;
	if (ImGui::DragFloat("Evade predator Weight", &boidSeePredWeight, 0.1, 0, 40)) {
		for (Boid &b : m_boids) {
			b.setBoidSeePredWeight(boidSeePredWeight);
		}
	}









	static float p_mass = 2.0f;
	if (ImGui::SliderFloat("Mass of Predator Boids", &p_mass, 1, 20, "%.0f")) {
		for (Boid &b : m_boids) {
			b.setPredMass(p_mass);
		}
	}

	static float p_minVel = 1.0f;
	if (ImGui::SliderFloat("Min Predator Velocity", &p_minVel, 1, 20, "%.0f")) {
		for (Boid &b : m_boids) {
			b.setPredMinVel(p_minVel);
		}
	}

	static float p_maxVel = 18.0f;
	if (ImGui::SliderFloat("Max Predator Velocity", &p_maxVel, 1, 50, "%.0f")) {
		for (Boid &b : m_boids) {
			b.setPredMaxVel(p_maxVel);
		}
	}

	static float p_maxAccel = 125.0f;
	if (ImGui::SliderFloat("Max Predator Acceleration", &p_maxAccel, 100, 200, "%.0f")) {
		for (Boid &b : m_boids) {
			b.setPredMaxAccel(p_maxAccel);
		}
	}

	static float p_seekForce = 100.0f;
	if (ImGui::SliderFloat("Seek force of predator", &p_seekForce, 100, 500, "%.0f")) {
		for (Boid &b : m_boids) {
			b.setPredSeekF(p_seekForce);
		}
	}
}
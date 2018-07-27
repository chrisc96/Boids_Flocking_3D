
#pragma once

// glm
#include <glm.hpp>

// project
#include "scene.hpp"


class Boid {
private:

	// Physics
	glm::vec3		m_position;
	glm::vec3		m_velocity;
	glm::vec3		m_acceleration;
	
	// Generic Boid State Information
	glm::vec3 color			= glm::vec3(0, 1, 0);
	int flockID				= -1;	// 0 or 1 for completion (two flocks). -1 if it's a boid.
	int boidType			= 0;	// 0 - normal boid
									// 1 - predator boid

	// Normal Boids
	float b_m_mass			= 1.0f;
	float b_minVelocity		= 9.0f;
	float b_maxVelocity		= 28.0f;
	float b_maxAcceleration	= 70.0f;

	// Predator Boids
	float p_m_mass			= 2.0f;
	float p_minVelocity		= 1.0f;
	float p_maxVelocity		= 22.0f;
	float p_maxAcceleration = 125.0f;
	float predSeekForce		= 100.0f;

	// Predator seeking
	Boid *activeBoidToSeek = nullptr;
	int boidIndexInList;
	float hitTargetError = 1.1f; // Collision distance error check (to handle radius of boid)

	//
	float defaultAvoidDist	= 1.0f; // Gets reset to this
	bool expandingSight		= false;

	// Each behaviour has an individual distance parameter (and a weight too)
	float cohesionDist		= 1.0f;
	float avoidDist			= 1.0f;
	float alignmentDist		= 1.0f;
	float boidSeePredatorDist = 1.0f;

	// Weights
	float avoidWeight = 1.0f;
	float cohereWeight = 1.0f;
	float alignWeight = 1.0f;
	float evadeWeight = 1.0f;

public:
	Boid(glm::vec3 pos, glm::vec3 dir) : m_position(pos), m_velocity(dir) {
		m_acceleration = glm::vec3(0);
	}

	// For completion
	Boid(glm::vec3 pos, glm::vec3 dir, int id, glm::vec3 col, int type) : m_position(pos), m_velocity(dir), flockID(id), color(col), boidType(type) {
		m_acceleration = glm::vec3(0);
	}

	glm::vec3 position() const { return m_position; }
	glm::vec3 velocity() const { return m_velocity; }
	glm::vec3 acceleration() const { return m_acceleration; }

	// Normal Boid values
	float b_mass() const { return b_m_mass; }
	float b_minVel() { return b_minVelocity; }
	float b_maxVel() { return b_maxVelocity; }
	float b_maxAccel() { return b_maxAcceleration; }
	
	void setBoidMinVel(float vel) { b_minVelocity = vel; }
	void setBoidMaxVel(float vel) { b_maxVelocity = vel; }
	void setBoidMaxAccel(float a) { b_maxAcceleration = a; }
	void setBoidMass(float m) { b_m_mass = m; }

	// Predator values
	float p_mass() { return p_m_mass; }
	float p_minVel() { return p_minVelocity; }
	float p_maxVel() { return p_maxVelocity; }
	float p_maxAccel() { return p_maxAcceleration; }

	void setPredMinVel(float vel) { p_minVelocity = vel; }
	void setPredMaxVel(float vel) { p_maxVelocity = vel; }
	void setPredMaxAccel(float a) { p_maxAcceleration = a; }
	void setPredMass(float m) { p_m_mass = m; }
	void setPredSeekF(float s) { predSeekForce = s; }

	// Used by imgui
	void setAvoidDist(float d) { avoidDist = d; }
	void setCoherenceDist(float d) { cohesionDist = d; }
	void setAlignmentDist(float d) { alignmentDist = d; }
	void setBoidSeePredDist(float d) { boidSeePredatorDist = d; }

	void setAvoidWeight(float d) { avoidWeight = d; }
	void setCoherenceWeight(float d) { cohereWeight = d; }
	void setAlignmentWeight(float d) { alignWeight = d; }
	void setBoidSeePredWeight(float d) { evadeWeight = d; }

	glm::vec3 getColor() const { return color; }
	glm::vec3 setColor(glm::vec3 col) { color = col; }

	glm::vec3 evade(Scene *scene);
	glm::vec3 avoid(Scene *scene);
	glm::vec3 cohere(Scene *scene);
	glm::vec3 align(Scene *scene);
	glm::vec3 seek(glm::vec3 target);

	void calculateForces(Scene *scene);
	void update(float timestep, Scene *scene);
	void applyForceWithoutLimits(glm::vec3 force);
	void applyForce(glm::vec3 force);
	void wrapBorders(Scene *scene);
	void bounceBorders(Scene * scene);
	void forceBounceBorders(Scene * scene);
};
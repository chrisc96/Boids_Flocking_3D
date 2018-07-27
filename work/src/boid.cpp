#define GLM_ENABLE_EXPERIMENTAL

// glm
#include <gtc/random.hpp>
#include <gtx\transform.hpp>

// project
#include "boid.hpp"
#include "scene.hpp"
#include "cgra/cgra_mesh.hpp"
#include <iostream>

void Boid::calculateForces(Scene *scene) {

	// Boid flocking
	if (boidType == 0) {
		glm::vec3 avoidance = avoid(scene);	// Returns the avoidance force to apply
		glm::vec3 coherence = cohere(scene); // Returns the coherence force to apply
		glm::vec3 alignment = align(scene);	// Returns the alignment force to apply
		// glm::vec3 evadePreds = evade(scene);

		applyForce(avoidance * avoidWeight);
		applyForce(alignment * alignWeight);
		applyForce(coherence * cohereWeight);
		// applyForce(evadePreds * evadeWeight); Boid evasion destroys everything else... :L

	}
	else {
		if (activeBoidToSeek == nullptr) {
			float nearestBoid = 10000;
			for (int i = 0; i < scene->boids().size(); i++) {
				Boid &b = scene->boids().at(i);
				if (this == &b) continue; // Don't add ourselves -_-
				if (b.boidType == 1) continue; // Don't seek other predators

				if (glm::distance(b.m_position, m_position) < nearestBoid) {
					nearestBoid = glm::distance(m_position, b.m_position);
					boidIndexInList = i;
					activeBoidToSeek = &b;
				}
			}
		}
		else {
			glm::vec3 steering = seek(activeBoidToSeek->m_position);
			applyForce(steering * predSeekForce);
		}

		// If we've pretty much hit the boid, we need to set the null
		if (activeBoidToSeek != nullptr) {
			if (glm::distance(m_position, activeBoidToSeek->m_position) < hitTargetError) {
				// Remove boid from list of boids (also deallocate boid)
				activeBoidToSeek = nullptr;
				scene->boids().erase(scene->boids().begin() + boidIndexInList);
			}
		}
	}

	//-------------------------------------------------------------
	// [Assignment 3] :
	// Calculate the forces affecting the boid and update the
	// acceleration (assuming mass = 1).
	// Do NOT update velocity or position in this function.
	// Core : 
	//  - Cohesion
	//  - Alignment
	//  - Avoidance
	//  - Soft Bound (optional)
	// Completion : 
	//  - Cohesion and Alignment with only boids from the same flock
	//  - Predator Avoidance (boids only)
	//  - Predator Chase (predator only)
	// Challenge : 
	//  - Obstacle avoidance
	//-------------------------------------------------------------
}


void Boid::update(float timestep, Scene *scene) {

	switch (scene->wrappingType()) {
	case 0: // 0 = Wrap
		wrapBorders(scene);
		break;
	case 1: // 1 = Bounce
		bounceBorders(scene);
		break;
	case 2: // 2 = Force Bounce
		forceBounceBorders(scene);
		break;
	}

	//-------------------------------------------------------------
	// [Assignment 3] :
	// Integrate the velocity of the boid using the timestep.
	// Update the position of the boid using the new velocity.
	// Take into account the bounds of the scene which may
	// require you to change the velocity (if bouncing) or
	// change the position (if wrapping).
	//-------------------------------------------------------------
	glm::vec3 oldVel = m_velocity;
	m_velocity += m_acceleration * timestep;

	if (boidType == 0) {
		if (glm::length(m_velocity) < b_minVel()) {
			m_velocity = b_minVel() * glm::normalize(m_velocity);
		}
		if (glm::length(m_velocity) > b_maxVel()) {
			m_velocity = b_maxVel() * glm::normalize(m_velocity);
		}
	}
	else if (boidType == 1) {
		if (glm::length(m_velocity) < p_minVel()) {
			m_velocity = p_minVel() * glm::normalize(m_velocity);
		}
		if (glm::length(m_velocity) > b_maxVel()) {
			m_velocity = p_maxVel() * glm::normalize(m_velocity);
		}
	}

	// framerate independent correct calculation:
	// http://lolengine.net/blog/2011/12/14/understanding-motion-in-games
	m_position += ((oldVel + m_velocity) * 0.5f * timestep);

	m_acceleration *= 0;
}

glm::vec3 Boid::evade(Scene *scene) {
	glm::vec3 steer(0);
	for (Boid b : scene->boids()) {
		if (b.boidType == 1 && boidType == 0 && b.flockID == -1) { // Predator
			float distance = glm::distance(m_position, b.m_position);
			if (distance < boidSeePredatorDist) {
				glm::vec3 dif = m_position - b.m_position;
				glm::normalize(dif);
				dif /= distance;
				return seek(-dif);
			}
		}
	}
	return glm::vec3(0);
}

glm::vec3 Boid::avoid(Scene *scene) {
	float numBoids = 0;
	glm::vec3 steer(0);
	for (Boid b : scene->boids()) {
		// If within sight distance
		float distance = glm::distance(m_position, b.m_position);
		if (distance < avoidDist && distance != 0) {
			if (this == &b) continue; // if same obj, skip
			// We only want to avoid our own flock. We also don't want to avoid predators here (b.flockID = -1)
			// We will avoid predators in evadePredators method
			if (b.flockID != -1) {
				glm::vec3 dif = (m_position - b.m_position);
				glm::normalize(dif);
				dif /= distance;
				steer += dif;
				numBoids++;
			}
		}
	}

	// Average to avoid
	if (numBoids > 0) {
		steer /= numBoids;
	}

	if (glm::length(steer) != 0) {
		glm::normalize(steer);
		steer *= b_maxVel();
		steer -= m_velocity;

		if (glm::length(steer) > b_maxAccel()) {
			steer = b_maxAccel() * glm::normalize(steer);
		}
	}
	return steer;
}

glm::vec3 Boid::cohere(Scene *scene) {
	glm::vec3 sumsPos(0);
	float numBoids = 0;

	for (Boid b : scene->boids()) {
		// If within sight distance
		if (glm::distance(m_position, b.m_position) < cohesionDist) {
			if (this == &b) continue; // if same obj, skip
			if (flockID == b.flockID && flockID != -1) {
				numBoids++;
				sumsPos += b.m_position;
			}
		}
	}

	if (expandingSight && numBoids != 0) {
		// We've found some birdy friends, lets reset our sight to default,
		expandingSight = false;
		avoidDist = defaultAvoidDist;
	}
	if (numBoids > 0) return seek(sumsPos / numBoids);
	else {
		avoidDist = 50;
		expandingSight = true;
	}
	return glm::vec3(0);
}

glm::vec3 Boid::align(Scene *scene) {
	glm::vec3 sum(0);
	float numBoids = 0;
	
	for (Boid b : scene->boids()) {
		float distance = glm::distance(m_position, b.m_position);
		if (distance < alignmentDist && distance != 0) {
			if (this == &b) continue; // if same obj, skip
			if (flockID == b.flockID && flockID != -1) {
				sum += b.m_velocity;
				numBoids++;
			}
		}
	}

	if (numBoids > 0) {
		sum /= numBoids;
		glm::normalize(sum);
		sum *= b_maxVel();
		glm::vec3 steer = sum - m_velocity;

		if (glm::length(steer) > b_maxAccel()) {
			steer = b_maxAccel() * glm::normalize(steer);
		}
		return steer;
	}
	return sum;
}

// Functionality methods

glm::vec3 Boid::seek(glm::vec3 target) {
	glm::vec3 desired = target - m_position;
	glm::normalize(desired);
	if (boidType == 0) {
		desired *= b_maxVel();
	}
	else {
		desired *= p_maxVel();
	}

	glm::vec3 steer = desired - m_velocity;

	if (boidType == 0) {
		if (glm::length(steer) > b_maxAccel()) {
			steer = b_maxAccel() * glm::normalize(steer);
		}
	}
	else {
		if (glm::length(steer) > p_maxAccel()) {
			steer = p_maxAccel() * glm::normalize(steer);
		}
	}
	return steer;
}

void Boid::applyForce(glm::vec3 force) {
	if (boidType == 0) {
		if (glm::length(force) > b_maxAccel()) {
			force = b_maxAccel() * glm::normalize(force);

		}
		m_acceleration += (force / b_mass());
	}
	else {
		if (glm::length(force) > p_maxAccel()) {
			force = p_maxAccel() * glm::normalize(force);
		}
		m_acceleration += (force / p_mass());
	}
}

void Boid::applyForceWithoutLimits(glm::vec3 force) {
	if (boidType == 0) {
		m_acceleration += (force / b_mass());
	}
	else {
		m_acceleration += (force / p_mass());
	}
}


// Bounding methods

void Boid::wrapBorders(Scene *scene) {
	if (m_position.x < -scene->bound().x) m_position.x = scene->bound().x;
	if (m_position.x > scene->bound().x) m_position.x = -scene->bound().x;

	if (m_position.y < -scene->bound().y) m_position.y = scene->bound().y;
	if (m_position.y > scene->bound().y) m_position.y = -scene->bound().y;

	if (m_position.z < -scene->bound().z) m_position.z = scene->bound().z;
	if (m_position.z > scene->bound().z) m_position.z = -scene->bound().z;
}

void Boid::bounceBorders(Scene *scene) {
	if (m_position.x < -scene->bound().x || m_position.x > scene->bound().x) { m_velocity.x *= -1; }
	if (m_position.y < -scene->bound().y || m_position.y > scene->bound().y) { m_velocity.y *= -1; }
	if (m_position.z < -scene->bound().z || m_position.z > scene->bound().z) { m_velocity.z *= -1; }
}

void Boid::forceBounceBorders(Scene *scene) {
		glm::vec3 desired(0);

		if (m_position.x < -scene->bound().x) {
			if (boidType == 0) {
				desired = glm::vec3(b_maxVel(), m_velocity.y, m_velocity.z);
			}
			else {
				desired = glm::vec3(p_maxVel(), m_velocity.y, m_velocity.z);
			}
		}
		else if (m_position.x > scene->bound().x) {
			if (boidType == 0) {
				desired = glm::vec3(-b_maxVel(), m_velocity.y, m_velocity.z);
			}
			else {
				desired = glm::vec3(-p_maxVel(), m_velocity.y, m_velocity.z);
			}
		}

		if (m_position.y < -scene->bound().y) {
			if (boidType == 0) {
				desired = glm::vec3(m_velocity.x, b_maxVel(), m_velocity.z);
			}
			else {
				desired = glm::vec3(m_velocity.x, p_maxVel(), m_velocity.z);
			}
		}
		else if (m_position.y > scene->bound().y) {
			if (boidType == 0) {
				desired = glm::vec3(m_velocity.x, -b_maxVel(), m_velocity.z);
			}
			else {
				desired = glm::vec3(m_velocity.x, -p_maxVel(), m_velocity.z);
			}
		}

		if (m_position.z < -scene->bound().z) {
			if (boidType == 0) {
				desired = glm::vec3(m_velocity.x, m_velocity.y, b_maxVel());
			}
			else {
				desired = glm::vec3(m_velocity.x, m_velocity.y, p_maxVel());
			}
		}
		else if (m_position.z > scene->bound().z) {
			if (boidType == 0) {
				desired = glm::vec3(m_velocity.x, m_velocity.y, -b_maxVel());
			}
			else {
				desired = glm::vec3(m_velocity.x, m_velocity.y, -p_maxVel());
			}
		}

		if (glm::length(desired) != 0) {
			glm::normalize(desired);
			if (boidType == 0) {
				desired *= b_maxVel();
			}
			else {
				desired *= p_maxVel();
			}
			glm::vec3 steer = desired - m_velocity;

			applyForce(steer);
		}
}
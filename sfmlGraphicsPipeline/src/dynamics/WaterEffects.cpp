#include "./../../include/dynamics/WaterEffects.hpp"

WaterEffects::WaterEffects(const std::vector<ParticlePtr> particles, const glm::vec3 collisionPosition, const float splashVelocity, const float splashRadius) {
    m_particles = particles;
    m_collisionPosition = collisionPosition;
    m_splashVelocity = splashVelocity;
    m_splashRadius = splashRadius;
}

const std::vector<ParticlePtr> WaterEffects::getParticles() {
    return m_particles;
}

void WaterEffects::setParticles(const std::vector<ParticlePtr>& particles) {
    m_particles = particles;
}

void WaterEffects::do_addForce() {
    for (ParticlePtr p : m_particles) {
        if (!p->isFixed()) {
            glm::vec3 position = p->getPosition();
            // Vérifie si la particule est proche du point de collision
            float distance = glm::length(position - m_collisionPosition);
            if (distance < m_splashRadius) {
                // Calcule la direction du mouvement pour les particules comprises dans le rayon à effet
                // Somme pour accentuer le mouvement vers le haut
                glm::vec3 direction = glm::normalize(position - m_collisionPosition)+glm::vec3(0.0, 3.5, 0.0);
                // Ajuste la vitesse des particules en fonction de leur distance au point de collision
                float speedFactor = 1.0f - (distance / m_splashRadius);
                glm::vec3 velocity = 0.1f*m_splashVelocity * speedFactor * direction;
                // Met à jour la vitesse de la particule selon la vitesse courante
                p->incrVelocity(velocity);
            }
        }
    }
}
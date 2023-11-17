#ifndef WATER_EFFECTS_HPP
#define WATER_EFFECTS_HPP

#include <vector>
#include "ForceField.hpp"
#include "Particle.hpp"

class WaterEffects : public ForceField {

public:
    /*Build a "splash" force applied to a set of particles.
     * @param particles Set of particles influenced by this force.
     * @param collisionPosition the position where an object will dive providing the force.
     * @param splashVelocity the velocity of the splash effect.
     * @param splashRadius the radius reached by the effect.
    */
    WaterEffects(const std::vector<ParticlePtr> particles, const glm::vec3 collisionPosition, const float splashVelocity, const float splashRadius);
   /* Get the particles influenced by this damping force field.
    * @return The set of particles influenced by this.
   */
    const std::vector<ParticlePtr> getParticles();
    /*Define a new set of particles that will be influenced by this force .
     * @param particles The new set of influenced particles.
    */
    void setParticles(const std::vector<ParticlePtr>& particles);

private:
    void do_addForce();
    std::vector<ParticlePtr> m_particles;
    glm::vec3 m_collisionPosition;
    float m_splashVelocity;
    float m_splashRadius;
};

typedef std::shared_ptr<WaterEffects> WaterEffectsPtr;

#endif
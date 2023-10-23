#include "./../../include/dynamics/ParticlePlaneCollision.hpp"

ParticlePlaneCollision::~ParticlePlaneCollision()
{}


ParticlePlaneCollision::ParticlePlaneCollision(ParticlePtr particle, PlanePtr plane, float restitution) :
    Collision(restitution)
{
    m_particle = particle;
    m_plane = plane;
}

void ParticlePlaneCollision::do_solveCollision()
{
    //Don't process fixed particles (Let's assume that the ground plane is fixed)
    if (m_particle->isFixed()) return;

    //TODO: Solve ParticlePlane collisions, update particle position and velocity after collision
    //Functions to use:
    //glm::dot(v1, v2): Return the dot product of two vector.
    //Plane::distanceToOrigin(): Return the distance to origin from the plane
    //Plane::normal(): Return the normal of the plane
    //Particle::getRadius(), Particle::getPosition(), Particle::getVelocity(), Particle::setPosition(), Particle::setVelocity()

    //Compute particle-plane distance
    glm::vec3 n = m_plane -> normal();
    glm::vec3 p = n * m_plane -> distanceToOrigin();
    glm::vec3 q = m_particle -> getPosition();
    glm::vec3 u1 = m_particle->getVelocity();

    float d2plane = std::abs (glm::dot(q-p, n));

    //Project the particle on the plane
    m_particle -> incrPosition((m_particle->getRadius() - d2plane) * n);

    //Compute post-collision velocity
    m_particle ->incrVelocity(-(1+m_restitution) * glm::dot(u1,n) * n);

}



bool testParticlePlane(const ParticlePtr &particle, const PlanePtr &plane)
{
    /* Equation of a plane passing through A and normal to n:
   * dot( p - A, n ) = dot( p, n ) - dot( A, n ) = 0
   * dot( A, n ) is stored in the "distanceToOrigin" of the plane.
   *
   * Equation of a particle of radius r centered in c:
   * dot( p - c, p - c ) = r²
   *
   * distance( plane, particle )
   *   = min( distance( plane, c ) - r, 0 )              //definition
   *   = min( abs( dot( c - A, n ) ) - r, 0 )
   *   = min( abs( dot( c, n ) - dot( A, n ) ) - r, 0 )
   *
   * So, there is intersection if distance( plane, particle ) = 0
   * <=> abs( dot( c, n ) - dot( A, n ) ) - r <= 0
   * <=> abs( dot( c, n ) - dot( A, n ) ) <= r
   */

    //TODO: Test collision between particle and plane
    //Functions to use:
    //glm::dot(v1, v2): Return the dot product of two vector.
    //Plane::distanceToOrigin(): Return the distance to origin from the plane
    //Plane::normal(): Return the normal of the plane
    //Particle::getRadius(), Particle::getPosition()

    glm::vec3 n = plane -> normal();
    glm::vec3 q = particle -> getPosition();
    float r = particle -> getRadius();

    if(std::abs(glm::dot(q,n) - plane->distanceToOrigin())<=r){
        return true;
    }

    return false;
}

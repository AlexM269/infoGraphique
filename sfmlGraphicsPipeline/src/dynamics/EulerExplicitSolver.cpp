#include "./../../include/dynamics/EulerExplicitSolver.hpp"

EulerExplicitSolver::EulerExplicitSolver()
{

}

EulerExplicitSolver::~EulerExplicitSolver()
{

}

void EulerExplicitSolver::do_solve(const float& dt, std::vector<ParticlePtr>& particles)
{
    for(ParticlePtr p : particles)
    {
        if(!p->isFixed())
        {
            //TODO: Implement explicit euler solver
            //Functions to use:
            //Particle::getPosition(), Particle::getVelocity(), Particle::getMass(), Particle::getForce()
            //Particle::setPosition(), Particle::setVelocity()
           
            //get v(t)
            glm::vec3 positionT = p->getPosition();
            glm::vec3 velocityT = p->getVelocity();
            glm::vec3 acc = p->getForce() / p->getMass();

            // v(t+dt) = v(t) + dt * a(t) 
            glm::vec3 v_t_dt = velocityT + dt * acc ;
            p->setVelocity(v_t_dt);

            //p(t+dt) = p(t) + dt * v(t)
            p->setPosition(positionT + dt * v_t_dt);
        }
    }
}

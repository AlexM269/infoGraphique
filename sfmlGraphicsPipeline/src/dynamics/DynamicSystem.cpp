#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <random>

#include "./../../include/gl_helper.hpp"
#include "./../../include/dynamics/DynamicSystem.hpp"
#include "./../../include/dynamics/ParticlePlaneCollision.hpp"
#include "./../../include/dynamics/ParticleParticleCollision.hpp"
#include "./../../include/dynamics/ConstantForceField.hpp"



DynamicSystem::DynamicSystem() :
    m_dt(0.1),
    m_restitution(1.0),
    m_handleCollisions(true),
    m_elapsedTimeSinceWindUpdate(0.0) // Initialisez la variable à zéro
{}

glm::vec3 DynamicSystem::gravity = glm::vec3(0, -9.81, 0);
glm::vec3 DynamicSystem::wind = glm::vec3(-40,-9.81,-70);
glm::vec3 DynamicSystem::courant = glm::vec3(-5,0,-5);

void DynamicSystem::updateWind(float animationTime)
{
    m_elapsedTimeSinceWindUpdate += animationTime;

    // Si 5 secondes se sont écoulées, mettre à jour le vent et réinitialiser la minuterie
    if (m_elapsedTimeSinceWindUpdate >= 5.0)
    {
        // Générez des valeurs aléatoires selon une distribution normale pour chaque composant du vent.
        int randomXInt = rand();
        int randomZInt = rand();

        // Normalisez les valeurs à la plage [0, 20].
        float randomX = static_cast<float>(randomXInt % 21);
        float randomZ = static_cast<float>(randomZInt % 21);
        // Mise à jour du vecteur wind avec les valeurs aléatoires.
        wind = glm::vec3(randomX,0, randomZ);

        // Réinitialisez la minuterie
        m_elapsedTimeSinceWindUpdate = 0.0;
        ConstantForceFieldPtr windForceField = std::make_shared<ConstantForceField>(this->getParticles(), DynamicSystem::wind);
        this->addForceField(windForceField);

    }
}


const std::vector<ParticlePtr>& DynamicSystem::getParticles() const
{
    return m_particles;
}

void DynamicSystem::setParticles(const std::vector<ParticlePtr> &particles)
{
    m_particles = particles;
}

const std::vector<ForceFieldPtr>& DynamicSystem::getForceFields() const
{
    return m_forceFields;
}

void DynamicSystem::setForceFields(const std::vector<ForceFieldPtr> &forceFields)
{
    m_forceFields = forceFields;
}


float DynamicSystem::getDt() const
{
    return m_dt;
}

void DynamicSystem::setDt(float dt)
{
    m_dt = dt;
}

DynamicSystem::~DynamicSystem() {}

void DynamicSystem::clear()
{
    m_particles.clear();
    m_forceFields.clear();
    m_planeObstacles.clear();
}

bool DynamicSystem::getCollisionDetection()
{
    return m_handleCollisions;
}

void DynamicSystem::setCollisionsDetection(bool onOff)
{
    m_handleCollisions = onOff;
}

void DynamicSystem::addParticle(ParticlePtr p)
{
    m_particles.push_back(p);
}

void DynamicSystem::addForceField(ForceFieldPtr forceField)
{
    m_forceFields.push_back(forceField);
}

void DynamicSystem::addPlaneObstacle(PlanePtr planeObstacle)
{
    m_planeObstacles.push_back(planeObstacle);
}

SolverPtr DynamicSystem::getSolver()
{
    return m_solver;
}

void DynamicSystem::setSolver(SolverPtr solver)
{
    m_solver = solver;
}

void DynamicSystem::detectCollisions()
{
    //Detect particle plane collisions
    for(ParticlePtr p : m_particles)
    {
        for(PlanePtr o : m_planeObstacles)
        {
            if(testParticlePlane(p, o))
            {
                ParticlePlaneCollisionPtr c = std::make_shared<ParticlePlaneCollision>(p,o,m_restitution);
                m_collisions.push_back(c);
            }
        }
    }

    //Detect particle particle collisions
    for(size_t i=0; i<m_particles.size(); ++i)
    {
        for(size_t j=i; j<m_particles.size(); ++j)
        {
            ParticlePtr p1 = m_particles[i];
            ParticlePtr p2 = m_particles[j];
            if(testParticleParticle(p1,p2))
            {
                ParticleParticleCollisionPtr c = std::make_shared<ParticleParticleCollision>(p1,p2,m_restitution);
                m_collisions.push_back(c);
            }
        }
    }
}

void DynamicSystem::solveCollisions()
{
    while(!m_collisions.empty())
    {
        CollisionPtr collision = m_collisions.back();
        collision->solveCollision();
        m_collisions.pop_back();
    }
}

void DynamicSystem::computeSimulationStep()
{
    //Compute particle's force
    for(ParticlePtr p : m_particles)
    {
        p->setForce(glm::vec3(0.0,0.0,0.0));
    }
    for(ForceFieldPtr f : m_forceFields)
    {
        f->addForce();
    }

    //Integrate position and velocity of particles 
    m_solver->solve(m_dt, m_particles);

    //Detect and resolve collisions
    if(m_handleCollisions)
    {
        detectCollisions();
        solveCollisions();
    }
}

const float DynamicSystem::getRestitution()
{
    return m_restitution;
}

void DynamicSystem::setRestitution(const float& restitution)
{
    m_restitution = std::max(0.0f,std::min(restitution,1.0f));
}

std::ostream& operator<<(std::ostream& os, const DynamicSystemPtr& system)
{
    std::vector<ParticlePtr> particles = system->getParticles();
    os << "Particle number: " << particles.size() << std::endl;
    for(ParticlePtr p : particles)
        os << p << std::endl;
    return os;
}

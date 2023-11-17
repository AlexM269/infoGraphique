#include "./../../include/dynamics/ParticleRenderable.hpp"
#include "./../../include/gl_helper.hpp"
#include "./../../include/log.hpp"
#include "./../../include/Utils.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include <random>

ParticleRenderable::~ParticleRenderable()
{}

ParticleRenderable::ParticleRenderable(ShaderProgramPtr program, const ParticlePtr & particle, unsigned int strips, unsigned int slices) :
    MeshRenderable(program, false),
    m_particle(particle)
{
    std::vector<glm::uvec3> uvec3_indices;
    getUnitIndexedSphere(m_positions, m_normals, uvec3_indices, strips, slices);
    // Need to unpack the indices
    unpack(uvec3_indices, m_indices);

    //m_colors.resize( m_positions.size() );
    //for( size_t i = 0; i < m_positions.size(); ++ i )
      //  m_colors[i] = getColor( m_positions[i].x, -1, 1 );

      m_colors.resize(m_positions.size());

      // Crée un générateur de nombres aléatoires pour la transparence
      std::random_device rd;
      std::mt19937 gen(rd());
      //std::uniform_real_distribution<float> alphaDistribution(0.8f, 1.0f); // Valeur de transparence aléatoire entre 0.8 et 1.0
      std::uniform_real_distribution<float> blueDistribution(0.5f, 0.9f); // Valeur de bleu aléatoire entre 0.5 et 1.0
      std::uniform_real_distribution<float> greenDistribution(0.5F, 0.8f); // Valeur de vert aléatoire entre 0.4 et 0.6

      for (size_t i = 0; i < m_positions.size(); ++i) {
          // Génère la valeur de transparence aléatoire
          //float alpha = alphaDistribution(gen);
          // Crée une nuance de bleu et de vert aléatoire
          float blueShade = blueDistribution(gen);
          float greenShade = greenDistribution(gen);
          m_colors[i] = glm::vec4(0.3f, greenShade, blueShade, 1.0);
      }

    update_all_buffers();
}

void ParticleRenderable::do_draw()
{
    //Update the parent and local transform matrix to position the geometric data according to the particle's data.
    const float& pRadius = m_particle->getRadius();
    const glm::vec3& pPosition = m_particle->getPosition();
    glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(pRadius));
    glm::mat4 translate = glm::translate(glm::mat4(1.0), glm::vec3(pPosition));
    setLocalTransform(translate*scale);
    MeshRenderable::do_draw();
}
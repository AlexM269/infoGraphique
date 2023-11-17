#ifndef FLAG_RENDERABLE_HPP
#define FLAG_RENDERABLE_HPP


#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "./../ShaderProgram.hpp"
#include "TexturedMeshRenderable.hpp"
#include "./../dynamics/Particle.hpp"
#include "./../dynamics/SpringForceField.hpp"


class FlagRenderable : public TexturedMeshRenderable {
public:
    FlagRenderable(ShaderProgramPtr program, float width, float height,
        int horizontal_resolution, int vertical_resolution,
        float stiffness, float damping, const std::string &texture_filename),

    ~FlagRenderable();

    void do_draw() override;

    const std::vector<ParticlePtr> & getParticles();
    const std::vector<SpringForceFieldPtr> & getSprings();

private:
    std::vector<ParticlePtr> m_particles;
    std::vector<SpringForceFieldPtr>  m_springs;

};

typedef std::shared_ptr<FlagRenderable> FlagRenderablePtr;
#endif

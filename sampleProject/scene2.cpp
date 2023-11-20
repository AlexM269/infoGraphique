#include <Viewer.hpp>
#include <ShaderProgram.hpp>

#include <texturing/TexturedPlaneRenderable.hpp>
#include <texturing/TexturedMeshRenderable.hpp>
#include <texturing/CubeMapRenderable.hpp>

#include <lighting/DirectionalLightRenderable.hpp>
#include <lighting/LightedMeshRenderable.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <dirent.h>

#include <FrameRenderable.hpp>

#include <dynamics/DynamicSystemRenderable.hpp>
#include <dynamics/DampingForceField.hpp>
#include <dynamics/ConstantForceField.hpp>
#include <dynamics/EulerExplicitSolver.hpp>
#include <dynamics/WaterEffects.hpp>

#include <dynamics/ParticleRenderable.hpp>
#include <dynamics/ParticleListRenderable.hpp>
#include <dynamics/ConstantForceFieldRenderable.hpp>
#include <dynamics/ControlledForceFieldRenderable.hpp>

#include <random>
#include <string>

//PLONGEON//


void initialize_scene( Viewer& viewer )
{
   // Lumière directionnelle du jour pour l'ensemble de la scène
   ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                       "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
   viewer.addShaderProgram( flatShader );
   glm::vec3 d_direction = glm::normalize(glm::vec3(0.0,-1.0,-1.0));
   glm::vec3 d_ambient(1.0,1.0,1.0), d_diffuse(1.0,1.0,1.0), d_specular(1.0,1.0,1.0);
   glm::vec3 lightPosition(0.0,300,0.0);
   DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
   DirectionalLightRenderablePtr directionalLightRenderable = std::make_shared<DirectionalLightRenderable>(flatShader, directionalLight);
   directionalLight->setGlobalTransform(getTranslationMatrix(lightPosition) * directionalLight->getGlobalTransform());
   viewer.addDirectionalLight(directionalLight);
   viewer.addRenderable(directionalLightRenderable);

   {
       // Caméra positionnée à certaine position (1er paramètre) et fixant une target (2ème paramètre)
       viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(15, 2, 8 ), glm::vec3(3,0.65,10), glm::vec3( 0, 1, 0 ) ) );
   }
}


void createIce(Viewer& viewer){
    

   ShaderProgramPtr texShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/simpleTextureVertex.glsl",
                                                                   "../../sfmlGraphicsPipeline/shaders/simpleTextureFragment.glsl");
   viewer.addShaderProgram( texShader );
   ShaderProgramPtr phongShader = std::make_shared<ShaderProgram>( "../../sfmlGraphicsPipeline/shaders/phongVertex.glsl",
                                                                       "../../sfmlGraphicsPipeline/shaders/phongFragment.glsl");
   viewer.addShaderProgram( phongShader );


   // Partie banquise en 3D avec matériau neige illuminable
   std::string banquise_mesh = "./../../sfmlGraphicsPipeline/meshes/banquise.obj";
   MaterialPtr ice = Material::Snow();
   LightedMeshRenderablePtr banquise = std::make_shared<LightedMeshRenderable>(phongShader, banquise_mesh, ice);
   banquise->setGlobalTransform(getTranslationMatrix(-40,0,0));
   banquise->setGlobalTransform(getScaleMatrix(0.5,0.5,0.5));
   viewer.addRenderable(banquise);

   // Partie banquise plane avec texture image neige
   std::string banquise_text = "./../../sfmlGraphicsPipeline/textures/banquise.png";
   TexturedPlaneRenderablePtr texPlane1 = std::make_shared<TexturedPlaneRenderable>(texShader, banquise_text);
   texPlane1->setGlobalTransform(getTranslationMatrix(-500,1,0)*getRotationMatrix(M_PI/2,1,0,0)*getScaleMatrix(950,700,0));
   viewer.addRenderable(texPlane1);
   TexturedPlaneRenderablePtr texPlane2 = std::make_shared<TexturedPlaneRenderable>(texShader, banquise_text);
   texPlane2->setGlobalTransform(getTranslationMatrix(-100,1,350)*getRotationMatrix(M_PI/2,1,0.0,0.0)*getRotationMatrix(M_PI,0,1.0,0.0)
                                   *getRotationMatrix(-2.3*M_PI/3,0,0.0,1.0)*getScaleMatrix(450,600,0));
   viewer.addRenderable(texPlane2);
   TexturedPlaneRenderablePtr texPlane3 = std::make_shared<TexturedPlaneRenderable>(texShader, banquise_text);
   texPlane3->setGlobalTransform(getTranslationMatrix(0,1,-550)*getRotationMatrix(M_PI/2,1,0.0,0.0)*getRotationMatrix(M_PI,0,1.0,0.0)
                                      *getRotationMatrix(2.3*M_PI/3,0,0.0,1.0)*getScaleMatrix(600,700,0));
   viewer.addRenderable(texPlane3);

}


void createSky(Viewer& viewer){ // Skybox du décor panoramique
   ShaderProgramPtr cubeMapShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/cubeMapVertex.glsl",
                                                                   "../../sfmlGraphicsPipeline/shaders/cubeMapFragment.glsl");
   viewer.addShaderProgram(cubeMapShader);
   std::string cubemap_dir = "../../sfmlGraphicsPipeline/textures/CubeMap_Sky";
   auto cubemap = std::make_shared<CubeMapRenderable>(cubeMapShader, cubemap_dir);
   viewer.addRenderable(cubemap);
}


void animateTexture(const TexturedPlaneRenderablePtr texPlane, const std::vector<std::string> textureFiles, sf::Clock &clock, int &currentTextureIndex){
    // Mesure le temps écoulé depuis le dernier changement de texture
    float elapsedTime = clock.getElapsedTime().asSeconds();
    const float textureChangeInterval = 0.1f;
    // Vérifie si le temps écoulé est supérieur à l'intervalle de changement de texture
    if (elapsedTime >= textureChangeInterval) {
       // Change l'indice de la texture à charger avec l'indice suivant
       currentTextureIndex = (currentTextureIndex + 1) % textureFiles.size();
       // Met à jour la texture avec le nouveau filename du vecteur
       texPlane->setTexture(textureFiles[currentTextureIndex]);
       // Réinitialise la minuterie
       clock.restart();
    }
}


void initialize_seal(Viewer& viewer){ // Creation des phoques de départ

    //Instanciation du shader de base 
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");

    //Instanciation du shader pour le phoque lorsqu'il se dandine
    ShaderProgramPtr walkingNonRigidSealShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/walkingNonRigidSealVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/walkingNonRigidSealFragment.glsl");

    //Ajout du flatShader au viewer                                                                
    viewer.addShaderProgram(flatShader);
    //Ajout du fixedwalkingNonRigidSealShader au viewer 
    viewer.addShaderProgram(walkingNonRigidSealShader);
    
    //Initialise le phoque principal
    std::string seal_final_mesh_path = "../../sfmlGraphicsPipeline/meshes/seal_final.obj";
    std::string seal_final_texture_path = "../../sfmlGraphicsPipeline/textures/seal/seal_texture_4.png";
    auto seal_final = std::make_shared<TexturedMeshRenderable>(walkingNonRigidSealShader, seal_final_mesh_path, seal_final_texture_path);
    
    //Instanciation du shader pour les objets non rigides
    ShaderProgramPtr fixedNonRigidSealShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/fixedNonRigidSealVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/fixedNonRigidSealFragment.glsl");

     //Init des autres phoques//
    //2
    auto seal_final_2 = std::make_shared<TexturedMeshRenderable>(fixedNonRigidSealShader, seal_final_mesh_path, seal_final_texture_path);
    seal_final_2->setGlobalTransform(getTranslationMatrix(-4,1.7,2)*getRotationMatrix(0,1,0,0)*getRotationMatrix(-M_PI/2,0,1,0)*getRotationMatrix(0,0,0,1)
                *getScaleMatrix(1.5,1.5,1.5));
    viewer.addRenderable(seal_final_2);

    //3
    auto seal_final_3 = std::make_shared<TexturedMeshRenderable>(fixedNonRigidSealShader, seal_final_mesh_path, seal_final_texture_path);
    seal_final_3->setGlobalTransform(getTranslationMatrix(-3,1.6,3)*getRotationMatrix(0,1,0,0)*getRotationMatrix(-2*M_PI/3,0,1,0)*getRotationMatrix(0,0,0,1)
                *getScaleMatrix(1.5,1.5,1.5));
    viewer.addRenderable(seal_final_3);

    //4
    auto seal_final_4 = std::make_shared<TexturedMeshRenderable>(fixedNonRigidSealShader, seal_final_mesh_path, seal_final_texture_path);
    seal_final_4->setGlobalTransform(getTranslationMatrix(-3.25,1.6,0)*getRotationMatrix(0,1,0,0)*getRotationMatrix(-M_PI/3,0,1,0)*getRotationMatrix(0,0,0,1)
                *getScaleMatrix(1.5,1.5,1.5));
    viewer.addRenderable(seal_final_4);


    //5
    auto seal_final_5 = std::make_shared<TexturedMeshRenderable>(fixedNonRigidSealShader, seal_final_mesh_path, seal_final_texture_path);
    seal_final_5->setGlobalTransform(getTranslationMatrix(-5,1.7,3.5)*getRotationMatrix(0,1,0,0)*getRotationMatrix(-2.5*M_PI/3,0,1,0)*getRotationMatrix(0,0,0,1)
                *getScaleMatrix(1.5,1.5,1.5));
    viewer.addRenderable(seal_final_5);

    //6
    auto seal_final_6 = std::make_shared<TexturedMeshRenderable>(fixedNonRigidSealShader, seal_final_mesh_path, seal_final_texture_path);
    seal_final_6->setGlobalTransform(getTranslationMatrix(-6,1.8,1.5)*getRotationMatrix(0,1,0,0)*getRotationMatrix(-3*M_PI/4,0,1,0)*getRotationMatrix(0,0,0,1)
                *getScaleMatrix(1.5,1.5,1.5));
    viewer.addRenderable(seal_final_6);

    //7
    auto seal_final_7 = std::make_shared<TexturedMeshRenderable>(fixedNonRigidSealShader, seal_final_mesh_path, seal_final_texture_path);
    seal_final_7->setGlobalTransform(getTranslationMatrix(-4.5,1.7,-2)*getRotationMatrix(0,1,0,0)*getRotationMatrix(-M_PI/6,0,1,0)*getRotationMatrix(0,0,0,1)
                *getScaleMatrix(1.5,1.5,1.5));
    viewer.addRenderable(seal_final_7);

    //8
    auto seal_final_8 = std::make_shared<TexturedMeshRenderable>(fixedNonRigidSealShader, seal_final_mesh_path, seal_final_texture_path);
    seal_final_8->setGlobalTransform(getTranslationMatrix(-5.5,1.75,-0.25)*getRotationMatrix(0,1,0,0)*getRotationMatrix(M_PI/5,0,1,0)*getRotationMatrix(0,0,0,1)
                *getScaleMatrix(1.5,1.5,1.5));
    viewer.addRenderable(seal_final_8);

    //Initialise sa position
    seal_final->setGlobalTransform(getTranslationMatrix(3,0.65,10)*getRotationMatrix(M_PI/2,0,1,0)
                *getScaleMatrix(1.5,1.5,1.5));

    //Animation procédurale du phoque vers le bord de la banquise
    seal_final->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.2,-1), glm::quat{}, glm::vec3(1,1,1)),0.0);

    //Premier arrêt
    seal_final->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0,1.25), glm::quat{}, glm::vec3(1,1,1)),5.0);

    //Changment de vitesse
    seal_final->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,-0.2,3.5),glm::quat{}, glm::vec3(1,1,1)),7);

    //Début du plongeon (+0.5 s)
    seal_final->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,0.75,5),
    glm::angleAxis(float(M_PI/4),glm::vec3(1,0,0)), glm::vec3(1,1,1)),7.5);

    //Fin du plongeon (+0.4s)
    seal_final->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,-1.35,6),
    glm::angleAxis(float(M_PI/2),glm::vec3(1,0,0)), glm::vec3(1,1,1)),7.9); 

    seal_final->addLocalTransformKeyframe(GeometricTransformation(glm::vec3(0,-1.35,6),
    glm::angleAxis(float(M_PI/2),glm::vec3(1,0,0)), glm::vec3(1,1,1)),12);     

    viewer.addRenderable(seal_final);
    viewer.startAnimation();
}


std::vector<std::string> listFilesInDirectory(const std::string& directoryPath) {
     //Fonction de gestion de répertoire pour l'animation des textures
   std::vector<std::string> textureFiles;
   DIR* dir;
   struct dirent* ent;
   if ((dir = opendir(directoryPath.c_str())) != NULL) {
       while ((ent = readdir(dir)) != NULL) {
           if (ent->d_type == DT_REG) {  // Vérifier si c'est un fichier régulier
               std::string fileName = ent->d_name;
                   textureFiles.push_back(directoryPath + "/" + fileName);
           }
       }
       closedir(dir);
   } else {
       std::cerr << "Erreur lors de l'ouverture du répertoire." << std::endl;
   }
   return textureFiles;
}


void createSplash(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable){
     //Initialize a shader for the following renderables
     ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex_splash.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment_splash.glsl");
     viewer.addShaderProgram( flatShader );

        //génération de nb aléatoires
        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_real_distribution<float> pr_distribution(0.07f, 0.1f); //entre 0.05 et 0.08 pour la taille
        std::uniform_real_distribution<float> x_distribution(8.5f, 15.5f); //entre -3.5 et 3.5 pour la position en x
        std::uniform_real_distribution<float> z_distribution(6.5f, 13.5f); //entre -3.5 et 3.5 pour la position en z
        std::uniform_real_distribution<float> pm_distribution(3.5f, 5.5f); //et 1.5 et 3.5 pour la masse

        glm::vec3 px,pv;
        float pm, pr;
       // Crée 15 particules avec certaines valeurs aléatoires
       for (int i = 0; i < 25; ++i) {
          float random_pr = pr_distribution(generator);
          float random_x = x_distribution(generator);
          float random_pm = pm_distribution(generator);
          float random_z = z_distribution(generator);
           //Initialize a particle with position, velocity, mass and radius and add it to the system
          px = glm::vec3(random_x, 0.7, random_z);
          pv = glm::vec3(0.0, 0.0, 0.0);
          pr = random_pr;
          pm = random_pm;
          ParticlePtr particle = std::make_shared<Particle>(px, pv, pm, pr);
          system->addParticle(particle);
       }

       std::uniform_real_distribution<float> x_distribution2(-1.0f, 1.0f); //entre -1.5 et 1.5 pour la position en x
       std::uniform_real_distribution<float> z_distribution2(-1.0f, 1.0f); //entre -1.5 et 1.5 pour la position en z
       // 2ème groupe de particules plus proche du centre
       for (int i = 0; i < 15; ++i) {
            float random_pr = pr_distribution(generator);
            float random_x = x_distribution2(generator);
            float random_z = z_distribution2(generator);
            float random_pm = pm_distribution(generator);
            //Initialize a particle with position, velocity, mass and radius and add it to the system
            px = glm::vec3(random_x, 0.7, random_z);
            pv = glm::vec3(0.0, 0.0, 0.0);
            pr = random_pr;
            pm = random_pm;
            ParticlePtr particle = std::make_shared<Particle>(px, pv, pm, pr);
            system->addParticle(particle);
       }

         //Initialize a force field that apply to all the particles of the system to simulate gravity
         //Add it to the system as a force field
         ConstantForceFieldPtr gravityForceField = std::make_shared<ConstantForceField>(system->getParticles(), DynamicSystem::gravity);
         system->addForceField(gravityForceField);

         // Crée une instance de WaterEffects en fixant les paramètes
         WaterEffectsPtr waterEffects = std::make_shared<WaterEffects>(system->getParticles(), glm::vec3(12,0.7,10), 0.7f, 2.5f);
         // Ajoute waterEffects comme force au système
         system->addForceField(waterEffects);

         //Initialize a force field that apply to all the particles of the system to simulate vicosity (air friction)
         float dampingCoefficient = 5.0;
         DampingForceFieldPtr dampingForceField = std::make_shared<DampingForceField>(system->getParticles(), dampingCoefficient);
         system->addForceField(dampingForceField);

         //Create a particleRenderable for each particle of the system
         //DynamicSystemRenderable act as a hierarchical renderable
         //This which allows to easily apply transformation on the visualiazation of a dynamicSystem
         for (int i = 0; i < 40; ++i) {
             ParticleRenderablePtr particleRenderable = std::make_shared<ParticleRenderable>(flatShader, system->getParticles()[i]);
             HierarchicalRenderable::addChild(systemRenderable, particleRenderable);
         }

}


void animate_camera(Viewer& viewer){
    
    viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(12, 2, 6 ), glm::vec3(7.5,-0.5,10), glm::vec3( 0, 1, 0 ) ) );

    //lookAt(position des yeux, position de la cible, (0,1,0))

    //Début du plan 
    viewer.getCamera().addGlobalTransformKeyframe(lookAtUpModel(glm::vec3(12, 2, 5  ), glm::vec3(7.5,-0.5,10)),0.0);

    //Fin du plan avec vue sur le plongeon
    viewer.getCamera().addGlobalTransformKeyframe(lookAtUpModel(glm::vec3(10.5, 1.5, 5  ), glm::vec3(9,0,10)),7.9);

    viewer.getCamera().addGlobalTransformKeyframe(lookAtUpModel(glm::vec3(9.5, 1.5, 5  ), glm::vec3(11,0,10)),12);
}


int main()
{
   Viewer viewer(1280,720, glm::vec4(0.8, 0.8, 0.8, 1.0));
   initialize_scene(viewer);
   createSky(viewer);
   createIce(viewer);
   

   // Pour le plan avec texture eau animée
   ShaderProgramPtr texShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/simpleTextureVertex.glsl",
                                                              "../../sfmlGraphicsPipeline/shaders/simpleTextureFragment.glsl");

   viewer.addShaderProgram( texShader );


   // Vecteur contenant toutes les frames de la texture animée
   std::string directoryPath = "./../../sfmlGraphicsPipeline/textures/waves";

   std::vector<std::string> textureFiles = listFilesInDirectory(directoryPath);
   // Initialise un TexturedPlaneRenderable avec la première texture du vecteur textureFiles
   TexturedPlaneRenderablePtr texPlane = std::make_shared<TexturedPlaneRenderable>(texShader, textureFiles[0]);

   texPlane->setGlobalTransform(getRotationMatrix(M_PI/2,1,0,0)*getScaleMatrix(500));
   texPlane->updateTextureOption(2); //Utilise l'option de texture MIRROR_REPEAT
   viewer.addRenderable(texPlane);

   sf::Clock clock; // Minuterie pour le changement de texture
   int currentTextureIndex = 0; // Indice de l'image de texture initial

   initialize_seal(viewer);

   
//Tout pour le splashh//
    //Set up a shader and add a 3D frame.
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex_splash.glsl", 
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment_splash.glsl");
    viewer.addShaderProgram( flatShader );

    //Initialize a dynamic system (Solver, Time step, Restitution coefficient)
    DynamicSystemPtr system = std::make_shared<DynamicSystem>();
    EulerExplicitSolverPtr solver = std::make_shared<EulerExplicitSolver>();
    system->setSolver(solver);
    system->setDt(0.01);

    //Create a renderable associated to the dynamic system
    //This renderable is responsible for calling DynamicSystem::computeSimulationStep() in the animate() function
    //It is also responsible for some of the key/mouse events
    DynamicSystemRenderablePtr systemRenderable = std::make_shared<DynamicSystemRenderable>(system);
    viewer.addRenderable(systemRenderable);

    animate_camera(viewer);
   
    bool splash_fait = false;

   while( viewer.isRunning() )
   {
       // Anime la texture du plan avec la mer
       animateTexture(texPlane, textureFiles, clock, currentTextureIndex);


      viewer.handleEvent();

      //Crée la gerbe d'eau moment du plongeon du phoque
      if(viewer.getTime() > 7.7){
        if(!splash_fait){
            splash_fait = true;
            createSplash(viewer, system, systemRenderable);
        }
      }
      viewer.animate();
      viewer.draw();
      viewer.display();

   } 


   return EXIT_SUCCESS;
}


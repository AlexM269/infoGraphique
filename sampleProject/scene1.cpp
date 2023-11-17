#include <Viewer.hpp>
#include <ShaderProgram.hpp>


#include <texturing/TexturedPlaneRenderable.hpp>
#include <texturing/TexturedCubeRenderable.hpp>
#include <texturing/TexturedMeshRenderable.hpp>
#include <texturing/MultiTexturedCubeRenderable.hpp>
#include <texturing/MipMapCubeRenderable.hpp>
#include <texturing/BillBoardPlaneRenderable.hpp>
#include <lighting/DirectionalLightRenderable.hpp>
#include <texturing/TexturedTriangleRenderable.hpp>
#include <texturing/CubeMapRenderable.hpp>
#include <FrameRenderable.hpp>
#include <lighting/DirectionalLightRenderable.hpp>
#include <lighting/LightedMeshRenderable.hpp>


#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <dirent.h>


#include <iostream>
#include <string>

// SCENE DU REVEIL


void initialize_scene( Viewer& viewer )
{
   //Position the camera
  

   //Add a 3D frame to the viewer
   //FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
   //viewer.addRenderable(frame);
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


       // Caméra positionnée à une hauteur de 10 (1er paramètre) et fixant une target à hauteur 10 également (2ème paramètre)
      
        //Camera originale
       //viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(100, 30, 50), glm::vec3(0, 10, 0), glm::vec3( 0, 1, 0 ) ) );
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


void animateTexture(const TexturedPlaneRenderablePtr texPlane, const std::vector<std::string> textureFiles, 
sf::Clock &clock, int &currentTextureIndex, float timeOfChange){
    // Mesure le temps écoulé depuis le dernier changement de texture
    float elapsedTime = clock.getElapsedTime().asSeconds();
    const float textureChangeInterval = timeOfChange;
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

    //Instanciation du shader pour les objets non rigides
    ShaderProgramPtr fixedNonRigidSealShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/fixedNonRigidSealVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/fixedNonRigidSealFragment.glsl");

    //Ajout du flatShader au viewer                                                                
    viewer.addShaderProgram(flatShader);
    //Ajout du fixedNonRigidSealShader au viewer 
    viewer.addShaderProgram(fixedNonRigidSealShader);
    
    //Initialisation du mesh et de la texture des phoques endormis
    std::string seal_final_mesh_path = "../../sfmlGraphicsPipeline/meshes/seal_final.obj";
    std::string seal_final_texture_path = "../../sfmlGraphicsPipeline/textures/seal/seal_texture_1.png";

    //Initialisation du phoque principal et de sa postion
    auto seal_final = std::make_shared<TexturedMeshRenderable>(fixedNonRigidSealShader, seal_final_mesh_path, seal_final_texture_path);
    seal_final->setGlobalTransform(getTranslationMatrix(1,0.85,10)*getRotationMatrix(M_PI/2,0,1,0)
                *getScaleMatrix(1.5,1.5,1.5));
    viewer.addRenderable(seal_final);

    
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
    
    //Resout un bug avec la flèche
    viewer.startAnimation();

}


std::vector<std::string> listFilesInDirectory(const std::string& directoryPath) {
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

void movingCamera(Viewer& viewer)
{
    //Coords du phoque : (3 , 0.65 , 10)

    viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(10, 4, -10 ), glm::vec3(-10,5,-30), glm::vec3( 0, 1, 0 ) ) );
    //lookAt(position des yeux, position de la cible, (0,1,0))
    viewer.getCamera().addGlobalTransformKeyframe(lookAtUpModel(glm::vec3(10, 4, -10 ), glm::vec3(-10,5,-19)),0.0);

    //Regarde les phoques
    viewer.getCamera().addGlobalTransformKeyframe(lookAtUpModel(glm::vec3(15, 4, -10 ), glm::vec3(-7,0.65,10)),8.0);

    //Attend
    viewer.getCamera().addGlobalTransformKeyframe(lookAtUpModel(glm::vec3(15, 4, -10 ), glm::vec3(-2,0.65,10)),10.5);

    //Zoom sur le phoque principal
    viewer.getCamera().addGlobalTransformKeyframe(lookAtUpModel(glm::vec3(3, 1.4, 9 ), glm::vec3(1,0.85,10)), 15);

    //Reste sur le phoque
    viewer.getCamera().addGlobalTransformKeyframe(lookAtUpModel(glm::vec3(3, 1.4, 9 ), glm::vec3(1,0.85,10)), 23);

}


void animateMeshTexture(const TexturedMeshRenderablePtr texPlane, const std::vector<std::string> textureFiles, 
sf::Clock &clock, int &currentTextureIndex, float timeOfChange){
    // Mesure le temps écoulé depuis le dernier changement de texture
    float elapsedTime = clock.getElapsedTime().asSeconds();
    const float textureChangeInterval = timeOfChange;
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


int main()
{
   Viewer viewer(1280,720, glm::vec4(0.8, 0.8, 0.8, 1.0));
   initialize_scene(viewer);
   createSky(viewer);
   createIce(viewer);
   
//ANIMATION OCEAN//
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
   texPlane->updateTextureOption(2);
   viewer.addRenderable(texPlane);

//ANIMATION PHOQUE//

    // Vecteur contenant toutes les frames de l'animation du phoque qui se réveille

    //Instanciation du shader pour les objets non rigides fixes
    ShaderProgramPtr fixedNonRigidSealShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/fixedNonRigidSealVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/fixedNonRigidSealFragment.glsl");
    viewer.addShaderProgram(fixedNonRigidSealShader);
    
    //Initialisation du mesh
    std::string seal_mesh_path = "../../sfmlGraphicsPipeline/meshes/seal_final.obj";
    
    std::string seal_texture_path = "../../sfmlGraphicsPipeline/textures/seal";

    std::string seal_texture = "../../sfmlGraphicsPipeline/textures/seal/seal_texture_1.png";

    std::vector<std::string> sealFiles = listFilesInDirectory(seal_texture_path);

    //Initialisation du phoque principal et de sa postion
    auto seal_final = std::make_shared<TexturedMeshRenderable>(fixedNonRigidSealShader, seal_mesh_path, seal_texture);

    seal_final->setGlobalTransform(getTranslationMatrix(1,0.85,10)*getRotationMatrix(M_PI/2,0,1,0)
                *getScaleMatrix(1.5,1.5,1.5));

    viewer.addRenderable(seal_final);

   sf::Clock clock; // Minuterie pour le changement de texture
   int currentTextureIndex = 0; // Indice de l'image de texture initial
   int currentSealIndex = 0;

   initialize_seal(viewer);
   movingCamera(viewer);



   while( viewer.isRunning() )
   {
       // Anime la texture du plan avec la mer
       animateTexture(texPlane, textureFiles, clock, currentTextureIndex,0.1f);

        viewer.handleEvent();

        //Activation de l'animation de la texture
        if(viewer.getTime() > 18 && viewer.getTime() < 20.3){
            animateMeshTexture(seal_final,sealFiles,clock,currentSealIndex,0.08f);
        }

        viewer.animate();
        viewer.draw();
        viewer.display();
         
   } 


   return EXIT_SUCCESS;
}


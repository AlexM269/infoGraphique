## Informatique Graphique INSA 2023 - L'Exploration

Dans le cadre de l’option Médias & Interactions, nous devons réaliser un court film d’animation d’une durée d’environ une minute combinant les notions étudiées en cours et en TP d’Informatique Graphique. Il doit ainsi contenir au minimum une animation procédurale, une animation basée sur la physique, différentes sources de lumières, des textures et des matériaux adéquats.

Cette année, le projet porte sur une expédition au pôle nord, comprenant un bateau comme élément principal. Nous allons donc tout d’abord détailler le scénario de notre film puis introduire les éléments sur lesquels nous nous sommes appuyés lors de sa réalisation. Ensuite, nous expliquerons comment nous avons mis en scène ces composantes tout au long de notre film. Ce compte rendu comportera enfin les ressources associées à notre projet, permettant de le visualiser et d’accéder à l’ensemble de son contenu.


### Lancement des scènes

Chaque scène est référencée sous le nom “sceneX.cpp”. La scène 3 est divisée en deux parties : le zoom vers le coffre puis celui vers le drapeau pirate permettant la transition avec la dernière scène comme détaillé au cours de ce rapport.

Pour exécuter les scènes, il faut tout d’abord cloner le projet avec la commande git clone à partir d’un terminal et du lien https renseigné sur le git ou bien télécharger le fichier zip et l’extraire. Nous pouvons ensuite reprendre les étapes d’initialisation vues en TP en réalisant ces commandes dans un terminal ouvert à la racine : 

cd sfmlGraphicsPipeline
cd extlib
make -j6
cd ../
mkdir build
cd build
cmake ../
make -j6
cd ../../
cd sampleProject
mkdir build
cd build
cmake ../
make -j6 

Il ne reste plus qu’à lancer les scènes une à une en tapant leur nom dans le terminal, toujours dans le dossier build de sampleProject.


### Film en vidéo

La vidéo du film totalement monté à partir du code et des bandes sonores est disponible sur Youtube avec le lien suivant : 
https://youtu.be/dTHuk3hOYRo

Il regroupe ainsi nos quatres scènes enchainées correctement et les crédits finaux, pour une durée totale de 2 minutes 40 secondes.


### Logiciels
- OpenGL
- C++
- glm
- SFML
- + logiciels externes d'édition d'images, de modélisation 3D, de dessin et de montage vidéo


### Intervenants
- Maud Marchal
- Glenn Kerbiriou 


### Etudiants de l'équipe
- Prune Lepvraud
- Alexandre Mahjoub
- Anouk Migliavacca

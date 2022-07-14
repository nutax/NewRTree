# NewRTree
## Informe
[CLick aquí](https://www.overleaf.com/read/vxkmdztxydsp)
## Ejecutar
#### Master Branch
Para correr en Windows abra el .sln en visual studio
Instale el Nugget Package: "SFML-cpp" por Laurent Gomila

### Linux Branch
Ejecute en el directorio raiz
```
g++ -std=c++17 -g -c main.cpp &&  g++ -g main.o -o sfml-app -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system && ./sfml-app
```

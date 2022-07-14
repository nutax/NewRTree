# NewRTree
## Informe
[CLick aquí](https://www.overleaf.com/read/vxkmdztxydsp)
## Presentación
[CLick aquí](https://docs.google.com/presentation/d/1vC5TYz4MlJO5x-nSv70hjuG0TJmf-CS9YIQ8qKS1ISo/edit?usp=sharing)
## Ejecutar
#### Master Branch
Para correr en Windows abra el .sln en visual studio
Instale el Nugget Package: "SFML-cpp" por Laurent Gomila

### Linux Branch
Ejecute en el directorio raiz
```
g++ -std=c++17 -g -c main.cpp &&  g++ -g main.o -o sfml-app -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system && ./sfml-app
```

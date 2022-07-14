# NewRTree
## Informe
[Click aquí](https://www.overleaf.com/read/vxkmdztxydsp)
## Presentación
[Click aquí](https://docs.google.com/presentation/d/1vC5TYz4MlJO5x-nSv70hjuG0TJmf-CS9YIQ8qKS1ISo/edit?usp=sharing)
## Video
[Click aquí](https://www.youtube.com/watch?v=t2ufdmN5eQg&ab_channel=LuisBerrospi)
## Ejecutar
#### Master Branch
Para correr en Windows abra el .sln en visual studio
Instale el Nugget Package: "SFML-cpp" por Laurent Gomila

### Linux Branch
Ejecute en el directorio raiz
```
g++ -std=c++17 -g -c main.cpp &&  g++ -g main.o -o sfml-app -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system && ./sfml-app
```

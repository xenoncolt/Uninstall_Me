# Compile with:
### Windows:
```
g++ src/main.cpp -o output/UninstallMe.exe -static-libgcc -static-libstdc++ -I"libs/SFML/include" -L"libs/SFML/lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```
### Linux:
```
g++ src/main.cpp -o UninstallMe -std=c++17 \ -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```
### Download libraries from release page and extract to `libs/` folder.

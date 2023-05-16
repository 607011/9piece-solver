# 9piece-solver

Solver for the 3x3 Scramble Puzzle, written in C++


## Prequisites

- C++11 compiler


## Build


### Linux, macOS

```
git clone https://github.com/607011/9piece-solver.git
cd 9piece-solver
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE ..
cmake --build .
```

### Windows

In Visual Studio Developer Console:

```
git clone https://github.com/607011/9piece-solver.git
cd 9piece-solver
md build
cd build
cmake ..
cmake --build . --config=Release
```


## Run

```
9piece-solver data/puzzle-0000.txt
```

## License

See [LICENSE](LICENSE).


_Copyright ©️ 2023 [Oliver Lau](mailto:ola@ct.de), [Heise](https://www.heise.de/) Medien GmbH & Co. KG_

--- 

### Nutzungshinweise

Diese Software wurde zu Lehr- und Demonstrationszwecken geschaffen und ist nicht für den produktiven Einsatz vorgesehen. Heise Medien und der Autor haften daher nicht für Schäden, die aus der Nutzung der Software entstehen, und übernehmen keine Gewähr für ihre Vollständigkeit, Fehlerfreiheit und Eignung für einen bestimmten Zweck.

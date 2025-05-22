cmake_minimum_required(VERSION 3.10)
project(AntColony)

# Spécifier la version du standard C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Créer un exécutable pour colony1.cpp
add_executable(colony1 colony1.cpp)

# Créer un exécutable pour colony2.cpp
add_executable(colony2 colony2.cpp)
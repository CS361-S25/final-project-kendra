g++ -O3 -DNDEBUG -march=native -Wall -Wno-unused-function -std=c++17 -Isignalgp-lite/third-party/Empirical/include/ -Isignalgp-lite/include/ -g native.cpp -o native_project
lldb native_project
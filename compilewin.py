from os import system

print("Compiling to Windows")
system("x86_64-w64-mingw32-g++ -static-libgcc -static-libstdc++ -o 'balanço energia'.exe main.cpp")
print("Compiling perfomed sucessfully")

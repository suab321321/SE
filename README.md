# SE
This is a just a program to search for local files in your system using the the multithreaded method to increase its speed.
machine(except for those whcih require root access, like boot folder or any other os related folders).You need to provide the
correct name of file the no of those files you want.Generate the binary executable go inside the src folder and type this in the
terminal `g++ -std=c++1z SearchEngine.cpp -lboost_filesystem -lboost_system -lpthread`.When you run the program it will ask for
starting searching directory,filename and no of search collisions or the files with same name but different location you want.


Any new improvement or any issues feel free to report.

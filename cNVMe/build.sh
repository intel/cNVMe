g++ --version
printf "Starting build!\n"
g++ *.h *.cpp -w -fpermissive -o cNVMe.out -pthread -std=c++11
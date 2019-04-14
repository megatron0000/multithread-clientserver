g++ -O3 client.cpp -o client -lpthread -lrt # -lpthread must be at the END !
g++ -O3 server.cpp -o server -lpthread -lrt
echo "Done !"

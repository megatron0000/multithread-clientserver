g++ -O3 client.cpp -o client -lpthread # -lpthread must be at the END !
g++ -O3 server.cpp -o server -lpthread 
echo "Done !"
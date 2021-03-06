

#include "ClientServerCommunication.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>

#define MAX_COMMAND_LEN 50

ClientServerCommunication::ClientServerCommunication(const char *serverIP, int serverPort):
        serverIP(serverIP), serverPort(serverPort), clientSocket(0) {
}

void ClientServerCommunication::connectToServer(Display *displayP) {
    // Create a socket point
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        throw "Error opening socket";
    }
    // Convert the ip string to a network address
    struct in_addr address;
    if (!inet_aton(serverIP, &address)) {
        throw "Can't parse IP address";
    }
    // Get a hostent structure for the given host address
    struct hostent *server;
    server = gethostbyaddr((const void *)&address, sizeof address, AF_INET);
    if (server == NULL) {
        throw "Host is unreachable";
    }

    // Create a structure for the server address
    struct sockaddr_in serverAddress;
    bzero((char *)&address, sizeof(address));
    serverAddress.sin_family = AF_INET;
    memcpy((char *)&serverAddress.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    // htons converts values between host and network byte orders
    serverAddress.sin_port = htons(serverPort);
    // Establish a connection with the TCP server
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        throw "Error connecting to server";
    }
    displayP->printMessage("Connected to server");

}

int ClientServerCommunication::getClientTurn() {
    int turn;
    int n=read(clientSocket,&turn,sizeof(turn));
    if (n==-1){
        throw "Error reading from socket";
    }
    if (n == 0) {
        throw "Server disconnected";
    }
    return turn;
}

void ClientServerCommunication::sendMoveToServer(pair<int,int> chosenMove) {
    int n=write(clientSocket,&chosenMove.first,sizeof(chosenMove.first));
    if (n==-1){
        throw "Error writing to socket";
    }
    if (n == 0) {
        throw "Server disconnected";
    }
    n=write(clientSocket,&chosenMove.second,sizeof(chosenMove.second));
    if (n==-1){
        throw "Error writing to socket";
    }
    if (n == 0) {
        throw "Server disconnected";
    }
}

pair<int, int> ClientServerCommunication::getMoveFromServer(){
    int row,col;
    int n=read(clientSocket,&row,sizeof(row));
    if (n==-1){
        throw "Error reading from socket";
    }
    if (n == 0) {
        throw "Server disconnected";
    }
    n=read(clientSocket,&col,sizeof(col));
    if (n==-1){
        throw "Error reading from socket";
    }
    if (n == 0) {
        throw "Server disconnected";
    }
    return make_pair(row,col);
}

void ClientServerCommunication::sendCommandToServer(string &command) {
    const char *temp=command.c_str();
    char com[MAX_COMMAND_LEN];
    for(int i=0;i<MAX_COMMAND_LEN;i++){
        if(i<command.size())
            com[i]=temp[i];
        else
            com[i]='\0';
    }
    int n=write(clientSocket,com,sizeof(com));
    if (n==-1){
        throw "Error writing to socket";
    }
    if (n == 0) {
        throw "Server disconnected";
    }
}

int ClientServerCommunication::getStatusFromServer() {
    int status;
    int n=read(clientSocket,&status,sizeof(status));
    if (n==-1){
        throw "Error reading from socket";
    }
    if (n == 0) {
        throw "Server disconnected";
    }
    return status;
}

string ClientServerCommunication::getNameFromServer() {
    char nameStr[MAX_COMMAND_LEN];
    int n=read(clientSocket,nameStr,sizeof(nameStr));
    if (n==-1){
        throw "Error reading from socket";
    }
    if (n == 0) {
        throw "Server disconnected";
    }
    string name(nameStr);
    return name;
};
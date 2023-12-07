#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

using namespace std;

class JarraPequena {
private:
    int capacidad;
    int contenido;
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

   void escribirAccion(const string& accion) {
        cout << accion << " contenido:" << contenido << endl;
    }

    bool contiene(const std::string& str, string s) {
        return str.find(s) != std::string::npos;
    }

public:
    JarraPequena() : capacidad(3), contenido(0) {
        // Creando el descriptor de archivo de socket
        server_fd = socket(AF_INET, SOCK_STREAM, 0);

        // Asignando dirección y puerto
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(8080);

        // Forzar asignación de puerto 8080 al socket
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }

        // Poniendo el servidor a escuchar
        if (listen(server_fd, 3) < 0) {
            perror("listen failed");
            exit(EXIT_FAILURE);
        }

        // Mostrar la dirección y el puerto
        struct sockaddr_in sin;
        socklen_t len = sizeof(sin);
        if (getsockname(server_fd, (struct sockaddr *)&sin, &len) == -1)
            perror("getsockname");
        else
            cout << "Listening on IP: " << inet_ntoa(sin.sin_addr) << " and port: " << ntohs(sin.sin_port) << endl;
    }

    void vaciar(int lim) {
        contenido -= lim;
        if (contenido < 0) contenido = 0;
        escribirAccion("vaciar(jp," + to_string(lim) + ")");
    }

    void anadir(int lim) {
        contenido += lim;
        string msg = "anadir(JG,jp," + to_string(lim) + ")";
        escribirAccion(msg);
    }

    void transvasar(int value) {
        contenido += value;
        string msg = "transvasar(JG,jp," + to_string(value) + ")";
        escribirAccion(msg);
    }

    void listoParaRecibir() {
        string mensaje = "Listo-para-recibir";
        if (send(new_socket, mensaje.c_str(), mensaje.size(), 0) < 0) {
            perror("send failed");
        }
        escribirAccion(mensaje);
    }

    int run (){
        bool flag = false;
        // Bucle infinito para aceptar conexiones entrantes
        while (true) { 
            // Aceptando conexiones entrantes
            new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

            if (new_socket < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // Recibir mensajes 
            while(true) {
                char buffer[1024] = {0};
                int bytes_read = read(new_socket, buffer, 1024);
                
                if (bytes_read == 0) {
                    // No hay más datos, cerrar esta conexión
                    close(new_socket);
                    break;
                }

                cout << "JG -> Mensaje recibido: " << buffer << endl;

                if(contiene(buffer, "anadir")){
                    string str = buffer;
                    // Posición justo después de la última coma
                    size_t start = str.find_last_of(',') + 1;
                    // Posición del último paréntesis
                    size_t end = str.find_last_of(')');       

                    if (start != string::npos && end != string::npos) {
                        // Extraer la subcadena
                        string valueStr = str.substr(start, end - start); 
                        // Convertir a entero
                        int value = stoi(valueStr); 
                        anadir(value);
                    }
                    vaciar(3);
                    listoParaRecibir();
                    if(flag){
                        return 0;
                    }
                }
                else if(contiene(buffer, "transvasar")){
                    string str = buffer;
                    // Posición justo después de la última coma
                    size_t start = str.find_last_of(',') + 1;
                    // Posición del último paréntesis
                    size_t end = str.find_last_of(')');       

                    if (start != string::npos && end != string::npos) {
                        // Extraer la subcadena
                        string valueStr = str.substr(start, end - start); 
                        // Convertir a entero
                        int value = stoi(valueStr); 
                        transvasar(value);
                    }
                    vaciar(3);
                    listoParaRecibir();
                    flag = true;
                }
            }
        }
        return 0;
    }

    void closeSocket (){
        close(server_fd);
    }
};

int main() {
    JarraPequena jp;
    jp.run();
    jp.closeSocket();
    cout << "FIN Jarra Pequena" << endl;
}

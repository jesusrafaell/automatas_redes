#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <vector>

using namespace std;

class JarraGrande {
    private:
        int capacidad;
        int contenido;
        int state;
        struct sockaddr_in serv_addr;
        int sock = 0;

        void escribirAccion(const string& accion) {
            cout << accion << " contenido:" << contenido << endl;
        }


    public:
        void connectar () {
            while (true) {
                cout << "Intentando conectar a IP: " << inet_ntoa(serv_addr.sin_addr)
                     << " en el puerto: " << ntohs(serv_addr.sin_port) << endl;

                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                    cout << "Fallo al conectar, reintentando..." << endl;
                    sleep(2); 
                } else {
                    cout << "Conectado exitosamente." << endl;
                    break;
                }
            }
        }

        JarraGrande() : capacidad(5), contenido(0), state(0) {
            sock = socket(AF_INET, SOCK_STREAM, 0);
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(8080);
            
            inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
            connectar();
        }


        void llenar(int cantidad) {
            contenido += cantidad;
            if (contenido > capacidad) contenido = capacidad;
            escribirAccion("llenar(JG," + to_string(cantidad) + ")");
        }

        void vaciar(int lim) {
            contenido -= lim;
            if (contenido < 0) contenido = 0;
            escribirAccion("vaciar(JG," + to_string(lim) + ")");
        }

        void transvasar() {
            string msg = "transvasar(JG,jp," + to_string(contenido) +")";
            send(sock, msg.c_str(), msg.size(), 0);
            escribirAccion(msg);
            contenido = 0;
        }

        void anadir(int lim) {
            //[3312] en el caso cuando la jarra pequena tenga algo
            // debo restar solo lo que puede pasar;
            contenido -= lim;
            string msg = "anadir(JG,jp," + to_string(lim) + ")";
            send(sock, msg.c_str(), msg.size(), 0);
            escribirAccion(msg);
        }

        void esperarPorRespuesta() {
            char buffer[1024];
            int bytesLeidos;

            cout << "Waiting" << endl;
            while (true)
            {
                memset(buffer, 0, sizeof(buffer));
                bytesLeidos = recv(sock, buffer, sizeof(buffer), 0);

                if (bytesLeidos > 0)
                {
                    cout << "Respuesta recibida: " << buffer << endl;
                    break;
                }
                else if (bytesLeidos == 0)
                {
                    cout << "El servidor ha cerrado la conexión. Reintentando..." << endl;
                    sleep(2);
                    connectar();
                }
                else
                {
                    cout << "Error en la recepción de datos. Reintentando..." << endl;
                    sleep(2); // Esperar antes de reintentar, por ejemplo
                }
            }
    }

        void closeSocket (){
            close(sock);
        }
};

int main() {
	JarraGrande jg;

    // Enviar mensaje "hola"
    jg.llenar(5);
    jg.anadir(3);
    jg.esperarPorRespuesta(); // Esperar por respuesta
    jg.transvasar();
    jg.llenar(5);
    jg.anadir(3);
    jg.esperarPorRespuesta(); // Esperar por respuesta

    jg.closeSocket();

    cout << "FIN Jarra Grande" << endl;
}

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdio>
#include <string>

using namespace std;

struct Orden {
    unsigned int espera = 0;
    bool soltarGranada1 = false;
    bool soltarGranada2 = false;
    bool ataqueKamikaze = false;
    bool aterrizaje = false;
    bool despegue = false;
    int siguientex = 0;
    int siguientey = 0;
};

struct OrdenArchivo {
    int x = 0;
    int y = 0;
    unsigned int espera = 0;
    bool soltarGranada1 = false;
    bool soltarGranada2 = false;
    bool ataqueKamikaze = false;
    bool aterrizaje = false;
    bool despegue = false;
    int siguientex = 0;
    int siguientey = 0;
};

const int FILAS = 200;
const int COLUMNAS = 200;

Orden memoria[FILAS][COLUMNAS];
bool memoriaOcupada[FILAS][COLUMNAS]; // Matriz auxiliar para saber qué celdas tienen datos

void inicializarMemoria() {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            memoriaOcupada[i][j] = false;
            memoria[i][j] = Orden();
        }
    }
}

// operacion 1: cargar archivo de ataque en memoria

bool validarCantidadesBasicas(const vector<OrdenArchivo>& registros, int& idxDespegue) {
    int cantDespegues = 0;
    int cantFinOperacion = 0;

    for (size_t i = 0; i < registros.size(); i++) {
        if (registros[i].despegue) {
            cantDespegues++;
            idxDespegue = i;
            if (registros[i].soltarGranada1 || registros[i].soltarGranada2 || registros[i].ataqueKamikaze) {
                cout << "Error de validacion: El despegue tiene ataques asociados.\n";
                return false;
            }
        }
        if (registros[i].aterrizaje || registros[i].ataqueKamikaze) {
            cantFinOperacion++;
        }
    }

    if (cantDespegues != 1) {
        cout << "Error de validacion: Debe haber exactamente 1 instruccion de despegue.\n";
        return false;
    }
    if (cantFinOperacion != 1) {
        cout << "Error de validacion: Debe haber exactamente 1 aterrizaje o ataque Kamikaze.\n";
        return false;
    }
    return true;
}

bool validarRutaContinua(const vector<OrdenArchivo>& registros, int idxDespegue) {
    bool visitados[FILAS][COLUMNAS] = {false};
    int actualX = registros[idxDespegue].x;
    int actualY = registros[idxDespegue].y;
    int pasos = 0;
    bool finAlcanzado = false;

    while (true) {
        if (visitados[actualX][actualY]) {
            cout << "Error de validacion: La ruta pasa por las mismas coordenadas mas de una vez.\n";
            return false;
        }
        visitados[actualX][actualY] = true;
        pasos++;

        int idxActual = -1;
        for (size_t i = 0; i < registros.size(); i++) {
            if (registros[i].x == actualX && registros[i].y == actualY) {
                idxActual = i;
                break;
            }
        }

        if (idxActual == -1) {
            cout << "Error de validacion: Hueco en la ruta o coordenada no encontrada.\n";
            return false;
        }

        if (registros[idxActual].aterrizaje || registros[idxActual].ataqueKamikaze) {
            finAlcanzado = true;
            break;
        }

        int sigX = registros[idxActual].siguientex;
        int sigY = registros[idxActual].siguientey;

        if (abs(sigX - actualX) > 1 || abs(sigY - actualY) > 1) {
            cout << "Error de validacion: Celdas no aledañas detectadas en el movimiento.\n";
            return false;
        }

        actualX = sigX;
        actualY = sigY;
    }

    if (!finAlcanzado || pasos != registros.size()) {
        cout << "Error de validacion: La ruta no esta completa o contiene registros huerfanos.\n";
        return false;
    }
    return true;
}


// operacion 1: cargar archivo de ataque en memoria
void cargarArchivo() {
    string ruta;
    cout << "Ingrese la ruta y nombre del archivo de ataque (ej. ataque.dat): ";
    cin >> ruta;

    ifstream archivo(ruta, ios::binary);
    if (!archivo) {
        cout << "Error: No se pudo abrir el archivo.\n";
        return;
    }

    vector<OrdenArchivo> registros;
    OrdenArchivo reg;
    while (archivo.read((char*)&reg, sizeof(OrdenArchivo))) {
        registros.push_back(reg);
    }
    archivo.close();

    int idxDespegue = -1;


    if (!validarCantidadesBasicas(registros, idxDespegue)) return;
    if (!validarRutaContinua(registros, idxDespegue)) return;

    inicializarMemoria();
    for (const auto& r : registros) {
        memoria[r.x][r.y].espera = r.espera;
        memoria[r.x][r.y].soltarGranada1 = r.soltarGranada1;
        memoria[r.x][r.y].soltarGranada2 = r.soltarGranada2;
        memoria[r.x][r.y].ataqueKamikaze = r.ataqueKamikaze;
        memoria[r.x][r.y].aterrizaje = r.aterrizaje;
        memoria[r.x][r.y].despegue = r.despegue;
        memoria[r.x][r.y].siguientex = r.siguientex;
        memoria[r.x][r.y].siguientey = r.siguientey;
        memoriaOcupada[r.x][r.y] = true;
    }
    cout << "Archivo cargado en memoria exitosamente.\n";
}

// operacion 2: mostrar ataque cargado
void mostrarAtaque() {
    int contadorLineas = 0;
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            if (memoriaOcupada[i][j]) {
                cout << "Celda [" << i << "][" << j << "] - "
                     << "Desp:" << memoria[i][j].despegue << " "
                     << "Ater:" << memoria[i][j].aterrizaje << " "
                     << "Kmkz:" << memoria[i][j].ataqueKamikaze << " "
                     << "G1:" << memoria[i][j].soltarGranada1 << " "
                     << "G2:" << memoria[i][j].soltarGranada2 << " "
                     << "SigX:" << memoria[i][j].siguientex << " "
                     << "SigY:" << memoria[i][j].siguientey << "\n";
                contadorLineas++;

                // Evitar exceder tamaño de pantalla
                if (contadorLineas >= 15) {
                    cout << "\n--- Presione ENTER para continuar a la siguiente pagina ---\n";
                    cin.ignore();
                    cin.get();
                    contadorLineas = 0;
                }
            }
        }
    }
    cout << "\nFin del registro de ataques en memoria.\n";
}

// operacion 3: crear un archivo de ataque nuevo
void crearArchivo() {
    string ruta;
    cout << "Ruta y nombre del archivo a crear (ej. nuevo.dat): ";
    cin >> ruta;

    ofstream archivo(ruta, ios::binary);
    if (!archivo) {
        cout << "Error al crear archivo.\n";
        return;
    }

    char continuar = 's';
    while (tolower(continuar) == 's') {
        OrdenArchivo reg;
        cout << "Ingrese Coordenada X: "; cin >> reg.x;
        cout << "Ingrese Coordenada Y: "; cin >> reg.y;
        cout << "Espera (0 o ms): "; cin >> reg.espera;
        cout << "Soltar Granada 1 (1=Si, 0=No): "; cin >> reg.soltarGranada1;
        cout << "Soltar Granada 2 (1=Si, 0=No): "; cin >> reg.soltarGranada2;
        cout << "Ataque Kamikaze (1=Si, 0=No): "; cin >> reg.ataqueKamikaze;
        cout << "Aterrizaje (1=Si, 0=No): "; cin >> reg.aterrizaje;
        cout << "Despegue (1=Si, 0=No): "; cin >> reg.despegue;
        cout << "Siguiente X: "; cin >> reg.siguientex;
        cout << "Siguiente Y: "; cin >> reg.siguientey;

        archivo.write((char*)&reg, sizeof(OrdenArchivo));

        cout << "¿Desea cargar otro registro? (s/n): ";
        cin >> continuar;
    }
    archivo.close();
    cout << "Archivo creado exitosamente.\n";
}

// operacion 4: corregir un registro del archivo
void corregirRegistroArchivo() {
    string ruta;
    cout << "Ruta del archivo a corregir: ";
    cin >> ruta;

    fstream archivo(ruta, ios::binary | ios::in | ios::out);
    if (!archivo) {
        cout << "Error al abrir el archivo.\n";
        return;
    }

    int buscarX, buscarY;
    cout << "Ingrese X del registro a corregir: "; cin >> buscarX;
    cout << "Ingrese Y del registro a corregir: "; cin >> buscarY;

    OrdenArchivo reg;
    bool encontrado = false;
    long pos = 0;

    while (archivo.read((char*)&reg, sizeof(OrdenArchivo))) {
        if (reg.x == buscarX && reg.y == buscarY) {
            encontrado = true;
            cout << "\nRegistro actual encontrado:\n";
            cout << "SiguienteX: " << reg.siguientex << " SiguienteY: " << reg.siguientey << "\n";

            cout << "\n--- Ingrese nuevos datos ---\n";
            cout << "Espera: "; cin >> reg.espera;
            cout << "Soltar Granada 1 (1/0): "; cin >> reg.soltarGranada1;
            cout << "Soltar Granada 2 (1/0): "; cin >> reg.soltarGranada2;
            cout << "Ataque Kamikaze (1/0): "; cin >> reg.ataqueKamikaze;
            cout << "Aterrizaje (1/0): "; cin >> reg.aterrizaje;
            cout << "Despegue (1/0): "; cin >> reg.despegue;
            cout << "Siguiente X: "; cin >> reg.siguientex;
            cout << "Siguiente Y: "; cin >> reg.siguientey;

            archivo.seekp(pos, ios::beg);
            archivo.write((char*)&reg, sizeof(OrdenArchivo));
            cout << "Registro actualizado exitosamente.\n";
            break;
        }
        pos = archivo.tellg();
    }
    if (!encontrado) cout << "Registro no encontrado en el archivo.\n";
    archivo.close();
}

// operacion 5: corregir un registro en memoria
void corregirRegistroMemoria() {
    int buscarX, buscarY;
    cout << "Ingrese X de la memoria a corregir: "; cin >> buscarX;
    cout << "Ingrese Y de la memoria a corregir: "; cin >> buscarY;

    if (buscarX < 0 || buscarX >= FILAS || buscarY < 0 || buscarY >= COLUMNAS) {
        cout << "Coordenadas fuera de limites.\n";
        return;
    }

    cout << "\n--- Ingrese nuevos datos para celda [" << buscarX << "][" << buscarY << "] ---\n";
    cout << "Espera: "; cin >> memoria[buscarX][buscarY].espera;
    cout << "Soltar Granada 1 (1/0): "; cin >> memoria[buscarX][buscarY].soltarGranada1;
    cout << "Soltar Granada 2 (1/0): "; cin >> memoria[buscarX][buscarY].soltarGranada2;
    cout << "Ataque Kamikaze (1/0): "; cin >> memoria[buscarX][buscarY].ataqueKamikaze;
    cout << "Aterrizaje (1/0): "; cin >> memoria[buscarX][buscarY].aterrizaje;
    cout << "Despegue (1/0): "; cin >> memoria[buscarX][buscarY].despegue;
    cout << "Siguiente X: "; cin >> memoria[buscarX][buscarY].siguientex;
    cout << "Siguiente Y: "; cin >> memoria[buscarX][buscarY].siguientey;

    memoriaOcupada[buscarX][buscarY] = true;
    cout << "Memoria actualizada.\n";
}

// operacion 6: guardar memoria en un archivo nuevo
void guardarMemoria() {
    string ruta;
    cout << "Ruta y nombre para exportar la memoria: ";
    cin >> ruta;

    ofstream archivo(ruta, ios::binary);
     if (!archivo) {
        cout << "Error al crear archivo.\n";
        return;
    }

    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            if (memoriaOcupada[i][j]) {
                OrdenArchivo reg;
                reg.x = i;
                reg.y = j;
                reg.espera = memoria[i][j].espera;
                reg.soltarGranada1 = memoria[i][j].soltarGranada1;
                reg.soltarGranada2 = memoria[i][j].soltarGranada2;
                reg.ataqueKamikaze = memoria[i][j].ataqueKamikaze;
                reg.aterrizaje = memoria[i][j].aterrizaje;
                reg.despegue = memoria[i][j].despegue;
                reg.siguientex = memoria[i][j].siguientex;
                reg.siguientey = memoria[i][j].siguientey;

                archivo.write((char*)&reg, sizeof(OrdenArchivo));
            }
        }
    }
    archivo.close();
    cout << "Memoria guardada con exito.\n";
}

// operacion 7: visualizar un archivo de ataque en html
void generarHTML() {
    FILE* archivo = fopen("visualizacion_dron.html", "w");
    if (archivo == NULL) {
        cout << "No se pudo crear el archivo HTML.\n";
        return;
    }

    fprintf(archivo, "<!DOCTYPE html>\n<html>\n");
    fprintf(archivo, "<head><title>Ataque del Dron</title>\n");
    fprintf(archivo, "<style>td { width: 20px; height: 20px; text-align: center; font-size: 10px; }</style>\n");
    fprintf(archivo, "</head>\n<body>\n");
    fprintf(archivo, "<h2>Mapa de Ataque - Dron (200x200)</h2>\n");
    fprintf(archivo, "<table border=\"1\" cellspacing=\"0\">\n");

    for (int fila = 0; fila < FILAS; fila++) {
        fprintf(archivo, "<tr>\n");
        for (int columna = 0; columna < COLUMNAS; columna++) {
            if (memoriaOcupada[fila][columna]) {
                // Precedencia para mostrar marcador segun reglas del PDF
                if (memoria[fila][columna].despegue) {
                    fprintf(archivo, "<td style='background-color: yellow;'>D</td>");
                } else if (memoria[fila][columna].aterrizaje) {
                    fprintf(archivo, "<td style='background-color: lightgreen;'>A</td>");
                } else if (memoria[fila][columna].soltarGranada1 || memoria[fila][columna].soltarGranada2) {
                    fprintf(archivo, "<td style='background-color: orange;'>G</td>");
                } else if (memoria[fila][columna].ataqueKamikaze) {
                    fprintf(archivo, "<td style='background-color: red;'>K</td>");
                } else if (memoria[fila][columna].espera > 0) {
                    fprintf(archivo, "<td style='background-color: lightblue;'>E</td>");
                } else {
                    fprintf(archivo, "<td style='background-color: lightgray;'>M</td>"); // Movimiento
                }
            } else {

                fprintf(archivo, "<td> </td>");
            }
        }
        fprintf(archivo, "</tr>\n");
    }

    fprintf(archivo, "</table>\n</body>\n</html>\n");
    fclose(archivo);
    cout << "Archivo visualizacion_dron.html generado con exito en la ruta del programa.\n";
}

int main() {
    inicializarMemoria();
    int opcion = 0;

    do {
        cout << "\n ---MENU LOGICA DRON MILITAR--- \n";
        cout << "1. Cargar archivo de ataque en memoria\n";
        cout << "2. Mostrar ataque cargado\n";
        cout << "3. Crear un archivo de ataque nuevo\n";
        cout << "4. Corregir un registro del archivo\n";
        cout << "5. Corregir un registro en memoria\n";
        cout << "6. Guardar memoria en un archivo nuevo\n";
        cout << "7. Visualizar un archivo de ataque en html\n";
        cout << "8. Salir.\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch(opcion) {
            case 1: cargarArchivo(); break;
            case 2: mostrarAtaque(); break;
            case 3: crearArchivo(); break;
            case 4: corregirRegistroArchivo(); break;
            case 5: corregirRegistroMemoria(); break;
            case 6: guardarMemoria(); break;
            case 7: generarHTML(); break;
            case 8: cout << "Saliendo...\n"; break;
            default: cout << "Opcion no valida.\n";
        }
    } while (opcion != 8);

    return 0;
}

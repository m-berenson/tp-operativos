

#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include <protocolo.h>
#include <cliente.h>
#include <servidor.h>
#include <shared.h>
#include <pcb.h>
#include <paquetes.h>
#include <memoria_shared.h>

#include "memoria_principal.h"
#include "interfaz.h"
#include "swap.h"

void manejar_comunicacion(void* void_args);
int server_memoria(char* server_name, int server_socket);

#endif /* COMUNICACION_H_ */

/*
 * comunicacion.h
 *
 *  Created on: Sep 29, 2022
 *      Author: ubuntu
 */

#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include <protocolo.h>
#include <cliente.h>
#include <shared.h>

void manejar_comunicacion(int cliente_socket);
int server_escuchar(char* server_name, int server_socket);
int conectar_cpu(char* ip, char* puerto);

#endif /* COMUNICACION_H_ */

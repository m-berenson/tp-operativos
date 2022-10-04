/*
 * comunicacion.c
 *
 *  Created on: Sep 29, 2022
 *      Author: ubuntu
 */

#include "../include/comunicacion.h"

extern t_log* logger;

void manejar_comunicacion(void* void_args) {

		t_manejar_conexion_args* args = (t_manejar_conexion_args*) void_args;
		int cliente_socket = args->fd;
		char* server_name = args->server_name;
		free(args);

		 // Mientras la conexion este abierta
	    while (cliente_socket != -1) {
			int cod_op = recibir_operacion(cliente_socket);

			switch (cod_op) {
			case ELEMENTOS_CONSOLA:
				// Parsear segmentos
				break;
			case DEBUG:
				log_debug(logger, "Estoy debuggeando!");
				break;
			case -1:
				log_error(logger, "El cliente se desconecto. Terminando servidor");
				return;
			default:
				log_warning(logger,"Operacion desconocida. No quieras meter la pata");
				break;
			}
	    }

	    log_warning(logger, "El cliente se desconecto de %s server", server_name);
	    return;
}

int server_escuchar(char* server_name, int server_socket) {
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

    if (cliente_socket != -1) {
        pthread_t hilo;
        t_manejar_conexion_args* args = malloc(sizeof(t_manejar_conexion_args));
        args->fd = cliente_socket;
        args->server_name = server_name;
        pthread_create(&hilo, NULL, (void*) manejar_comunicacion, (void*) args);
        pthread_detach(hilo);
        return 1;
    }

    return 0;
}

int conectar_cpu(char* ip, char* puerto) {
	log_info(logger, "Iniciando conexion con la CPU - Puerto: %s - IP: %s", ip, puerto);
	return crear_conexion(ip, puerto);
}

int conectar_memoria(char* ip, char* puerto) {
	log_info(logger, "Iniciando conexion con la Memoria - Puerto: %s - IP: %s", ip, puerto);
	return crear_conexion(ip, puerto);
}

#include "../include/comunicacion.h"

extern t_log* logger;
extern t_configuracion_kernel* config;
int proccess_counter = 0;

sem_t pantalla_consola;
sem_t teclado_consola;
extern sem_t planificar;

uint32_t respuesta_teclado;

void manejar_comunicacion(void* void_args) {
	t_manejar_conexion_args* args = (t_manejar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char* server_name = args->server_name;
	free(args);

	// De manera provisional
	sem_init(&pantalla_consola, 0, 0);
	sem_init(&teclado_consola, 0, 0);

	// Mientras la conexion este abierta
	while (cliente_socket != -1) {
		int cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) {
		case ELEMENTOS_CONSOLA:
			PCB *pcb = crear_pcb(cliente_socket);
		
			nuevoProceso(pcb);
			break;
		case FIN_POR_EXIT: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			log_pcb(pcb);
			pasarAExit(pcb);
			break;
		}

		case OP_DISCO: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			int unidades_de_trabajo = recibir_operacion(cliente_socket);

			int *dispo1 = malloc(sizeof(int) * 2);
			dispo1 = list_get(config->tiempos_io, 0);

			pasarABlock(pcb, DISCO);
			
			int tiempo_de_suspension = dispo1[1] * unidades_de_trabajo / 1000;
			log_trace(logger, "DISCO");
			log_trace(logger, "SLEEP DE %d", tiempo_de_suspension);
			ejecutar_suspension_en_hilo(pcb, tiempo_de_suspension);
			break;
		}

		case OP_IMPRESORA: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			int unidades_de_trabajo = recibir_operacion(cliente_socket);

			int *impresora = malloc(sizeof(int) * 2); 
			impresora = list_get(config->tiempos_io, 1);
			
			pasarABlock(pcb, IMPRESORA);

			int tiempo_de_suspension = impresora[1] * unidades_de_trabajo / 1000;
			log_trace(logger, "IMPRESORA");
			log_trace(logger, "SLEEP DE %d", tiempo_de_suspension);

			ejecutar_suspension_en_hilo(pcb, tiempo_de_suspension);

			break;
		}

		case OP_PANTALLA: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			int reg = recibir_operacion(cliente_socket);
			
			//log_pcb(pcb);
			pasarABlock(pcb, PANTALLA);

			enviar_codop(pcb->socket_consola, OP_PANTALLA);
			enviar_codop(pcb->socket_consola, pcb->registros[reg]);
			
			sem_wait(&pantalla_consola);
			log_debug(logger, "Consola Envio el mensaje %d", respuesta_teclado);

			pasarAReady(pcb);
			break;
		}

		case RESPUESTA_PANTALLA: {
			sem_post(&pantalla_consola);
			break;
		}

		case OP_TECLADO: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			reg_cpu reg = recibir_operacion(cliente_socket);
			
			pasarABlock(pcb, TECLADO);
			log_debug(logger, "Enviando codigo de operacion: OP_TECLADO");

			enviar_codop(pcb->socket_consola, OP_TECLADO);
			log_debug(logger, "Esperando respuesta del teclado.");

			sem_wait(&teclado_consola);
			log_debug(logger, "Despues de la respuesta del teclado.");

			pcb->registros[reg] = respuesta_teclado;
			log_debug(logger, "[registro: %d, respuesta_teclado: %d]", reg, respuesta_teclado);

			log_pcb(pcb);
			pasarAReady(pcb);

			break;
		}

		case RESPUESTA_TECLADO: {
			respuesta_teclado = recibir_operacion(cliente_socket);
			sem_post(&teclado_consola);

			break;
		}

		case DEBUG:
			log_debug(logger, "Estoy debuggeando!");
			break;

		case DESCONEXION_CONTROLADA:
			log_info(logger, "El cliente se desconecto de manera esperada");
			close(cliente_socket);
			return;

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

void suspender_proceso(void* void_args ) {
	t_manejar_block* args = (t_manejar_block*) void_args;
	int tiempo = args->tiempo_de_suspension;
	PCB* pcb = args->pcb;
	free(args);

	sleep(tiempo);

	pasarAReady(pcb);
}

void ejecutar_suspension_en_hilo(PCB* pcb, int tiempo) {
    pthread_t hilo;

    t_manejar_block* args = malloc(sizeof(t_manejar_block));
    args->pcb = pcb;
    args->tiempo_de_suspension = tiempo;

    pthread_create(&hilo, NULL, (void*) suspender_proceso, (void*) args);
    pthread_detach(hilo);
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

int conectar_con(char *servername, char *ip, char *puerto) {
	log_info(logger, "Iniciando conexion con %s - Puerto: %s - IP: %s", ip, puerto, servername);

	int file_descriptor = crear_conexion(ip, puerto);

	if(file_descriptor == -1) {
		log_info(logger, "No se ha podido conectar %s", servername);
		exit(EXIT_FAILURE);
	}

	return file_descriptor;
}

PCB* crear_pcb(int cliente_socket) {
	t_list *listas = recibir_paquete(cliente_socket);

	void* ins = list_get(listas, 0);
	void* seg = list_get(listas, 1);

	t_list *lista_ins = deserializar_lista_inst(ins);
	t_list *lista_segm = deserializar_lista_segm(seg);

	PCB *pcb = nuevoPcb(proccess_counter, cliente_socket, lista_ins, lista_segm);
	
	proccess_counter++;

	return pcb;
}

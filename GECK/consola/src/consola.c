#include "../include/consola.h"

t_log* logger;

// TODO enviar los mensajes a kernel

int main(int argc, char** argv) {
	char* config_path;
	char* pseudo_path;
	t_config* config;
	t_list *lista_inst;
	t_list *lista_segmentos;
	int tiempo_pantalla;
	int kernel_fd;

	// Level trace para que hagamos logs debugs y trace
	// Así los logs minimos quedan al level info
	logger = log_create("Consola.log", "logger", true, LOG_LEVEL_TRACE);

	if (argc < 3) {
		log_error(logger, "Se necesitan mas argumentos para inicializar correctamente la consola");
		return EXIT_FAILURE;
	}

	config_path = argv[1];
	pseudo_path = argv[2];

	log_debug(logger, "config: %s", config_path);
	log_debug(logger, "pseudo: %s", pseudo_path);

	lista_inst = parsear_pseudocod(pseudo_path);	
	config = procesar_config(config_path, &lista_segmentos, &tiempo_pantalla);
	kernel_fd = connect_to_kernel(config);

	enviar_proceso(kernel_fd, lista_inst, lista_segmentos);

	return liberar_memoria(logger, kernel_fd, config);
}

t_list *char_to_int(char **segmentos) {
	t_list *lista_segmentos = list_create();

	for (int i = 0; segmentos[i] != NULL; i++)
		list_add(lista_segmentos, atoi(segmentos[i]));

	log_debug(logger, "cant segmentos: %d", list_size(lista_segmentos));

	return lista_segmentos;
}

t_config* procesar_config(char *config_path, t_list **lista_segmentos, int* tiempo_pantalla) {
	char **segmentos;
	t_config* config = config_create(config_path);

	if (config== NULL) {
		log_error(logger, "No se pudo abrir el archivo de configuracion en ese path");
		exit(EXIT_FAILURE);
	}

	*tiempo_pantalla = config_get_int_value(config, "TIEMPO_PANTALLA");
	segmentos = config_get_array_value(config, "SEGMENTOS");

	*lista_segmentos = char_to_int(segmentos);

	log_info(logger, "tiempo_pantalla: %d", *tiempo_pantalla);

	return config;
}

int connect_to_kernel(t_config* config) {
	char* ip = config_get_string_value(config, "IP_KERNEL");
	char* puerto = config_get_string_value(config, "PUERTO_KERNEL");

	log_info(logger, "Iniciando conexion con el Kernel - Puerto: %s - IP: %s", ip, puerto);
	int kernel_fd = crear_conexion(ip, puerto);

	if(kernel_fd == -1) {
		log_error(logger, "No se ha podido iniciar la conexion con el kernel");
		exit(EXIT_FAILURE);
	}

	log_debug(logger, "kernel file descriptor: %d", kernel_fd);

	return kernel_fd;
}

void *serializar_proceso(t_list* lista_inst, t_list* lista_segmentos, int *size_stream) {
	ts_ins *instruccion;
	int *segmento;

	int desplazamiento = 0;
	int size_lista_inst = list_size(lista_inst);
	int size_lista_segmentos = list_size(lista_segmentos);
	
	*size_stream = size_lista_inst + size_lista_segmentos + (sizeof(int) * 2);
	void* stream = malloc(*size_stream);

	// Tamaño de la lista de instrucciones
	memcpy(stream + desplazamiento, &size_lista_inst, sizeof(int)); 
	desplazamiento += sizeof(int);

	// Lista de instrucciones
	for(int i = 0; i < size_lista_inst; i++) {
		instruccion = list_get(lista_inst, i);
		memcpy(stream + desplazamiento, &instruccion, sizeof(instruccion));
		desplazamiento += sizeof(int);
	}

	list_clean(lista_inst);
	list_destroy(lista_inst);

	// Tamaño de la lista de segmentos
	memcpy(stream + desplazamiento, &size_lista_segmentos, sizeof(int)); 
	desplazamiento += sizeof(int);

	// Lista de segmentos
	for(int i = 0; i < size_lista_segmentos; i++) {
		segmento = list_get(lista_segmentos, i);
		memcpy(stream + desplazamiento, &segmento, sizeof(instruccion));
		desplazamiento += sizeof(int);
	}

	list_clean(lista_segmentos);
	list_destroy(lista_segmentos);

	return stream;
}

void enviar_proceso(int kernel_fd, t_list* lista_inst, t_list* lista_segmentos) {
	ts_paquete proceso;
	int size_stream;
	int bytes_mensaje;

	proceso.code = ELEMENTOS_CONSOLA;
	proceso.stream = serializar_proceso(lista_inst, lista_segmentos, &size_stream);
	proceso.size = sizeof(proceso.stream); // poner size_stream

	bytes_mensaje = sizeof(op_code) + sizeof(int) + proceso.size;

	// enviar_paquete;
}

int liberar_memoria(t_log* logg, int fd, t_config* config) {
	liberar_conexion(fd);
	config_destroy(config);
	log_destroy(logg);
	return EXIT_SUCCESS;
}

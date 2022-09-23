#include "../include/servidor.h"

extern t_log* logger;

// TODO comprobar que las syscall devuelvan >= 0 sino tirar error
int iniciar_servidor(char* ip, char* puerto) {
	int socket_servidor;

	struct addrinfo condiciones, *servinfo;

	// pone 0 a los sizeof(hints) caracteres de hints
	memset(&condiciones, 0, sizeof(condiciones));

	condiciones.ai_family = AF_UNSPEC; // No importa si IPv4 o IPv6
	condiciones.ai_socktype = SOCK_STREAM;
	condiciones.ai_flags = AI_PASSIVE; // Flag para que use el IP de este dispositivo

	// Como está definido un IP no se tiene en cuenta la flag passive
	getaddrinfo(ip, puerto, &condiciones, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(
			servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol
	);

	// si se interrumpe la ejecucion no tengo que esperar 2min para reutilizar ese socket
	// en ese ip y puerto
	int activado = 1;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	log_trace(logger, "KERNEL escuchando en la IP: %s PUERTO: %s", ip, puerto);

	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN); // hay que cambiar SOMAXCONN por el grado de multiprogramacion

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para recibir consolas");

	return socket_servidor;
}

// quizas haya que modificarla para usar hilos
int esperar_cliente(int server_fd) {
	int cliente_fd = accept(server_fd, NULL, NULL);
	log_info(logger, "Se ha conectado un cliente");

	return cliente_fd;
}

int recibir_operacion(int socket_cliente) {
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

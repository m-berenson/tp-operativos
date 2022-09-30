#include "../include/shared.h"

void liberar_conexion(int socket_cliente) {
	close(socket_cliente);
}

bool config_has_all_properties(t_config* cfg, char** properties) {
    for(uint8_t i = 0; properties[i] != NULL; i++) {
        if(!config_has_property(cfg, properties[i]))
            return false;
    }

    return true;
}

char* leer_archivo_completo(char* path) {
    FILE* fp = fopen(path, "r+");

    if(fp == NULL)
        return NULL;

    fseek(fp, 0L, SEEK_END);
    long lSize = ftell(fp);
    rewind(fp);

    char* text = calloc(1, lSize + 1);
    fread(text, lSize, 1, fp);
    fclose(fp);

    return text;
}

t_config* abrir_configuracion(char* path)
{
	t_config* config;

	config = config_create(path);

	return config;
}
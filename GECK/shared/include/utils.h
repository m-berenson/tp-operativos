/*
 * utils.h
 *
 *  Created on: Sep 21, 2022
 *      Author: ubuntu
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>

bool config_has_all_properties(t_config*, char**);
char* leer_archivo_completo(char*);
t_config* abrir_configuracion(char* path);

#endif /* UTILS_H_ */

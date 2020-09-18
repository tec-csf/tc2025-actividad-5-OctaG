/*
  Octavio Garduza
  A01021660

  Actividad 5

  Implementación en c de un token ring que pasa un char testigo entre los
  procesos que lo conforman utilizando memoria dinámica, fork() y pipes.

  Para poder hacer esta tarea me ayudó Sabrina Santana, quien me apoyó
  explicandome que debías tener una pipe por cada proceso, que las pipes debían
  ser almacenadas dentro de un arreglo dinámico y los pasos para
  escribir y leer el testigo en cada procesos hijo/padre.
*/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//Aquí se almacena la info de cada pipe
typedef struct{
  int *tubo;
}pipe_t;

int main(int argc, char* const *argv) {

  pid_t pid;

  pipe_t *pipes_head;
  pipe_t *pipes_tail;

  char testigo_base = 'T';
  char testigo;

  int c; //Aux para almacenar cada opt en argv
  int nvalue = 0; //Default 1 (sin argumento -n)

  //Mientras existan chars en argv
  while ((c = getopt (argc, argv, "n:")) != -1){
    if(c == 'n'){//Exito
      nvalue = atoi(optarg);//Asegura que el argumento es un int
    }else if(c == '?'){//Errores
      if (optopt == 'n')
        fprintf(stderr, "Opción -%c requiere un argumento.\n", optopt);
      else if(isprint(optopt))
        fprintf(stderr, "Opción desconocida '-%c'.\n", optopt);
      else
        fprintf(stderr, "Opción desconocida '\\x%x'.\n", optopt);
      return 1;
    }
    else{
      abort();
    }
  }

  //Por cada arg invalido pasado
  for (int index = optind; index < argc; index++){
    printf ("El argumento no es una opción válida %s\n", argv[index]);
    return(1);
  }

  //Si nvalue no es int o es menor/igual a 0
  if(nvalue <= 0){
    printf ("El valor de -n debe ser un int mayor a 0\n");
    return(1);
  }

  pipes_head = (pipe_t *) malloc(sizeof(pipe_t) * nvalue);
  pipes_tail = pipes_head + nvalue;

  printf("\n\n");

  //Crea n pipes
  for(pipe_t *aux = pipes_head; aux < pipes_tail; ++aux){
    aux->tubo = (int *) malloc(sizeof(int) * 2);
    pipe(aux->tubo);
  }

  //Crea n procesos
  for(pipe_t *aux = pipes_head; aux < pipes_tail-1; ++aux){
    pid = fork();
    //Ambos procesos ejecutan
    if(pid == -1){//Error
      printf("Error al crear el proceso hijo\n");
      break; //No crea el resto de los procesos
    }
    else if(pid == 0){//Hijo
      while(1){
        close(*(aux->tubo)+1);
        read(*(aux->tubo), &testigo, sizeof(char));
        printf("—-> Soy el proceso con PID %d y recibí el testigo %c, el cual tendré por 5 segundos\n\n", getpid(), testigo);
        sleep(5);
        printf("<—- Soy el proceso con PID %d y acabo de enviar el testigo %c\n\n", getpid(), testigo);
        close((*(aux+1)->tubo));
        write(*((aux+1)->tubo)+1, &testigo, sizeof(char));
      }
      exit(0);
    }
  }
  while(1){
    close(*pipes_head->tubo);
    printf("—-> Soy el proceso con PID %d y acabo de enviar el testigo %c\n\n", getpid(), testigo_base);
    write(*(pipes_head->tubo)+1, &testigo_base, sizeof(char));
    close((*(pipes_tail-1)->tubo)+1);
    read(*((pipes_tail-1)->tubo), &testigo, sizeof(char));
    printf("<—- Soy el proceso con PID %d y recibí el testigo %c, el cual tendré por 5 segundos\n\n", getpid(), testigo);
    sleep(5);
  }

  //Liberar memoria
  for(pipe_t *aux = pipes_head; aux < pipes_tail; ++aux){
    free(aux->tubo );
  }
  free(pipes_head);
  return 0;
}//Fin de main

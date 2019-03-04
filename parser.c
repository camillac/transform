#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"

// parse_args
//  arguments:
//    char * line : line of input from the terminal
//    char * c    : the character the line is being separated by
//  return value:
//    char ** - returns string array of each argument in the line
//  what it does:
//    separates each argument in a string and returns all the arguments as a string array
char ** parse_args(char * line, char * c){
  char ** array = calloc(256, sizeof(char*));

  int x = 0;

  char * token;
  while (line && x < 256){
    token = strsep(&line, c);
    if (strcmp(token,"")) {
      array[x] = token;
      x++;
    }
  }

  return array;
}

/*======== void parse_file () ==========
Inputs:   char * filename
          struct matrix * transform,
          struct matrix * pm,
          screen s
Returns:

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         line: add a line to the edge matrix -
               takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
         ident: set the transform matrix to the identity matrix -
         scale: create a scale matrix,
                then multiply the transform matrix by the scale matrix -
                takes 3 arguments (sx, sy, sz)
         translate: create a translation matrix,
                    then multiply the transform matrix by the translation matrix -
                    takes 3 arguments (tx, ty, tz)
         rotate: create a rotation matrix,
                 then multiply the transform matrix by the rotation matrix -
                 takes 2 arguments (axis, theta) axis should be x y or z
         apply: apply the current transformation matrix to the edge matrix
         display: clear the screen, then
                  draw the lines of the edge matrix to the screen
                  display the screen
         save: clear the screen, then
               draw the lines of the edge matrix to the screen
               save the screen to a file -
               takes 1 argument (file name)
         quit: end parsing

See the file script for an example of the file format

IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename,
                  struct matrix * transform,
                  struct matrix * edges,
                  screen s) {

  FILE *f;
  char line[256];
  color c;
  clear_screen(s);

  c.green = 256;
  c.red = 0;
  c.blue = 0;

  if ( strcmp(filename, "stdin") == 0 )
    f = stdin;
  else
    f = fopen(filename, "r");

  double * vals;

  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    // printf(":%s:\n",line);

    // === LINE ===
    if (strcmp(line, "line") == 0){
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      // printf(":%s:\n",line);
      // string array of vals
      vals = calloc(6, sizeof(double));
      char ** array =  parse_args(line, " ");
      for (int i = 0; i < 6; i++){
        vals[i] = atof(array[i]);
      }
      add_edge(edges, vals[0], vals[1], vals[2], vals[3], vals[4], vals[5]);
      free(array);
      free(vals);
    }

    // === IDENT ===
    else if (strcmp(line, "ident") == 0){
      ident(transform);
    }

    // === SCALE ===
    else if (strcmp(line, "scale") == 0){
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      // printf(":%s:\n",line);
      // string array of vals
      vals = calloc(3, sizeof(double) == 0);
      char ** array =  parse_args(line, " ");
      for (int i = 0; i < 3; i++){
        vals[i] = atof(array[0]);
      }
      struct matrix * scale = make_scale(vals[0], vals[1], vals[2]);
      matrix_mult(scale, transform);

      free(array);
      free(vals);
    }

    // === TRANSLATE ===
    else if (strcmp(line, "move") == 0){
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      // printf(":%s:\n",line);
      // string array of vals
      vals = calloc(3, sizeof(double));
      char ** array =  parse_args(line, " ");
      for (int i = 0; i < 3; i++){
        vals[i] = atof(array[i]);
      }
      struct matrix * move = make_translate(vals[0], vals[1], vals[2]);
      matrix_mult(move, transform);

      free(array);
      free(vals);
    }

    // === ROTATE ===
    else if (strcmp(line, "rotate") == 0){
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      // printf(":%s:\n",line);
      // string array of vals
      vals = calloc(1, sizeof(double));
      char ** array =  parse_args(line, " ");
      vals[0] = atof(array[1]);

      struct matrix * rotate;
      if (strcmp(array[0], "x") == 0){
        rotate = make_rotX(vals[0]);
      }
      else if (strcmp(array[0], "y") == 0){
        rotate = make_rotY(vals[0]);
      }
      else if (strcmp(array[0], "z") == 0){
        rotate = make_rotZ(vals[0]);
      }
      else {
        printf("BAD AXIS\n");
      }

      matrix_mult(rotate, transform);

      free(array);
      free(vals);
    }

    // === APPLY ===
    else if (strcmp(line, "apply") == 0){
      matrix_mult(transform, edges);
    }

    // === DISPLAY ===
    else if (strcmp(line, "display") == 0){
      clear_screen(s);
      draw_lines(edges, s, c);
    }

    // === SAVE ===
    else if (strcmp(line, "save") == 0){
      clear_screen(s);
      fgets(line, 255, f);
      line[strlen(line)-1]='\0';
      // printf(":%s:\n",line);

      draw_lines(edges, s, c);

      char ** array =  parse_args(line, " ");
      // printf("SAVE: %s\n", array[0]);
      save_extension(s, array[0]);
    }

    // === QUIT ===
    else if (strcmp(line, "quit") == 0){
      exit(0);
    }

    // === ??? ===
    else {
      printf("NOT A PROPER COMMAND");
    }
  }
}

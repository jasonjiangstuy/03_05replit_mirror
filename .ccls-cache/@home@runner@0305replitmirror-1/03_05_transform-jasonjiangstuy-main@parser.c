#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"


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
  clear_screen(s);

  if ( strcmp(filename, "stdin") == 0 )
    f = stdin;
  else
    f = fopen(filename, "r");


  char prevline[256] = "";
   // if true, then take in next line to finish the command left from prev line
  int waiting_for_para = 0;

  while ( fgets(line, 255, f) != NULL ) {

    line[strlen(line)-1]='\0';
    printf(":%s:\n",line);

    if (waiting_for_para == 1){
      // printf("finding paras for :%s:\n", prevline);
      waiting_for_para = 0;
      //storing 10 char pointers
      char *paras[10];
      char * cur = line;
      char *prev = cur;
      int i = 0;
      for (; strtok_r(cur, " ", &cur); i++){
        // printf("%s", prev);
        paras[i] = prev;
        prev = cur;
      }
      // for functions that need inputs
      if (strcmp(prevline, "line") == 0){
        printf("Drawing Line\n");
        int i = 0;
        for (; i < 6; i++){
          printf("%s", paras[i]);
        }
        add_edge(edges,
          atoi(paras[0]),atoi(paras[1]),atoi(paras[2]),
          atoi(paras[3]),atoi(paras[4]),atoi(paras[5]));
      }else if (strcmp(prevline, "scale") == 0){
        struct matrix * scale = make_scale(atoi(paras[0]),atoi(paras[1]),atoi(paras[2]));
        matrix_mult(scale, transform);
      }else if (strcmp(prevline, "move") == 0){
        struct matrix * translation = make_translate(atoi(paras[0]),atoi(paras[1]),atoi(paras[2]));
        matrix_mult(translation, transform);
      }else if (strcmp(prevline, "rotate") == 0){
        printf("paras[0]: %s", paras[0]);
        struct matrix * rotation;
        if (strcmp(paras[0], "x")==0){
          rotation = make_rotX(atoi(paras[1]));
        }else if (strcmp(paras[0], "y")==0){
          rotation = make_rotY(atoi(paras[1]));
        }else if (strcmp(paras[0], "z")==0){
          rotation = make_rotZ(atoi(paras[1]));
        }else{
          printf("ERROR, BAD INPUT");
        }
        matrix_mult(rotation, transform);
      }else if (strcmp(prevline, "save") == 0){
        clear_screen(s);
        color c;
        c.red = 0;
        c.green = 45;
        c.blue = 187;
        draw_lines(edges, s, c);
        save_extension(s, paras[0]);
      }


    }else{
  // functions that dont have parameters
      if (strcmp(line, "ident") == 0)
        ident(transform);
      else if(strcmp(line, "print") == 0)
        print_matrix(transform);
      else if(strcmp(line, "apply") == 0)
        matrix_mult(transform, edges);
      else if(strcmp(line, "display") == 0){
        print_matrix(edges);
        clear_screen(s);
        color c;
        c.red = 0;
        c.green = 45;
        c.blue = 187;
        draw_lines(edges, s, c);
        display(s);
      }
      else if(strcmp(line, "quit") == 0)
        return;
      else{
        strcpy(prevline, line);
        waiting_for_para = 1;
      }
    }


  }
}

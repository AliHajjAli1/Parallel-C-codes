#include <stdio.h>
#include <time.h>
#include <omp.h>

#define WIDTH 640
#define HEIGHT 480
#define MAX_ITER 255

struct complex{
  double real;
  double imag;
};


int cal_pixel(struct complex c) {
    

            double z_real = 0;
            double z_imag = 0;

            double z_real2, z_imag2, lengthsq;

            int iter = 0;
            do {
                z_real2 = z_real * z_real;
                z_imag2 = z_imag * z_imag;

                z_imag = 2 * z_real * z_imag + c.imag;
                z_real = z_real2 - z_imag2 + c.real;
                lengthsq =  z_real2 + z_imag2;
                iter++;
            }
            while ((iter < MAX_ITER) && (lengthsq < 4.0));

            return iter;

}

void save_pgm(const char *filename, int image[HEIGHT][WIDTH]) {
    FILE* pgmimg; 
    int temp;
    pgmimg = fopen(filename, "wb"); 
    fprintf(pgmimg, "P2\n"); // Writing Magic Number to the File   
    fprintf(pgmimg, "%d %d\n", WIDTH, HEIGHT);  // Writing Width and Height
    fprintf(pgmimg, "255\n");  // Writing the maximum gray value 
    int count = 0; 
    
    for (int i = 0; i < HEIGHT; i++) { 
        for (int j = 0; j < WIDTH; j++) { 
            temp = image[i][j]; 
            fprintf(pgmimg, "%d ", temp); // Writing the gray values in the 2D array to the file 
        } 
        fprintf(pgmimg, "\n"); 
    } 
    fclose(pgmimg); 
} 


int main() {
    int image[HEIGHT][WIDTH];
    double AVG = 0;
    int N = 10; // number of trials
    double total_time[N];
    int t=4;
    omp_set_num_threads(t);

    for (int k=0; k<N; k++){
      double start_time = omp_get_wtime(); // Start measuring time
      #pragma omp parallel for schedule(dynamic)
      for (int i = 0; i < HEIGHT; i++) {
      	  struct complex c;
          for (int j = 0; j < WIDTH; j++) {
              c.real = (j - WIDTH / 2.0) * 4.0 / WIDTH;
              c.imag = (i - HEIGHT / 2.0) * 4.0 / HEIGHT;
              image[i][j] = cal_pixel(c);
          }
      }

      double end_time = omp_get_wtime(); // End measuring time

      total_time[k] = (end_time - start_time);
      printf("Execution time : %f seconds\n" , total_time[k]);
      AVG += total_time[k];
    }

    save_pgm("mandelbrot2.pgm", image);
    printf("The average execution time of 10 trials is: %f ms with %d threads", AVG/N*1000,t);
    return 0;
}
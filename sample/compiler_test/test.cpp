#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ctime>

#define Na 16384
#define Nb 16384
#define _T 4096
#define DTYPE double

using namespace std;

int main(){
  DTYPE in[Na]={1.23,}, out[Nb]={0.0,};
  DTYPE **w;
  srand (static_cast <unsigned> (time(0)));

  // init
  w = new DTYPE*[Nb];
  for (int i=0; i<Nb; i++) {
     w[i] = new DTYPE[Na];
  }

  for (int i=0; i<Nb; i++){
     for (int j=0; j<Na; j++){
         w[i][j] = static_cast <DTYPE> (rand()) / (static_cast <DTYPE> (RAND_MAX));
     }
  }

  #ifdef ORIGIN
  // origin
  chrono::system_clock::time_point start = chrono::system_clock::now();
  for (int i=0; i<Nb; i++){
    for (int j=0; j<Na; j++){
      out[i] += w[j][i] * in[j];
    }
  }
  chrono::duration<double> time = chrono::system_clock::now() - start;
  // printf("origin-%d: %f\n", _T, time.count());
  fprintf(stderr, "out: %f\n", out[104]);
  printf("%f\n", time.count());
  #endif
  
  #ifdef INVERT
  // inverted
  chrono::system_clock::time_point start = chrono::system_clock::now();
  for (int j=0; j<Na; j++){
     for (int i=0; i<Nb; i++){
       out[i] += w[j][i] * in[j];
     }
   }
  chrono::duration<double> time = chrono::system_clock::now() - start;
  // printf("invert-%d: %f\n", _T, time.count());
  fprintf(stderr, "out: %f\n", out[104]);
  printf("%f\n", time.count());
  #endif

  #ifdef TILED
  // tiled
  chrono::system_clock::time_point start = chrono::system_clock::now();
  for (int j=0; j<(Na/_T); j++){
    for (int i=0; i<Nb; i++){
      for (int jj=j*_T; jj<(j*_T+_T); jj++){
        out[i] += w[jj][i] * in[jj];
      }
    }
  }
  chrono::duration<double> time = chrono::system_clock::now() - start;
  // printf("tiled-%d: %f\n", _T, time.count());
  fprintf(stderr, "out: %f\n", out[104]);
  printf("%f\n", time.count());
  #endif

  #ifdef INV_TILED_INNER
  // inverted + tiled
  chrono::system_clock::time_point start = chrono::system_clock::now();
  for (int i=0; i<(Nb/_T); i++){
    for (int j=0; j<Na; j++){
      for (int ii=i*_T; ii<(i*_T+_T); ii++){
        out[ii] += w[j][ii] * in[j];
      }
    }
  }
  chrono::duration<double> time = chrono::system_clock::now() - start;
  // printf("it_in-%d: %f\n", _T, time.count());
  fprintf(stderr, "out: %f\n", out[104]);
  printf("%f\n", time.count());
  #endif

  #ifdef INV_TILED_OUTER
  // inverted + tiled
  chrono::system_clock::time_point start = chrono::system_clock::now();
  for (int j=0; j<(Na/_T); j++){
    for (int jj=j*_T; jj<(j*_T+_T); jj++){
      for (int i=0; i<Nb; i++){
        out[i] += w[jj][i] * in[jj];
      }
    }
  }
  chrono::duration<double> time = chrono::system_clock::now() - start;
  // printf("it_out-%d: %f\n", _T, time.count());
  fprintf(stderr, "out: %f\n", out[104]);
  printf("%f\n", time.count());
  #endif

  // correctness
  // printf("in: %f, out: %f, w: %f\n", in[0], out[0], w[9][9]);

  for (int i=0; i<Nb; i++) {
     delete w[i];
  }
  delete w;

  return 0;
}

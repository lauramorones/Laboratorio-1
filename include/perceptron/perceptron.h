#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#define MAX_INPUTS 5
#define MAX_ROWS   32

extern int N_inputs;
extern float w[MAX_INPUTS+1];

int Detect_Pots();
void Read_Pots(int X[]);
void Generate_TruthTable(int X[][MAX_INPUTS]);
void Apply_DesiredOutput(int y[], int filas, unsigned long decimalSalida);
void Train_Perceptron(int X[][MAX_INPUTS], int y[], int filas);
int Perceptron_Predict(int X[]);

#endif

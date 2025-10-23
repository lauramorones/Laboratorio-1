#ifndef ML_LAYER_H
#define ML_LAYER_H

#include <Arduino.h>

// === Selección del modelo ===
//#define USE_KMEANS   // Descomenta para probar K-Means
#define USE_SVM        // Descomenta para probar SVM

// --- Parámetros del modelo K-Means ---
#if defined(USE_KMEANS)
static const float kmeans_centroids[2][3] = {
  {-0.678771f, -0.679626f, -0.689349f},
  { 0.720757f,  0.721665f,  0.731989f}
};
#endif

// --- Parámetros del modelo SVM ---
#if defined(USE_SVM)
static const float svm_weights[1][3] = {
  {-1.291496f, -1.373001f, -1.334645f}
};
static const float svm_bias[1] = {0.252633f};
#endif

// --- Normalización (StandardScaler del Colab) ---
static const float means[3] = {2.554935f, 2.548705f, 2.511487f};
static const float stds[3]  = {1.407006f, 1.408995f, 1.437407f};

// --- Prototipo de función ---
int ML_Predict(float pot1, float pot2, float pot3);

#endif

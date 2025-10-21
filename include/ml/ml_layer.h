#ifndef ML_LAYER_H
#define ML_LAYER_H

#include <Arduino.h>

// === Selección del modelo ===
//#define USE_KMEANS
#define USE_SVM

// --- Parámetros K-Means ---
#if defined(USE_KMEANS)
static const float kmeans_centroids[2][3] = {
  {-0.678771f, -0.679626f, -0.689349f},
  { 0.720757f,  0.721665f,  0.731989f}
};
#endif

// --- Parámetros SVM ---
#if defined(USE_SVM)
static const float svm_weights[1][3] = {
  {-1.291496f, -1.373001f, -1.334645f}
};
static const float svm_bias[1] = {0.252633f};
#endif

// Normalización (si usaste StandardScaler en Colab)
static const float means[3] = {0.0, 0.0, 0.0};
static const float stds[3]  = {1.0, 1.0, 1.0};

// Prototipo
int ML_Predict(float pot1, float pot2, float pot3);

#endif

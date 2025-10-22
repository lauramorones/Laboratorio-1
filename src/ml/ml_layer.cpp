#include "ml/ml_layer.h"
#include <math.h>

int ML_Predict(float pot1, float pot2, float pot3) {
  // Normalización (igual que en Colab)
  float x1 = (pot1 - means[0]) / stds[0];
  float x2 = (pot2 - means[1]) / stds[1];
  float x3 = (pot3 - means[2]) / stds[2];

#if defined(USE_SVM)
  // --- Modelo SVM ---
  float f = svm_weights[0][0]*x1 +
            svm_weights[0][1]*x2 +
            svm_weights[0][2]*x3 +
            svm_bias[0];

  Serial.print("SVM -> f = ");
  Serial.println(f, 6);

  return (f > 0) ? 1 : 0;  // 1 = LED ON, 0 = LED OFF

#elif defined(USE_KMEANS)
  // --- Modelo K-Means ---
  float d0 = sqrt(pow(x1 - kmeans_centroids[0][0], 2) +
                  pow(x2 - kmeans_centroids[0][1], 2) +
                  pow(x3 - kmeans_centroids[0][2], 2));
  float d1 = sqrt(pow(x1 - kmeans_centroids[1][0], 2) +
                  pow(x2 - kmeans_centroids[1][1], 2) +
                  pow(x3 - kmeans_centroids[1][2], 2));

  Serial.print("KMeans -> d0: ");
  Serial.print(d0, 6);
  Serial.print(" d1: ");
  Serial.println(d1, 6);

  return (d1 > d0) ? 1 : 0;  // LED ON si pertenece al clúster 1
#endif
}


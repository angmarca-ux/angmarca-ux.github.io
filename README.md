# Muestreo del movimiento de un Brazo Robótico

Este repositorio contiene la entrega del proyecto final de Métodos Numéricos enfocado en el análisis cinemático de un brazo mecánico.

## Contenido del Proyecto
* **[Informe Técnico (PDF)](./Informe_Brazo_Robotico.pdf)**
* **Código de Adquisición de Datos:** [ProyectoMétodos.ino](./ProyectoMétodos.ino)
* **Código de Procesamiento:** [programa.m](./programa.m)
* **Datos Registrados:** [datos.txt](./datos.txt)
* **Evidencia en Video:** [Proyecto Métodos 2.mp4](./Proyecto%20Métodos%202.mp4)

## Resumen del Proyecto

El proyecto inició con la adquisición de datos espaciales $(X, Y, Z)$ del movimiento de un brazo mecánico mediante un Arduino, capturando muestras cada 0.5 segundos. Para mitigar el ruido eléctrico de los sensores y los errores de lectura cuando el mecanismo estaba estático, se realizó una depuración matricial en limpio. Esto permitió aislar únicamente las coordenadas dinámicas donde existía un desplazamiento físico real, evitando que la distancia calculada se inflara artificialmente.

Con la lista de datos limpia, se implementó el análisis cinemático en MATLAB/Octave. Primero, se aplicó el método de **Diferencias Finitas Centrales** para calcular las velocidades instantáneas en cada eje y determinar la magnitud de la velocidad lineal total del brazo. Posteriormente, se utilizó la **Regla del Trapecio Compuesto** para integrar numéricamente el perfil de velocidades, calculando de forma exacta la distancia real recorrida bajo el área de la curva.

Finalmente, los resultados se estructuraron de forma visual mediante gráficas compuestas por una trayectoria tridimensional en el espacio 3D y un perfil temporal de velocidad que evidencia las aceleraciones y desaceleraciones del sistema. Toda la arquitectura del proyecto fue cargada y organizada en este repositorio público de GitHub, garantizando un control de versiones limpio y una presentación formal para la evaluación académica.

---
**Estudiante:** Huaman Marca Angel Joel  
**Curso:** Métodos Numéricos  
**Año:** 2026

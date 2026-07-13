clear; clc; close all;

% 1. CARGA INSTANTÁNEA DESDE EL ARCHIVO TXT
% Octave lee el archivo en milisegundos directamente del servidor
datos_brutos = load('datos.txt');

% Configuración del paso de tiempo:
% -> Usa h = 0.5 si el Arduino envía datos cada 500ms
% -> Usa h = 1.0 si cambiaste el delay del Arduino a 1000ms
h = 0.5; 

% ELIMINACIÓN DE RUIDO ESTÁTICO (Evita que la distancia se infle)
cambio_posicion = [true; sum(abs(diff(datos_brutos)), 2) > 0.05];
datos_utiles = datos_brutos(cambio_posicion, :);

X = datos_utiles(:, 1);
Y = datos_utiles(:, 2);
Z = datos_utiles(:, 3);

N = length(X);
tiempo = (0:N-1)' * h;

% ====================================================================
% TEMA 1: DIFERENCIACIÓN NUMÉRICA VECTORIAL 
% ====================================================================
vx = zeros(N, 1); vy = zeros(N, 1); vz = zeros(N, 1);

vx(2:end-1) = (X(3:end) - X(1:end-2)) / (2 * h);
vy(2:end-1) = (Y(3:end) - Y(1:end-2)) / (2 * h);
vz(2:end-1) = (Z(3:end) - Z(1:end-2)) / (2 * h);

vx(1) = (X(2) - X(1)) / h;         vx(end) = (X(end) - X(end-1)) / h;
vy(1) = (Y(2) - Y(1)) / h;         vy(end) = (Y(end) - Y(end-1)) / h;
vz(1) = (Z(2) - Z(1)) / h;         vz(end) = (Z(end) - Z(end-1)) / h;

V_total = sqrt(vx.^2 + vy.^2 + vz.^2);

% ====================================================================
% TEMA 2: INTEGRACIÓN NUMÉRICA (Método del Trapecio Compuesto)
% ====================================================================
suma_trapecios = V_total(1) + V_total(end) + 2 * sum(V_total(2:end-1));
distancia_trapecio = (h / 2) * suma_trapecios;

% Imprimir reporte formal limpio en consola
fprintf('==================================================\n');
fprintf('       INFORME DE CÁLCULO MÉTODOS NUMÉRICOS       \n');
fprintf('==================================================\n');
fprintf('  Número de nodos dinámicos (n): %d\n', N);
fprintf('  Paso de integración (h):        %.2f segundos\n', h);
fprintf('--------------------------------------------------\n');
fprintf('  Distancia Total Real (Trapecio): %.2f cm\n', distancia_trapecio);
fprintf('==================================================\n');

% ====================================================================
% PLOTS: PRESENTACIÓN VISUAL EN DOS PANELES
% ====================================================================
figure('Position', [50, 50, 1100, 480]);

% Sub-Gráfica 1: Espacio 3D
subplot(1, 2, 1);
plot3(X, Y, Z, '-o', 'LineWidth', 2.5, 'MarkerSize', 5, 'Color', [0 0.5 0.8]);
grid on; hold on;
plot3(X(1), Y(1), Z(1), 'gs', 'MarkerSize', 10, 'MarkerFaceColor', 'g');  
plot3(X(end), Y(end), Z(end), 'rd', 'MarkerSize', 10, 'MarkerFaceColor', 'r'); 
axis equal;
title('Trayectoria Espacial Corregida (Eje Z al Frente)');
xlabel('Eje X (cm)'); ylabel('Eje Y (cm)'); zlabel('Eje Z (cm)');
view(60, 25); 
legend('Recorrido', 'Inicio (A)', 'Fin (B)', 'Location', 'southoutside', 'Orientation', 'horizontal');

% Sub-Gráfica 2: Gráfica de Velocidad
subplot(1, 2, 2);
plot(tiempo, V_total, '-r', 'LineWidth', 2.2);
grid on;
title('Perfil de Velocidad (Diferencias Finitas)');
xlabel('Tiempo (Segundos)');
ylabel('Velocidad Lineal (cm/s)');
legend('Velocidad del Brazo', 'Location', 'southoutside');
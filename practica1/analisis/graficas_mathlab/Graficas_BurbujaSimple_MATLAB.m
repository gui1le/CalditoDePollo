% PRACTICA 01 - Aproximaciones de burbuja simple (tiempo real)
% Coloca este script junto a tu bitacora TXT en la misma carpeta de MATLAB.
% IMPORTANTE: Si el TXT tiene otro nombre, cambia SOLO la siguiente linea.
clear; clc; close all;
nombreArchivo = 'tiempos_burbujaSimple.txt';

if ~isfile(nombreArchivo)
    error('No encuentro %s en la carpeta actual. Comprueba el nombre con dir *.txt.', nombreArchivo);
end
texto = fileread(nombreArchivo);

% Extraer pares N / tiempo REAL. No es necesario borrar user, sys ni CPU/Wall.
patron = 'N consultadas:\s*(\d+)[\s\S]*?real\s*\(Tiempo total\)\s*:\s*([0-9.]+)\s*s';
datos = regexp(texto, patron, 'tokens');
if isempty(datos)
    error('No se encontraron pares N/real. Revisa el formato de tu TXT.');
end
x = cellfun(@(dato) str2double(dato{1}), datos);
y = cellfun(@(dato) str2double(dato{2}), datos);
x = x(:)'; y = y(:)';
[x, indice] = sort(x);
y = y(indice);
fprintf('Se leyeron %d mediciones de %s.\n', numel(x), nombreArchivo);
if numel(x) ~= 21
    warning('La practica contempla 21 tamanos; revisa que no falten mediciones.');
end

% t=N/1e6: escalar N para mejorar estabilidad numerica en polyfit.
t = x / 1e6;
xi = linspace(min(x), max(x), 600);
ti = xi / 1e6;
grados = [1 2 3 6];
R2 = zeros(size(grados));
coeficientes = cell(size(grados));

for k = 1:numel(grados)
    grado = grados(k);
    p = polyfit(t, y, grado);
    coeficientes{k} = p;
    valoresAjustados = polyval(p, t);
    R2(k) = 1 - sum((y - valoresAjustados).^2) / sum((y - mean(y)).^2);
    z = polyval(p, ti);

    figure(k); clf;
    plot(x, y, 'ko', xi, z, '-', 'LineWidth', 1.5);
    grid on;
    xlabel('Tamano del problema (N)');
    ylabel('Tiempo real (segundos)');
    title(sprintf('Burbuja simple: aproximacion polinomial grado %d', grado));
    legend('Datos experimentales', sprintf('Grado %d', grado), 'Location', 'northwest');
    saveas(gcf, sprintf('BurbujaSimple_grado_%d.png', grado));

    fprintf('Grado %d | R^2 = %.9f | coeficientes en t=N/1e6: ', grado, R2(k));
    fprintf(' %.10g', p);
    fprintf('\n');
end

% Una grafica comparando todos los ajustes con los puntos experimentales.
figure(5); clf;
plot(x, y, 'ko', 'MarkerSize', 5, 'MarkerFaceColor', 'k');
hold on;
for k = 1:numel(grados)
    plot(xi, polyval(coeficientes{k}, ti), 'LineWidth', 1.5);
end
hold off; grid on;
xlabel('Tamano del problema (N)');
ylabel('Tiempo real (segundos)');
title('Burbuja simple: comparativa de aproximaciones');
legend('Datos experimentales', 'Grado 1', 'Grado 2', 'Grado 3', 'Grado 6', 'Location', 'northwest');
saveas(gcf, 'BurbujaSimple_comparativa.png');

% Grado 2: candidato para modelar burbuja simple; validar con tus graficas.
p2 = coeficientes{2};
nFuturo = [3000000 4000000 5000000 6000000 10000000];
yFuturo = polyval(p2, nFuturo/1e6);
fprintf('\nPredicciones con grado 2 (extrapolaciones, no mediciones):\n');
for k = 1:numel(nFuturo)
    fprintf('N=%d -> %.3f segundos (%.2f minutos)\n', nFuturo(k), yFuturo(k), yFuturo(k)/60);
end

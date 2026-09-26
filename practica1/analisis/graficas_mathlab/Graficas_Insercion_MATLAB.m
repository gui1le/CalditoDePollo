% PRACTICA 01 - Aproximaciones de Insercion (tiempo real)
% Coloca este script y el archivo Insercion.txt en la misma carpeta.
% Si tu TXT tiene otro nombre, cambialo en la linea siguiente.
clear; clc; close all;
nombreArchivo = 'Insercion.txt';
texto = fileread(nombreArchivo);

% Extraer N y tiempo REAL de cada bloque de la bitacora.
patron = 'N consultadas:\s*(\d+)[\s\S]*?real\s*\(Tiempo total\)\s*:\s*([0-9.]+)\s*s';
datos = regexp(texto, patron, 'tokens');
if isempty(datos)
    error('No se encontraron pares N/real. Revisa el nombre y formato del TXT.');
end
x = cellfun(@(dato) str2double(dato{1}), datos);
y = cellfun(@(dato) str2double(dato{2}), datos);
x = x(:)'; y = y(:)';
[x, indice] = sort(x);
y = y(indice);
fprintf('Se leyeron %d mediciones de %s.\n', numel(x), nombreArchivo);

% Escala para estabilidad numerica: t=1 significa un millon de elementos.
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
    predEnMedidos = polyval(p, t);
    R2(k) = 1 - sum((y - predEnMedidos).^2) / sum((y - mean(y)).^2);
    z = polyval(p, ti);

    figure(k); clf;
    plot(x, y, 'ko', xi, z, '-', 'LineWidth', 1.5);
    grid on;
    xlabel('Tamano del problema (N)');
    ylabel('Tiempo real (segundos)');
    title(sprintf('Insercion: aproximacion polinomial grado %d', grado));
    legend('Datos experimentales', sprintf('Grado %d', grado), 'Location', 'northwest');
    saveas(gcf, sprintf('Insercion_grado_%d.png', grado));

    fprintf('Grado %d | R^2 = %.9f | coeficientes en t=N/1e6: ', grado, R2(k));
    fprintf(' %.10g', p);
    fprintf('\n');
end

% Comparativa de las 4 aproximaciones de Insercion en UNA grafica.
figure(5); clf;
plot(x, y, 'ko', 'MarkerSize', 5, 'MarkerFaceColor', 'k');
hold on;
for k = 1:numel(grados)
    plot(xi, polyval(coeficientes{k}, ti), 'LineWidth', 1.5);
end
hold off; grid on;
xlabel('Tamano del problema (N)');
ylabel('Tiempo real (segundos)');
title('Insercion: comparativa de aproximaciones');
legend('Datos experimentales', 'Grado 1', 'Grado 2', 'Grado 3', 'Grado 6', 'Location', 'northwest');
saveas(gcf, 'Insercion_comparativa.png');

% Para Insercion, modelo de grado 2 como candidato de extrapolacion.
p2 = coeficientes{2};
nFuturo = [3000000 4000000 5000000 6000000 10000000];
yFuturo = polyval(p2, nFuturo/1e6);
fprintf('\nPredicciones con grado 2 (extrapolaciones, no mediciones):\n');
for k = 1:numel(nFuturo)
    fprintf('N=%d -> %.3f segundos (%.2f minutos)\n', nFuturo(k), yFuturo(k), yFuturo(k)/60);
end

import os
import shutil
import pathlib
import collections

# Generar un nombre de directorio único para evitar sobreescribir
def f_directorio_salida(base_dir):
    contador_val = 1
    directorio_salida = base_dir

    # Buscar un nombre que no exista aún
    while pathlib.Path(directorio_salida).exists():
        directorio_salida = f"{base_dir} ({contador_val})"
        contador_val += 1

    return directorio_salida

# Agrupar archivos por carpeta contenedora
def f_agrupar_por_carpeta(directorio_base, extension_val):
    archivos_por_carpeta = collections.defaultdict(list)
    directorio_base = pathlib.Path(directorio_base)

    # Buscar recursivamente archivos con la extensión especificada
    for archivo in directorio_base.rglob(f"*.{extension_val}"):
        if archivo.is_file():
            carpeta = archivo.parent
            archivos_por_carpeta[carpeta].append(archivo)

    return archivos_por_carpeta

# Copiar y renombrar archivos en cada carpeta con su propio contador
def f_copiar_archivos(directorio_base, archivos_por_carpeta, directorio_salida, extension_val):
    total_archivos = 0
    directorio_base = pathlib.Path(directorio_base)
    directorio_salida = pathlib.Path(directorio_salida)

    # Procesar cada carpeta que contiene archivos
    for carpeta_origen, archivos in archivos_por_carpeta.items():
        cantidad = len(archivos)
        ancho_numeracion = len(str(cantidad))

        # Calcular ruta relativa y crear la carpeta destino
        ruta_relativa = carpeta_origen.relative_to(directorio_base)
        carpeta_destino = directorio_salida / ruta_relativa
        carpeta_destino.mkdir(parents=True, exist_ok=True)

        # Copiar cada archivo con nombre nuevo
        for i, archivo in enumerate(archivos, start=1):
            nombre_archivo = f"{str(i).zfill(ancho_numeracion)}.{extension_val}"
            destino = carpeta_destino / nombre_archivo
            shutil.copy2(archivo, destino)
            total_archivos += 1

    # Devolver la cantidad total de archivos procesados
    return total_archivos

# Bucle principal infinito
while True:
    # Solicitar el directorio de entrada
    while True:
        directorio_base = input("Enter directory: ").strip()

        if pathlib.Path(directorio_base).exists():
            break

        print("Wrong directory")

    # Solicitar la extensión de los archivos a procesar
    extension_val = input("Enter file extension: ").strip()

    # Agrupar los archivos por carpeta
    archivos_por_carpeta = f_agrupar_por_carpeta(directorio_base, extension_val)

    # Crear directorio de salida único
    directorio_salida = f_directorio_salida("Output files")
    pathlib.Path(directorio_salida).mkdir()

    # Copiar y renombrar los archivos
    contador_archivos = f_copiar_archivos(directorio_base, archivos_por_carpeta, directorio_salida, extension_val)

    # Mostrar resumen
    print("------------------------------------")

    if contador_archivos == 0:
        print("No modified files")
    elif contador_archivos == 1:
        print("1 modified file")
    else:
        print(f"{contador_archivos} modified files")

    print("------------------------------------\n")
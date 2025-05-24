#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <map>

// Generar un nombre de directory único para evitar sobreescribir
std::string f_directorio_salida()
{
    int contador_val = 1;
    std::string directorio_salida = "";

    while (std::filesystem::exists(directorio_salida))
    {
        directorio_salida = "Output files (" + std::to_string(contador_val) + ")";
        
        contador_val++;
    }

    return directorio_salida;
}

// Agrupar archivos por carpeta
std::map<std::filesystem::path, std::vector<std::filesystem::directory_entry>> f_agrupar_por_carpeta(
    const std::string &directorio_capt,
    const std::string &extension_val)
{
    std::map<std::filesystem::path, std::vector<std::filesystem::directory_entry>> archivos_por_carpeta;

    for (const std::filesystem::directory_entry &entrada : std::filesystem::recursive_directory_iterator(directorio_capt))
    {
        if (entrada.is_regular_file() && entrada.path().extension() == ("." + extension_val))
        {
            archivos_por_carpeta[entrada.path().parent_path()].push_back(entrada);
        }
    }

    return archivos_por_carpeta;
}

// Copiar y renombrar archivos en cada carpeta con su propio contador
int f_copiar_archivos(const std::string &directorio_capt,
    const std::map<std::filesystem::path, std::vector<std::filesystem::directory_entry>> &archivos_por_carpeta,
    const std::string &directorio_salida,
    const std::string &extension_val)
{
    int total_archivos = 0;

    for (const std::pair<const std::filesystem::path, std::vector<std::filesystem::directory_entry>> &par : archivos_por_carpeta)
    {
        const std::filesystem::path &carpeta_origen = par.first;
        const std::vector<std::filesystem::directory_entry> &archivos = par.second;
        
        // Cantidad de directorios de archivos de la carpeta
        int cantidad = archivos.size();
        // Largo de dígitos del número de cantidad de archivos en carpeta
        int ancho_numeracion = std::to_string(cantidad).length();

        // Obtiene la ruta relativa de la carpeta origen respecto al directorio base
        std::string ruta_relativa = std::filesystem::relative(carpeta_origen, directorio_capt).string();
        // Construye la ruta completa de destino combinando el directorio salida con la ruta relativa
        std::filesystem::path carpeta_destino = std::filesystem::path(directorio_salida) / ruta_relativa;
        
        // Crea la estructura de directorios de destino (incluyendo subcarpetas si no existen)
        std::filesystem::create_directories(carpeta_destino);
        
        // Itera sobre cada archivo en la carpeta actual
        for (int i = 0; i < cantidad; ++i)
        {
            // Nuevo nombre de archivo
            std::ostringstream nombre_archivo;
            
            // Genera nombre con formato de ceros
            nombre_archivo << std::setfill('0') << std::setw(ancho_numeracion) << (i + 1) << "." << extension_val;
        
            // Construye la ruta completa de destino
            std::filesystem::path destino = carpeta_destino / nombre_archivo.str();
        
            // Copia el archivo original al destino con nuevo nombre
            std::filesystem::copy_file(archivos[i].path(), destino, std::filesystem::copy_options::overwrite_existing);
            
            total_archivos++;
        }
    }

    return total_archivos;
}

// Bucle principal
int main()
{
    while (true)
    {
        // directory de entrada 
        std::string directorio_capt;

        while (true)
        {
            std::cout << "Enter directory: ";
            std::getline(std::cin, directorio_capt);

            if (std::filesystem::exists(directorio_capt))
            {
                break;
            }

            std::cout << "Wrong directory\n";
        }

        std::string extension_val;

        std::cout << "Enter file extension: ";
        std::getline(std::cin, extension_val);
        
        // Obtener directorio de carpeta y directorio de archivos en un mismo objeto
        std::map<std::filesystem::path, std::vector<std::filesystem::directory_entry>> archivos_por_carpeta = f_agrupar_por_carpeta(directorio_capt, extension_val);

        // Obtener directory de salida
        std::string directorio_salida = f_directorio_salida();
        
        // Crear directory de salida
        std::filesystem::create_directory(directorio_salida);
        
        // Contar, copiar y renombrar archivos
        int contador_archivos = f_copiar_archivos(directorio_capt, archivos_por_carpeta, directorio_salida, extension_val);

        std::cout << "------------------------------------\n";

        if (contador_archivos == 0)
        {
            // Eliminar directory
            std::filesystem::remove_all(directorio_salida);
            
            std::cout << "No modified files\n";
        }
        else if (contador_archivos == 1)
        {
            std::cout << "1 modified file\n";
        }
        else
        {
            std::cout << contador_archivos << " modified files\n";
        }
        std::cout << "------------------------------------\n\n";
    }

    return 0;
}

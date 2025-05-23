#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <map>

// Generar un nombre de directorio único para evitar sobreescribir
std::string f_directorio_salida(const std::string& base_dir)
{
    int contador_val = 1;
    std::string directorio_salida = base_dir;

    while (std::filesystem::exists(directorio_salida))
    {
        directorio_salida = base_dir + " (" + std::to_string(contador_val) + ")";
        contador_val++;
    }

    return directorio_salida;
}

// Agrupar archivos por carpeta contenedora
std::map<std::filesystem::path, std::vector<std::filesystem::directory_entry>> f_agrupar_por_carpeta(
    const std::string& directorio_base,
    const std::string& extension_val)
{
    std::map<std::filesystem::path, std::vector<std::filesystem::directory_entry>> archivos_por_carpeta;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(directorio_base))
    {
        if (entry.is_regular_file() && entry.path().extension() == ("." + extension_val))
        {
            archivos_por_carpeta[entry.path().parent_path()].push_back(entry);
        }
    }

    return archivos_por_carpeta;
}

// Copiar y renombrar archivos en cada carpeta con su propio contador
int f_copiar_archivos(const std::string& directorio_base,
    const std::map<std::filesystem::path, std::vector<std::filesystem::directory_entry>>& archivos_por_carpeta,
    const std::string& directorio_salida,
    const std::string& extension_val)
{
    int total_archivos = 0;

    for (const auto& par : archivos_por_carpeta)
    {
        const auto& carpeta_origen = par.first;
        const auto& archivos = par.second;

        int cantidad = archivos.size();
        int ancho_numeracion = std::to_string(cantidad).length();

        std::string ruta_relativa = std::filesystem::relative(carpeta_origen, directorio_base).string();
        std::filesystem::path carpeta_destino = std::filesystem::path(directorio_salida) / ruta_relativa;
        std::filesystem::create_directories(carpeta_destino);

        for (int i = 0; i < cantidad; ++i)
        {
            std::ostringstream nombre_archivo;
            nombre_archivo << std::setfill('0') << std::setw(ancho_numeracion) << (i + 1) << "." << extension_val;

            std::filesystem::path destino = carpeta_destino / nombre_archivo.str();

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
        // Directorio de entrada 
        std::string directorio_base;

        while (true)
        {
            std::cout << "Enter directory: ";
            std::getline(std::cin, directorio_base);

            if (std::filesystem::exists(directorio_base))
            {
                break;
            }

            std::cout << "Wrong directory\n";
        }

        std::string extension_val;

        std::cout << "Enter file extension: ";
        std::getline(std::cin, extension_val);

        auto archivos_por_carpeta = f_agrupar_por_carpeta(directorio_base, extension_val);

        // Directorio de salida
        std::string directorio_salida = f_directorio_salida("Output files");
        std::filesystem::create_directory(directorio_salida);

        int contador_archivos = f_copiar_archivos(directorio_base, archivos_por_carpeta, directorio_salida, extension_val);

        std::cout << "------------------------------------\n";

        if (contador_archivos == 0)
        {
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
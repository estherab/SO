#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination: pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int copynFile(FILE *origin, FILE *destination, int nBytes) {
	
    int numBytes = 0;
    int p;

    // En caso de error devuelve -1
    if ((origin == NULL) || (destination == NULL)) {
		numBytes = -1;
    }
  
    // Copiamos el contenido del fichero origen al destino, mientras no haya un error y no hayamos terminado
    while ((numBytes != nBytes) && (p != EOF)) {
		// Escribe p en el ﬁchero destination
		// Si tiene éxito devuelve el carácter escrito. En caso de error devuelve EOF
 		p = getc(origin);
		if (p != EOF) {
			putc(p, destination);
			numBytes++;
		}
		// Lee un carácter del ﬁchero origin
		// Tiene éxito si devuelve el carácter leído como unsigned char transformado a entero. En caso de error devuelve EOF.
    }
    
    return(numBytes);
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char* loadstr(FILE * file) {
	
	// Reservamos memoria
	// PATH_MAX = máximo número de bytes en un pathname
	char *cadena = (malloc(PATH_MAX * sizeof(char)));
	int i = 0;
	int c;

	// Mientras no haya error y no llegue al final de la cadena
	while (((c = getc(file)) != EOF) && ((char)c != '\0')){
		cadena[i] = c;
		i++;
	}
	
	// Si no ha habido error, devuelve 0
	if (i != 0) {
		// Devuelve algo distinto a NULL porque no ha habido error
		return cadena;
	} 
	
	else {
		// Devuelve NULL porque ha habido un error.
		return NULL;
	}
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry* readHeader(FILE * tarFile, int nFiles)  {
  
	int i = 0;  
    stHeaderEntry* array = NULL;

    // Reservamos memoria para el array
    array = malloc(sizeof(stHeaderEntry) * nFiles);

    // Leemos la metainformación del tarFile y la volcamos en el array
	// Usamos loadstr para cargar el nombre (p[i].name)
	// Leemos el tamaño del fichero y lo almacenamos en p[i].size
    while (i < nFiles) {
		array[i].name = loadstr(tarFile);
		puts(array[i].name);
		fread(&array[i].size,sizeof(int),1 ,tarFile);
		
		i++;
    }

    // Si no hemos podido leer completa la cabecera
    if (i != nFiles) {
		perror("No se ha podido leer la cabecera de los tar");
		array = NULL;
    }

   return array;
}

int fileSize(FILE * origin) {

    int prev = ftell(origin);
    fseek(origin, 0L, SEEK_END);
    int numBytes = ftell(origin);
    fseek(origin, prev, SEEK_SET);

	return numBytes;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int createTar(int nFiles, char *fileNames[], char tarName[]) {
	
	FILE *tarFile, *inputFile; // Creamos una variable donde guardar los ficheros tarFile e inputFile
	int i = 0;
	stHeaderEntry *header;
	int sizeHeader = 0;
	int sizeFile = 0;
	int totalSize = 0;
	
	// Abrimos el fichero mtar para escritura 
	tarFile = fopen(tarName,"wb");
	
	// Calculamos el tamaño total de la cabecera sin contar numfiles
	sizeHeader = sizeof(stHeaderEntry) * nFiles;

	// El array tendrá tantas posiciones como ficheros haya que introducir en el mtar
	header = malloc(sizeHeader);
	
	fseek(tarFile, 0, SEEK_SET);

	// Inicializamos el campo name de cada estructura stHeaderEntry
	for (i = 0; i < nFiles; i++) {
		
		if ((inputFile = fopen(fileNames[i], "r")) == NULL) {
			fprintf(stderr, "No se ha podido abrir el fichero %s\n", fileNames[i]);
			return(EXIT_FAILURE);
		}

	    // Completamos la cabecera en memoria (NO en el fichero)
		header[i].name = fileNames[i];
		puts(header[i].name);
		header[i].size = fileSize(inputFile); 

		sizeFile = sizeFile + fileSize(inputFile);

		if ((fclose(inputFile)) == EOF) {
			perror("No se ha podido cerrar el fichero fuente");
			return(EXIT_FAILURE);
		}		
	}
	
	totalSize = sizeFile + 1 + sizeHeader; // Tamaño total del archivo

	rewind(tarFile); // Coloca el puntero al inicio de todo
	fwrite(&nFiles,sizeof(int), 1, tarFile); // Escribimos el número de ficheros en el tar
	
	for (i = 0; i < nFiles; i++) {
		fwrite(header[i].name, sizeof(char), strlen(fileNames[i]), tarFile);
		fputc('\0', tarFile);
		fwrite(&header[i].size, sizeof(unsigned int), 1, tarFile);
	}

	free(header);

	for (i = 0; i < nFiles; i++) {

		if ((inputFile = fopen(fileNames[i], "r")) == NULL) {
			fprintf(stderr, "No se ha podido abrir el fichero %s\n", fileNames[i]);
			return(EXIT_FAILURE);
		}

		copynFile(inputFile, tarFile, 10000);
	}

	if ((fclose(tarFile)) == EOF) {
		perror("No se ha podido cerrar correctamente el tar");
		return(EXIT_FAILURE);
	}
	
	return(EXIT_SUCCESS);
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int extractTar(char tarName[]) {
	
	stHeaderEntry *header;
	FILE *tarFile, *outputFile;
	int j, nFiles;

	tarFile = fopen(tarName, "r");
	
	if (tarFile == NULL) {
		perror("No se ha podido abrir el fichero no existe el nombre de proyecto");
		return(EXIT_FAILURE);
	}
	
	// Leo y guardo el número de archivos a extraer
	fread(&nFiles,sizeof(int),1,tarFile);

	header = readHeader(tarFile,nFiles);
  
	// Creamos los ficheros contenidos en el mtar. Para ello recorremos secuencialmente la cabecera,
	// creando los ficheros indicados en cada descriptor y copiando en ellos su contenido
	int i;
	
	for (i = 0; i < nFiles; i++) {
		
		if ((outputFile = fopen(header[i].name, "w")) == NULL) {
			perror("No se ha podido abrir el fichero");
			return(EXIT_FAILURE);		
		}

		j = copynFile(tarFile, outputFile, header[i].size);

		if ((fclose(outputFile)) == EOF) {
			perror("No se ha podido cerrar el fichero en el que estamos copiando");
			return(EXIT_FAILURE);
		}
	}

	// Liberamos memoria de la cabecera y cerramos el tarball
	free(header);
	
	if ((fclose(tarFile)) == EOF) {
		perror("No se ha podido cerrar el fichero .mtar");
		return(EXIT_FAILURE);
	}
 
	return(EXIT_SUCCESS);
}

int extractTarDirectory(char *tarName, char *directory) {
	
	stHeaderEntry* header;
	FILE *tarFile, *outputFile;
	int j, nFiles;

	tarFile = fopen(tarName, "r");
	
	if (tarFile == NULL) {
		perror("No se ha podido abrir el fichero no existe el nombre de proyecto");
		return(EXIT_FAILURE);
	}
	
	// Leo y guardo el numero de archivos a extraer
	fread(&nFiles, sizeof(int), 1, tarFile);
	
	header = readHeader(tarFile, nFiles);
	
	char* ruta = malloc(strlen(directory) + strlen(tarName));
  
	// Creamos los ficheros contenidos en el mtar. Para ello recorremos secuencialmente la cabecera,
	// creando (abriendo en escritura) los ficheros indicados en cada uno de los descriptores y copiando
	// en ellos su contenido, que se leerá del tarball usando la función copynFile.
	int i;
	for (i = 0; i < nFiles; i++) {
	  	sprintf(ruta, "%s/%s", directory, header[i].name);

		if ((outputFile = fopen(ruta, "w")) == NULL) {
			perror("No se ha podido abrir el fichero");
			return(EXIT_FAILURE);		
		}

		j = copynFile(tarFile,outputFile,header[i].size);

		if ((fclose(outputFile)) == EOF) {
			perror("No se ha podido cerrar el fichero en el que estamos copiando");
			return(EXIT_FAILURE);
		}
	}

	// Liberamos memoria de la cabecera y cerramos el tarball
	free(header);
	
	if ((fclose(tarFile)) == EOF) {
		perror("No se ha podido cerrar el fichero .mtar");
		return(EXIT_FAILURE);
	}
 
	return(EXIT_SUCCESS);
}

int listTar(char* tarName) {
	int nFiles = 0, i = 0, j = 0, k = 0;
	stHeaderEntry *header, temp;
	FILE *tarFile;
	
	tarFile = fopen(tarName, "r");
	fread(&nFiles, sizeof(int), 1, tarFile);
	header = readHeader(tarFile, nFiles);
	
	if (tarFile == NULL) {
		perror("No se ha podido abrir el fichero no existe el nombre de proyecto");
		return(EXIT_FAILURE);
	}

	for (k = 1; k < nFiles; k++) {
		for (j = 0 ; j < nFiles - 1; j++) { 
			if (header[j].size < header[j+1].size) {
				temp = header[j];
				header[j] = header[j+1];
				header[j+1] = temp;
			}
		} 
	}	

	for (i = 0; i < nFiles; i++) {
		printf("File name: %s ,",header[i].name);
		printf(" size: %d ",header[i].size);
		printf(" bytes\n");
	}
	
	return(EXIT_SUCCESS);	
}

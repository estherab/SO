#!/bin/sh

# Creamos el directorio temporal
rm -r copiasTemporales
mkdir copiasTemporales

#a) Copiamos dos archivos (que ocupan más de un bloque) a nuestro SF y al fichero temporal SF
cp ./src/fuseLib.c ./copiasTemporales
cp ./src/fuseLib.c ./mount-point

cp ./src/myFS.h ./copiasTemporales
cp ./src/myFS.h ./mount-point
echo "---------------------------------------------------------------------------";
echo " ---------- Copiado con éxito fuseLib.c y myFS.h en nuestro SF y en el directorio temporal ------";
echo "---------------------------------------------------------------------------";


#b) Audita el disco y haga un diff entre los ficheros originales y los copiados en el SF Trunque
# El primer fichero(man truncate) en copiasTemporales y 
# en nuestro SF de manera que ocupe un bloque de datos menos.
./my-fsck virtual-disk

diff ./src/fuseLib.c ./mount-point/fuseLib.c
diff ./src/myFS.h ./mount-point/myFS.h

truncate -s -4096 ./mount-point/fuseLib.c
truncate -s -4096 ./copiasTemporales/fuseLib.c

echo "---------------------------------------------------------------------------";
echo "----------- Auditado el disco con éxito -----------";
echo "----------- Diff entre los ficheros originales y los copiados en el SF hecho-----------";
echo "----------- Truncado del primer fichero en copiasTemporales y SF ocupando un bloque menos hecho -----";
echo "---------------------------------------------------------------------------";

#c) Audita el disco y hace un diff entre el fichero original y el trucado
./my-fsck virtual-disk
diff ./src/fuseLib.c ./mount-point/fuseLib.c
echo "---------------------------------------------------------------------------";
echo "----------- Auditado el disco con éxito -----------";
echo "----------- Diff entre el fichero original y el truncado hecho -----------";
echo "---------------------------------------------------------------------------";

#d) Copia un tercer fichero de texto al SF
echo 'Prueba de mensaje' > t.txt
cp ./t.txt ./mount-point
echo "---------------------------------------------------------------------------";
echo "----------- Copiado un tercer fichero a nuestro SF -----------";
echo "---------------------------------------------------------------------------";

#e) Audita el disco y hace un diff entre el fichero original y el copiado en el SF
./my-fsck virtual-disk
diff ./t.txt ./mount-point/t.txt 
echo "---------------------------------------------------------------------------";
echo "----------- Auditado el disco con éxito -----------";
echo "----------- Diff entre el fichero original y el copiado en el SF hecho -----------";
echo "---------------------------------------------------------------------------";

#f) Trunca el segundo fichero en copiasTemporales y en el SF haciendo que ocupe algún bloque de datos más
truncate -s +4096 ./mount-point/myFS.h
truncate -s +4096 ./copiasTemporales/myFS.h
echo "---------------------------------------------------------------------------";
echo "----------- Truncado del segundo fichero en copiasTemporales y SF ocupando un bloque mas hecho -----";
echo "---------------------------------------------------------------------------";

#g) Audita el disco y hace un diff entre el fichero original y el truncado
./my-fsck virtual-disk
diff ./src/myFS.h ./mount-point/myFS.h
echo "---------------------------------------------------------------------------";
echo "----------- Auditado el disco con éxito -----------";
echo "----------- Diff entre el fichero original y el truncado hecho -----------";
echo "---------------------------------------------------------------------------";



















				

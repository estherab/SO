#!/bin/sh

# Creamos el directorio temporal.
rm -r copiasTemporales
mkdir copiasTemporales

#a) Copiamos dos archivos (que ocupan más de un bloque) a nuestro SF y al directorio temporal (Copiamos fuseLib.c).
cp ./src/fuseLib.c ./copiasTemporales
cp ./src/fuseLib.c ./mount-point
echo "---------------------------------------------------------------------------";
echo " ---------- Copiado con éxito fuseLib.c en nuestro SF y en el directorio temporal ------";
echo "---------------------------------------------------------------------------";

#b) Audita el disco y haga un diff entre los fichero original y el copiado.
./my-fsck virtual-disk
diff ./src/fuseLib.c ./mount-point/fuseLib.c
echo "---------------------------------------------------------------------------";
echo "----------- Auditado el disco con éxito -----------";
echo "----------- Diff entre el fichero originaly el copiado en el SF hecho-----------";
echo "---------------------------------------------------------------------------";

#c) Cambiar el nombre del fichero copiado.
mv ./copiasTemporales/fuseLib.c ./copiasTemporales/nuevo.c
echo "---------------------------------------------------------------------------";
echo " ---------- Cambiado el nombre del fichero copiado con exito ------";
echo "---------------------------------------------------------------------------";

#d) Audita el disco y haga un diff entre los fichero original y el renombrado.
./my-fsck virtual-disk
diff ./src/fuseLib.c ./copiasTemporales/nuevo.c
echo "---------------------------------------------------------------------------";
echo "----------- Auditado el disco con éxito -----------";
echo "----------- Diff entre el ficheros original y el copiado hecho-----------";
echo "---------------------------------------------------------------------------";

#e) Cambia la propiedad del fichero renombrado con los permisos ‘rwxr-xr-x’.
chmod 751 nuevo.c

#f) Audita el disco y hace un ls -la para comprobar las propiedades del fichero renombrado
./my-fsck virtual-disk
cd ./copiasTemporales
ls -la 

#1. Comprobamos que el programa mytar esta en el directorio y es ejecutable.

if [ ! -x /${PWD}/mytar ];
then
	echo No se ha encontrado el archivo.
	exit
fi 

#2. Comprobamos si existe el directorio tmp y si es así, lo eliminamos junto a todos los archivos de su interior.

if [ -d /${PWD}/tmp ];
then 
	rm -r ${PWD}/tmp/
fi

#3. Creamos el directorio tmp

mkdir tmp

#4. Creamos los tres ficheros 
 
# touch tmp/file1.txt
echo "Hello World!" > tmp/file1.txt
# touch tmp/file2.txt
head /etc/passwd > tmp/file2.txt 
# touch tmp/file3.dat
head -c 1024 /dev/urandom > tmp/file3.dat

#5. 
cp mytar tmp/
cd tmp/
# Ejecutamos mytar para generar filetar.mtar que contendrá a los otros tres archivos
./mytar -c -f filetar.mtar file1.txt file2.txt file3.dat
#6. Creamos el directorio output dentro de tmp
mkdir out
#7. Copiamos a la carpeta out el fichero filetar.mtar
cp filetar.mtar out/
cp mytar out
cd out/
#echo ${PWD}
#8.
./mytar -xf filetar.mtar 
cd ..
diff -s file1.txt out/file1.txt
result1=$? 
diff -s file2.txt out/file2.txt
result2=$?
diff -s file3.dat out/file3.dat
result3=$?
#9.
if [ $result1 = 0 ] && [ $result2 = 0 ] && [ $result3 = 0 ];
then 
	echo Correct
	exit 0
else
	echo Incorrect
	exit 1
fi




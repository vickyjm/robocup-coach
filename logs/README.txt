Para generar los archivos de los dos preprocesamientos de Maryam debe correrse el archivo preprocessingLogs.py con los siguientes argumentos:
	python3 preprocessingLogs.py logFile outputFile
donde logFile es el path del log que quiere analizarse y outputFile es donde se guardará el resultado del primer preprocesamiento (solo para referencia en caso de problemas).
Además, se generarán cuatro archivos que son los que deben utilizarse para los cuatro árboles que plantea Maryam. Estos estarán en shotFile.dat, passFile.dat, dribbleFile.dat
y opponentFile.dat. 

Para entrenar cada árbol debe correrse el archivo dectree.cpp con los siguientes argumentos:
	./dectree [-d | -s | -p | -o] logFile
Donde -d es para generar el árbol de dribles, -s para el de tiros, -p el de pases y -o el del oponente, y logFile es el path del archivo
correspondiente con el flag que se generó en el paso anterior.
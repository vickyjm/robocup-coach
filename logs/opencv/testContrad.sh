#!/bin/bash

N1=20
N2=30
E=200
FEAT=24	

echo "Genius"
echo "DRIBBLE"
./mlp -d ../Genius/dats/11-11/SinCont/dribbleFile.dat 1 8490 $N1 $E $FEAT
echo "PASS"
./mlp -p ../Genius/dats/11-11/SinCont/passFile.dat 1 4320 $N2 $E $FEAT
echo "SHOT"
./mlp -s ../Genius/dats/11-11/SinCont/shotFile.dat 1 880 $N2 $E $FEAT

mv dribbleMLP.yml passMLP.yml shotMLP.yml ../Genius/dats/11-11/

echo ""\n"Helios"
echo "DRIBBLE"
./mlp -d ../Helios/dats/11-11/SinCont/dribbleFile.dat 1 7600 $N1 $E $FEAT
echo "PASS"
./mlp -p ../Helios/dats/11-11/SinCont/passFile.dat 1 3850 $N2 $E $FEAT
echo "SHOT"
./mlp -s ../Helios/dats/11-11/SinCont/shotFile.dat 1 950 $N2 $E $FEAT

mv dribbleMLP.yml passMLP.yml shotMLP.yml ../Helios/dats/11-11/

echo ""\n"Hermes"
echo "DRIBBLE"
./mlp -d ../Hermes/dats/11-11/SinCont/dribbleFile.dat 1 7890 $N1 $E $FEAT
echo "PASS"
./mlp -p ../Hermes/dats/11-11/SinCont/passFile.dat 1 3930 $N2 $E $FEAT
echo "SHOT"
./mlp -s ../Hermes/dats/11-11/SinCont/shotFile.dat 1 990 $N2 $E $FEAT

mv dribbleMLP.yml passMLP.yml shotMLP.yml ../Hermes/dats/11-11/

echo ""\n"Jaeger"
echo "DRIBBLE"
./mlp -d ../Jaeger/dats/11-11/SinCont/dribbleFile.dat 1 7310 $N1 $E $FEAT
echo "PASS"
./mlp -p ../Jaeger/dats/11-11/SinCont/passFile.dat 1 4160 $N2 $E $FEAT
echo "SHOT"
./mlp -s ../Jaeger/dats/11-11/SinCont/shotFile.dat 1 1050 $N2 $E $FEAT

mv dribbleMLP.yml passMLP.yml shotMLP.yml ../Jaeger/dats/11-11/

echo ""\n"WrightEagle"
echo "DRIBBLE"
./mlp -d ../WrightEagle/dats/11-11/SinCont/dribbleFile.dat 1 8250 $N1 $E $FEAT
echo "PASS"
./mlp -p ../WrightEagle/dats/11-11/SinCont/passFile.dat 1 4890 $N2 $E $FEAT
echo "SHOT"
./mlp -s ../WrightEagle/dats/11-11/SinCont/shotFile.dat 1 880 $N2 $E $FEAT

mv dribbleMLP.yml passMLP.yml shotMLP.yml ../WrightEagle/dats/11-11/
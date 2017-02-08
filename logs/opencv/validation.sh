#!/bin/bash

N1=20
N2=30
E=200
FEAT=24	

echo "Genius"
echo "DRIBBLE"
./testMLP -d ../DatsControl/dribbleFile.dat 5 3102 $N1 $E $FEAT ../Genius/dats/11-11/dribbleMLP.yml >> GeniusValidation.txt
echo "PASS"
./testMLP -p ../DatsControl/passFile.dat 5 2820 $N2 $E $FEAT ../Genius/dats/11-11/passMLP.yml >> GeniusValidation.txt
echo "SHOT"
./testMLP -s ../DatsControl/shotFile.dat 5 520 $N2 $E $FEAT ../Genius/dats/11-11/shotMLP.yml >> GeniusValidation.txt


echo ""\n"Helios"
echo "DRIBBLE"
./testMLP -d ../DatsControl/dribbleFile.dat 5 3102 $N1 $E $FEAT ../Helios/dats/11-11/dribbleMLP.yml >> HeliosValidation.txt
echo "PASS"
./testMLP -p ../DatsControl/passFile.dat 5 2820 $N2 $E $FEAT ../Helios/dats/11-11/passMLP.yml >> HeliosValidation.txt
echo "SHOT"
./testMLP -s ../DatsControl/shotFile.dat 5 520 $N2 $E $FEAT ../Helios/dats/11-11/shotMLP.yml >> HeliosValidation.txt


echo ""\n"Hermes"
echo "DRIBBLE"
./testMLP -d ../DatsControl/dribbleFile.dat 5 3102 $N1 $E $FEAT ../Hermes/dats/11-11/dribbleMLP.yml >> HermesValidation.txt
echo "PASS"
./testMLP -p ../DatsControl/passFile.dat 5 2820 $N2 $E $FEAT ../Hermes/dats/11-11/passMLP.yml >> HermesValidation.txt
echo "SHOT"
./testMLP -s ../DatsControl/shotFile.dat 5 520 $N2 $E $FEAT ../Hermes/dats/11-11/shotMLP.yml >> HermesValidation.txt


echo ""\n"Jaeger"
echo "DRIBBLE"
./testMLP -d ../DatsControl/dribbleFile.dat 5 3102 $N1 $E $FEAT ../Jaeger/dats/11-11/dribbleMLP.yml >> JaegerValidation.txt
echo "PASS"
./testMLP -p ../DatsControl/passFile.dat 5 2820 $N2 $E $FEAT ../Jaeger/dats/11-11/passMLP.yml >> JaegerValidation.txt
echo "SHOT"
./testMLP -s ../DatsControl/shotFile.dat 5 520 $N2 $E $FEAT ../Jaeger/dats/11-11/shotMLP.yml >> JaegerValidation.txt


echo ""\n"WrightEagle"
echo "DRIBBLE"
./testMLP -d ../DatsControl/dribbleFile.dat 5 3102 $N1 $E $FEAT ../WrightEagle/dats/11-11/dribbleMLP.yml >> WrightEagleValidation.txt
echo "PASS"
./testMLP -p ../DatsControl/passFile.dat 5 2820 $N2 $E $FEAT ../WrightEagle/dats/11-11/passMLP.yml >> WrightEagleValidation.txt
echo "SHOT"
./testMLP -s ../DatsControl/shotFile.dat 5 520 $N2 $E $FEAT ../WrightEagle/dats/11-11/shotMLP.yml >> WrightEagleValidation.txt

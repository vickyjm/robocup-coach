#!/bin/bash

N1=10
N2=20
N3=30
N4=50
N5=80
E1=200
E2=500
E3=800
FEAT=24
D=14620
P=10410
T=740

echo "DRIBBLE  10-200: " >> GeniusSigmoidal.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N1 $E1 $FEAT 1 >> GeniusSigmoidal.txt
mv dribbleMLP.yml ../Genius/MLP/10-200/dribbleSigmoidal.yml

echo "DRIBBLE  10-500: " >> GeniusSigmoidal.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N1 $E2 $FEAT 1 >> GeniusSigmoidal.txt
mv dribbleMLP.yml ../Genius/MLP/10-500/dribbleSigmoidal.yml

echo "DRIBBLE  20-200: " >> GeniusSigmoidal.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N2 $E1 $FEAT 1 >> GeniusSigmoidal.txt
mv dribbleMLP.yml ../Genius/MLP/20-200/dribbleSigmoidal.yml

echo "DRIBBLE  20-500: " >> GeniusSigmoidal.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N2 $E2 $FEAT 1 >> GeniusSigmoidal.txt
mv dribbleMLP.yml ../Genius/MLP/20-500/dribbleSigmoidal.yml

echo "DRIBBLE  30-200: " >> GeniusSigmoidal.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N3 $E1 $FEAT 1 >> GeniusSigmoidal.txt
mv dribbleMLP.yml ../Genius/MLP/30-200/dribbleSigmoidal.yml

echo "DRIBBLE  30-500: " >> GeniusSigmoidal.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N3 $E2 $FEAT 1 >> GeniusSigmoidal.txt
mv dribbleMLP.yml ../Genius/MLP/30-500/dribbleSigmoidal.yml

echo "DRIBBLE  50-500: " >> GeniusSigmoidal.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N4 $E2 $FEAT 1 >> GeniusSigmoidal.txt
mv dribbleMLP.yml ../Genius/MLP/50-500/dribbleSigmoidal.yml

echo "DRIBBLE  50-800: " >> GeniusSigmoidal.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N4 $E3 $FEAT 1 >> GeniusSigmoidal.txt
mv dribbleMLP.yml ../Genius/MLP/50-800/dribbleSigmoidal.yml

echo "DRIBBLE  80-500: " >> GeniusSigmoidal.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N5 $E2 $FEAT 1 >> GeniusSigmoidal.txt
mv dribbleMLP.yml ../Genius/MLP/80-500/dribbleSigmoidal.yml

echo "DRIBBLE  80-800: " >> GeniusSigmoidal.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N5 $E3 $FEAT 1 >> GeniusSigmoidal.txt
mv dribbleMLP.yml ../Genius/MLP/80-800/dribbleSigmoidal.yml


echo "DRIBBLE  10-200: " >> GeniusGaussiana.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N1 $E1 $FEAT 0 >> GeniusGaussiana.txt
mv dribbleMLP.yml ../Genius/MLP/10-200/dribbleGaussian.yml

echo "DRIBBLE  10-500: " >> GeniusGaussiana.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N1 $E2 $FEAT 0 >> GeniusGaussiana.txt
mv dribbleMLP.yml ../Genius/MLP/10-500/dribbleGaussian.yml

echo "DRIBBLE  20-200: " >> GeniusGaussiana.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N2 $E1 $FEAT 0 >> GeniusGaussiana.txt
mv dribbleMLP.yml ../Genius/MLP/20-200/dribbleGaussian.yml

echo "DRIBBLE  20-500: " >> GeniusGaussiana.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N2 $E2 $FEAT 0 >> GeniusGaussiana.txt
mv dribbleMLP.yml ../Genius/MLP/20-500/dribbleGaussian.yml

echo "DRIBBLE  30-200: " >> GeniusGaussiana.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N3 $E1 $FEAT 0 >> GeniusGaussiana.txt
mv dribbleMLP.yml ../Genius/MLP/30-200/dribbleGaussian.yml

echo "DRIBBLE  30-500: " >> GeniusGaussiana.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N3 $E2 $FEAT 0 >> GeniusGaussiana.txt
mv dribbleMLP.yml ../Genius/MLP/30-500/dribbleGaussian.yml

echo "DRIBBLE  50-500: " >> GeniusGaussiana.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N4 $E2 $FEAT 0 >> GeniusGaussiana.txt
mv dribbleMLP.yml ../Genius/MLP/50-500/dribbleGaussian.yml

echo "DRIBBLE  50-800: " >> GeniusGaussiana.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N4 $E3 $FEAT 0 >> GeniusGaussiana.txt
mv dribbleMLP.yml ../Genius/MLP/50-800/dribbleGaussian.yml

echo "DRIBBLE  80-500: " >> GeniusGaussiana.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N5 $E2 $FEAT 0 >> GeniusGaussiana.txt
mv dribbleMLP.yml ../Genius/MLP/80-500/dribbleGaussian.yml

echo "DRIBBLE  80-800: " >> GeniusGaussiana.txt
./mlp -d ../Genius/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N5 $E3 $FEAT 0 >> GeniusGaussiana.txt
mv dribbleMLP.yml ../Genius/MLP/80-800/dribbleGaussian.yml
echo "============================" >> GeniusSigmoidal.txt
echo "============================" >> GeniusGaussiana.txt

echo "pass  10-200: " >> GeniusSigmoidal.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N1 $E1 $FEAT 1 >> GeniusSigmoidal.txt
mv passMLP.yml ../Genius/MLP/10-200/passSigmoidal.yml

echo "pass  10-500: " >> GeniusSigmoidal.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N1 $E2 $FEAT 1 >> GeniusSigmoidal.txt
mv passMLP.yml ../Genius/MLP/10-500/passSigmoidal.yml

echo "pass  20-200: " >> GeniusSigmoidal.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N2 $E1 $FEAT 1 >> GeniusSigmoidal.txt
mv passMLP.yml ../Genius/MLP/20-200/passSigmoidal.yml

echo "pass  20-500: " >> GeniusSigmoidal.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N2 $E2 $FEAT 1 >> GeniusSigmoidal.txt
mv passMLP.yml ../Genius/MLP/20-500/passSigmoidal.yml

echo "pass  30-200: " >> GeniusSigmoidal.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N3 $E1 $FEAT 1 >> GeniusSigmoidal.txt
mv passMLP.yml ../Genius/MLP/30-200/passSigmoidal.yml

echo "pass  30-500: " >> GeniusSigmoidal.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N3 $E2 $FEAT 1 >> GeniusSigmoidal.txt
mv passMLP.yml ../Genius/MLP/30-500/passSigmoidal.yml

echo "pass  50-500: " >> GeniusSigmoidal.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N4 $E2 $FEAT 1 >> GeniusSigmoidal.txt
mv passMLP.yml ../Genius/MLP/50-500/passSigmoidal.yml

echo "pass  50-800: " >> GeniusSigmoidal.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N4 $E3 $FEAT 1 >> GeniusSigmoidal.txt
mv passMLP.yml ../Genius/MLP/50-800/passSigmoidal.yml

echo "pass  80-500: " >> GeniusSigmoidal.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N5 $E2 $FEAT 1 >> GeniusSigmoidal.txt
mv passMLP.yml ../Genius/MLP/80-500/passSigmoidal.yml

echo "pass  80-800: " >> GeniusSigmoidal.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N5 $E3 $FEAT 1 >> GeniusSigmoidal.txt
mv passMLP.yml ../Genius/MLP/80-800/passSigmoidal.yml


echo "pass  10-200: " >> GeniusGaussiana.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N1 $E1 $FEAT 0 >> GeniusGaussiana.txt
mv passMLP.yml ../Genius/MLP/10-200/passGaussian.yml

echo "pass  10-500: " >> GeniusGaussiana.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N1 $E2 $FEAT 0 >> GeniusGaussiana.txt
mv passMLP.yml ../Genius/MLP/10-500/passGaussian.yml

echo "pass  20-200: " >> GeniusGaussiana.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N2 $E1 $FEAT 0 >> GeniusGaussiana.txt
mv passMLP.yml ../Genius/MLP/20-200/passGaussian.yml

echo "pass  20-500: " >> GeniusGaussiana.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N2 $E2 $FEAT 0 >> GeniusGaussiana.txt
mv passMLP.yml ../Genius/MLP/20-500/passGaussian.yml

echo "pass  30-200: " >> GeniusGaussiana.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N3 $E1 $FEAT 0 >> GeniusGaussiana.txt
mv passMLP.yml ../Genius/MLP/30-200/passGaussian.yml

echo "pass  30-500: " >> GeniusGaussiana.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N3 $E2 $FEAT 0 >> GeniusGaussiana.txt
mv passMLP.yml ../Genius/MLP/30-500/passGaussian.yml

echo "pass  50-500: " >> GeniusGaussiana.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N4 $E2 $FEAT 0 >> GeniusGaussiana.txt
mv passMLP.yml ../Genius/MLP/50-500/passGaussian.yml

echo "pass  50-800: " >> GeniusGaussiana.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N4 $E3 $FEAT 0 >> GeniusGaussiana.txt
mv passMLP.yml ../Genius/MLP/50-800/passGaussian.yml

echo "pass  80-500: " >> GeniusGaussiana.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N5 $E2 $FEAT 0 >> GeniusGaussiana.txt
mv passMLP.yml ../Genius/MLP/80-500/passGaussian.yml

echo "pass  80-800: " >> GeniusGaussiana.txt
./mlp -p ../Genius/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N5 $E3 $FEAT 0 >> GeniusGaussiana.txt
mv passMLP.yml ../Genius/MLP/80-800/passGaussian.yml

echo "============================" >> GeniusSigmoidal.txt
echo "============================" >> GeniusGaussiana.txt
echo "shot  10-200: " >> GeniusSigmoidal.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N1 $E1 $FEAT 1 >> GeniusSigmoidal.txt
mv shotMLP.yml ../Genius/MLP/10-200/shotGaussian.yml

echo "shot  10-500: " >> GeniusSigmoidal.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N1 $E2 $FEAT 1 >> GeniusSigmoidal.txt
mv shotMLP.yml ../Genius/MLP/10-500/shotGaussian.yml

echo "shot  20-200: " >> GeniusSigmoidal.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N2 $E1 $FEAT 1 >> GeniusSigmoidal.txt
mv shotMLP.yml ../Genius/MLP/20-200/shotGaussian.yml

echo "shot  20-500: " >> GeniusSigmoidal.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N2 $E2 $FEAT 1 >> GeniusSigmoidal.txt
mv shotMLP.yml ../Genius/MLP/20-500/shotGaussian.yml

echo "shot  30-200: " >> GeniusSigmoidal.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N3 $E1 $FEAT 1 >> GeniusSigmoidal.txt
mv shotMLP.yml ../Genius/MLP/30-200/shotGaussian.yml

echo "shot  30-500: " >> GeniusSigmoidal.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N3 $E2 $FEAT 1 >> GeniusSigmoidal.txt
mv shotMLP.yml ../Genius/MLP/30-500/shotGaussian.yml

echo "shot  50-500: " >> GeniusSigmoidal.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N4 $E2 $FEAT 1 >> GeniusSigmoidal.txt
mv shotMLP.yml ../Genius/MLP/50-500/shotGaussian.yml

echo "shot  50-800: " >> GeniusSigmoidal.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N4 $E3 $FEAT 1 >> GeniusSigmoidal.txt
mv shotMLP.yml ../Genius/MLP/50-800/shotGaussian.yml

echo "shot  80-500: " >> GeniusSigmoidal.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N5 $E2 $FEAT 1 >> GeniusSigmoidal.txt
mv shotMLP.yml ../Genius/MLP/80-500/shotGaussian.yml

echo "shot  80-800: " >> GeniusSigmoidal.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N5 $E3 $FEAT 1 >> GeniusSigmoidal.txt
mv shotMLP.yml ../Genius/MLP/80-800/shotGaussian.yml


echo "shot  10-200: " >> GeniusGaussiana.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N1 $E1 $FEAT 0 >> GeniusGaussiana.txt
mv shotMLP.yml ../Genius/MLP/10-200/shotGaussian.yml

echo "shot  10-500: " >> GeniusGaussiana.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N1 $E2 $FEAT 0 >> GeniusGaussiana.txt
mv shotMLP.yml ../Genius/MLP/10-500/shotGaussian.yml

echo "shot  20-200: " >> GeniusGaussiana.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N2 $E1 $FEAT 0 >> GeniusGaussiana.txt
mv shotMLP.yml ../Genius/MLP/20-200/shotGaussian.yml

echo "shot  20-500: " >> GeniusGaussiana.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N2 $E2 $FEAT 0 >> GeniusGaussiana.txt
mv shotMLP.yml ../Genius/MLP/20-500/shotGaussian.yml

echo "shot  30-200: " >> GeniusGaussiana.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N3 $E1 $FEAT 0 >> GeniusGaussiana.txt
mv shotMLP.yml ../Genius/MLP/30-200/shotGaussian.yml

echo "shot  30-500: " >> GeniusGaussiana.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N3 $E2 $FEAT 0 >> GeniusGaussiana.txt
mv shotMLP.yml ../Genius/MLP/30-500/shotGaussian.yml

echo "shot  50-500: " >> GeniusGaussiana.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N4 $E2 $FEAT 0 >> GeniusGaussiana.txt
mv shotMLP.yml ../Genius/MLP/50-500/shotGaussian.yml

echo "shot  50-800: " >> GeniusGaussiana.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N4 $E3 $FEAT 0 >> GeniusGaussiana.txt
mv shotMLP.yml ../Genius/MLP/50-800/shotGaussian.yml

echo "shot  80-500: " >> GeniusGaussiana.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N5 $E2 $FEAT 0 >> GeniusGaussiana.txt
mv shotMLP.yml ../Genius/MLP/80-500/shotGaussian.yml

echo "shot  80-800: " >> GeniusGaussiana.txt
./mlp -s ../Genius/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N5 $E3 $FEAT 0 >> GeniusGaussiana.txt
mv shotMLP.yml ../Genius/MLP/80-800/shotGaussian.yml
echo "==== " >> GeniusSigmoidal.txt

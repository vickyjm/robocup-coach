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
D=9310
P=8370
T=420

# echo "DRIBBLE  10-200: " >> HermesSigmoidal.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N1 $E1 $FEAT 1 >> HermesSigmoidal.txt
# mv dribbleMLP.yml ../Hermes/MLP/10-200/dribbleSigmoidal.yml

# echo "DRIBBLE  10-500: " >> HermesSigmoidal.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N1 $E2 $FEAT 1 >> HermesSigmoidal.txt
# mv dribbleMLP.yml ../Hermes/MLP/10-500/dribbleSigmoidal.yml

# echo "DRIBBLE  20-200: " >> HermesSigmoidal.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N2 $E1 $FEAT 1 >> HermesSigmoidal.txt
# mv dribbleMLP.yml ../Hermes/MLP/20-200/dribbleSigmoidal.yml

# echo "DRIBBLE  20-500: " >> HermesSigmoidal.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N2 $E2 $FEAT 1 >> HermesSigmoidal.txt
# mv dribbleMLP.yml ../Hermes/MLP/20-500/dribbleSigmoidal.yml

# echo "DRIBBLE  30-200: " >> HermesSigmoidal.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N3 $E1 $FEAT 1 >> HermesSigmoidal.txt
# mv dribbleMLP.yml ../Hermes/MLP/30-200/dribbleSigmoidal.yml

# echo "DRIBBLE  30-500: " >> HermesSigmoidal.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N3 $E2 $FEAT 1 >> HermesSigmoidal.txt
# mv dribbleMLP.yml ../Hermes/MLP/30-500/dribbleSigmoidal.yml

# echo "DRIBBLE  50-500: " >> HermesSigmoidal.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N4 $E2 $FEAT 1 >> HermesSigmoidal.txt
# mv dribbleMLP.yml ../Hermes/MLP/50-500/dribbleSigmoidal.yml

# echo "DRIBBLE  50-800: " >> HermesSigmoidal.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N4 $E3 $FEAT 1 >> HermesSigmoidal.txt
# mv dribbleMLP.yml ../Hermes/MLP/50-800/dribbleSigmoidal.yml

# echo "DRIBBLE  80-500: " >> HermesSigmoidal.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N5 $E2 $FEAT 1 >> HermesSigmoidal.txt
# mv dribbleMLP.yml ../Hermes/MLP/80-500/dribbleSigmoidal.yml

# echo "DRIBBLE  80-800: " >> HermesSigmoidal.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N5 $E3 $FEAT 1 >> HermesSigmoidal.txt
# mv dribbleMLP.yml ../Hermes/MLP/80-800/dribbleSigmoidal.yml


# echo "DRIBBLE  10-200: " >> HermesGaussiana.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N1 $E1 $FEAT 0 >> HermesGaussiana.txt
# mv dribbleMLP.yml ../Hermes/MLP/10-200/dribbleGaussian.yml

# echo "DRIBBLE  10-500: " >> HermesGaussiana.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N1 $E2 $FEAT 0 >> HermesGaussiana.txt
# mv dribbleMLP.yml ../Hermes/MLP/10-500/dribbleGaussian.yml

# echo "DRIBBLE  20-200: " >> HermesGaussiana.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N2 $E1 $FEAT 0 >> HermesGaussiana.txt
# mv dribbleMLP.yml ../Hermes/MLP/20-200/dribbleGaussian.yml

# echo "DRIBBLE  20-500: " >> HermesGaussiana.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N2 $E2 $FEAT 0 >> HermesGaussiana.txt
# mv dribbleMLP.yml ../Hermes/MLP/20-500/dribbleGaussian.yml

# echo "DRIBBLE  30-200: " >> HermesGaussiana.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N3 $E1 $FEAT 0 >> HermesGaussiana.txt
# mv dribbleMLP.yml ../Hermes/MLP/30-200/dribbleGaussian.yml

# echo "DRIBBLE  30-500: " >> HermesGaussiana.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N3 $E2 $FEAT 0 >> HermesGaussiana.txt
# mv dribbleMLP.yml ../Hermes/MLP/30-500/dribbleGaussian.yml

# echo "DRIBBLE  50-500: " >> HermesGaussiana.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N4 $E2 $FEAT 0 >> HermesGaussiana.txt
# mv dribbleMLP.yml ../Hermes/MLP/50-500/dribbleGaussian.yml

# echo "DRIBBLE  50-800: " >> HermesGaussiana.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N4 $E3 $FEAT 0 >> HermesGaussiana.txt
# mv dribbleMLP.yml ../Hermes/MLP/50-800/dribbleGaussian.yml

# echo "DRIBBLE  80-500: " >> HermesGaussiana.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N5 $E2 $FEAT 0 >> HermesGaussiana.txt
# mv dribbleMLP.yml ../Hermes/MLP/80-500/dribbleGaussian.yml

# echo "DRIBBLE  80-800: " >> HermesGaussiana.txt
# ./mlp -d ../Hermes/dats/OnlyJEMV/Normalizados/dribbleFile.dat 5 $D $N5 $E3 $FEAT 0 >> HermesGaussiana.txt
# mv dribbleMLP.yml ../Hermes/MLP/80-800/dribbleGaussian.yml
# echo "============================" >> HermesSigmoidal.txt
# echo "============================" >> HermesGaussiana.txt

echo "pass  10-200: " >> HermesSigmoidal.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N1 $E1 $FEAT 1 >> HermesSigmoidal.txt
mv passMLP.yml ../Hermes/MLP/10-200/passSigmoidal.yml

echo "pass  10-500: " >> HermesSigmoidal.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N1 $E2 $FEAT 1 >> HermesSigmoidal.txt
mv passMLP.yml ../Hermes/MLP/10-500/passSigmoidal.yml

echo "pass  20-200: " >> HermesSigmoidal.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N2 $E1 $FEAT 1 >> HermesSigmoidal.txt
mv passMLP.yml ../Hermes/MLP/20-200/passSigmoidal.yml

echo "pass  20-500: " >> HermesSigmoidal.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N2 $E2 $FEAT 1 >> HermesSigmoidal.txt
mv passMLP.yml ../Hermes/MLP/20-500/passSigmoidal.yml

echo "pass  30-200: " >> HermesSigmoidal.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N3 $E1 $FEAT 1 >> HermesSigmoidal.txt
mv passMLP.yml ../Hermes/MLP/30-200/passSigmoidal.yml

echo "pass  30-500: " >> HermesSigmoidal.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N3 $E2 $FEAT 1 >> HermesSigmoidal.txt
mv passMLP.yml ../Hermes/MLP/30-500/passSigmoidal.yml

echo "pass  50-500: " >> HermesSigmoidal.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N4 $E2 $FEAT 1 >> HermesSigmoidal.txt
mv passMLP.yml ../Hermes/MLP/50-500/passSigmoidal.yml

echo "pass  50-800: " >> HermesSigmoidal.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N4 $E3 $FEAT 1 >> HermesSigmoidal.txt
mv passMLP.yml ../Hermes/MLP/50-800/passSigmoidal.yml

echo "pass  80-500: " >> HermesSigmoidal.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N5 $E2 $FEAT 1 >> HermesSigmoidal.txt
mv passMLP.yml ../Hermes/MLP/80-500/passSigmoidal.yml

echo "pass  80-800: " >> HermesSigmoidal.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N5 $E3 $FEAT 1 >> HermesSigmoidal.txt
mv passMLP.yml ../Hermes/MLP/80-800/passSigmoidal.yml


echo "pass  10-200: " >> HermesGaussiana.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N1 $E1 $FEAT 0 >> HermesGaussiana.txt
mv passMLP.yml ../Hermes/MLP/10-200/passGaussian.yml

echo "pass  10-500: " >> HermesGaussiana.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N1 $E2 $FEAT 0 >> HermesGaussiana.txt
mv passMLP.yml ../Hermes/MLP/10-500/passGaussian.yml

echo "pass  20-200: " >> HermesGaussiana.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N2 $E1 $FEAT 0 >> HermesGaussiana.txt
mv passMLP.yml ../Hermes/MLP/20-200/passGaussian.yml

echo "pass  20-500: " >> HermesGaussiana.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N2 $E2 $FEAT 0 >> HermesGaussiana.txt
mv passMLP.yml ../Hermes/MLP/20-500/passGaussian.yml

echo "pass  30-200: " >> HermesGaussiana.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N3 $E1 $FEAT 0 >> HermesGaussiana.txt
mv passMLP.yml ../Hermes/MLP/30-200/passGaussian.yml

echo "pass  30-500: " >> HermesGaussiana.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N3 $E2 $FEAT 0 >> HermesGaussiana.txt
mv passMLP.yml ../Hermes/MLP/30-500/passGaussian.yml

echo "pass  50-500: " >> HermesGaussiana.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N4 $E2 $FEAT 0 >> HermesGaussiana.txt
mv passMLP.yml ../Hermes/MLP/50-500/passGaussian.yml

echo "pass  50-800: " >> HermesGaussiana.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N4 $E3 $FEAT 0 >> HermesGaussiana.txt
mv passMLP.yml ../Hermes/MLP/50-800/passGaussian.yml

echo "pass  80-500: " >> HermesGaussiana.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N5 $E2 $FEAT 0 >> HermesGaussiana.txt
mv passMLP.yml ../Hermes/MLP/80-500/passGaussian.yml

echo "pass  80-800: " >> HermesGaussiana.txt
./mlp -p ../Hermes/dats/OnlyJEMV/Normalizados/passFile.dat 5 $P $N5 $E3 $FEAT 0 >> HermesGaussiana.txt
mv passMLP.yml ../Hermes/MLP/80-800/passGaussian.yml

echo "============================" >> HermesSigmoidal.txt
echo "============================" >> HermesGaussiana.txt
echo "shot  10-200: " >> HermesSigmoidal.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N1 $E1 $FEAT 1 >> HermesSigmoidal.txt
mv shotMLP.yml ../Hermes/MLP/10-200/shotSigmoidal.yml

echo "shot  10-500: " >> HermesSigmoidal.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N1 $E2 $FEAT 1 >> HermesSigmoidal.txt
mv shotMLP.yml ../Hermes/MLP/10-500/shotSigmoidal.yml

echo "shot  20-200: " >> HermesSigmoidal.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N2 $E1 $FEAT 1 >> HermesSigmoidal.txt
mv shotMLP.yml ../Hermes/MLP/20-200/shotSigmoidal.yml

echo "shot  20-500: " >> HermesSigmoidal.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N2 $E2 $FEAT 1 >> HermesSigmoidal.txt
mv shotMLP.yml ../Hermes/MLP/20-500/shotSigmoidal.yml

echo "shot  30-200: " >> HermesSigmoidal.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N3 $E1 $FEAT 1 >> HermesSigmoidal.txt
mv shotMLP.yml ../Hermes/MLP/30-200/shotSigmoidal.yml

echo "shot  30-500: " >> HermesSigmoidal.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N3 $E2 $FEAT 1 >> HermesSigmoidal.txt
mv shotMLP.yml ../Hermes/MLP/30-500/shotSigmoidal.yml

echo "shot  50-500: " >> HermesSigmoidal.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N4 $E2 $FEAT 1 >> HermesSigmoidal.txt
mv shotMLP.yml ../Hermes/MLP/50-500/shotSigmoidal.yml

echo "shot  50-800: " >> HermesSigmoidal.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N4 $E3 $FEAT 1 >> HermesSigmoidal.txt
mv shotMLP.yml ../Hermes/MLP/50-800/shotSigmoidal.yml

echo "shot  80-500: " >> HermesSigmoidal.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N5 $E2 $FEAT 1 >> HermesSigmoidal.txt
mv shotMLP.yml ../Hermes/MLP/80-500/shotSigmoidal.yml

echo "shot  80-800: " >> HermesSigmoidal.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N5 $E3 $FEAT 1 >> HermesSigmoidal.txt
mv shotMLP.yml ../Hermes/MLP/80-800/shotSigmoidal.yml


echo "shot  10-200: " >> HermesGaussiana.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N1 $E1 $FEAT 0 >> HermesGaussiana.txt
mv shotMLP.yml ../Hermes/MLP/10-200/shotGaussian.yml

echo "shot  10-500: " >> HermesGaussiana.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N1 $E2 $FEAT 0 >> HermesGaussiana.txt
mv shotMLP.yml ../Hermes/MLP/10-500/shotGaussian.yml

echo "shot  20-200: " >> HermesGaussiana.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N2 $E1 $FEAT 0 >> HermesGaussiana.txt
mv shotMLP.yml ../Hermes/MLP/20-200/shotGaussian.yml

echo "shot  20-500: " >> HermesGaussiana.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N2 $E2 $FEAT 0 >> HermesGaussiana.txt
mv shotMLP.yml ../Hermes/MLP/20-500/shotGaussian.yml

echo "shot  30-200: " >> HermesGaussiana.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N3 $E1 $FEAT 0 >> HermesGaussiana.txt
mv shotMLP.yml ../Hermes/MLP/30-200/shotGaussian.yml

echo "shot  30-500: " >> HermesGaussiana.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N3 $E2 $FEAT 0 >> HermesGaussiana.txt
mv shotMLP.yml ../Hermes/MLP/30-500/shotGaussian.yml

echo "shot  50-500: " >> HermesGaussiana.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N4 $E2 $FEAT 0 >> HermesGaussiana.txt
mv shotMLP.yml ../Hermes/MLP/50-500/shotGaussian.yml

echo "shot  50-800: " >> HermesGaussiana.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N4 $E3 $FEAT 0 >> HermesGaussiana.txt
mv shotMLP.yml ../Hermes/MLP/50-800/shotGaussian.yml

echo "shot  80-500: " >> HermesGaussiana.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N5 $E2 $FEAT 0 >> HermesGaussiana.txt
mv shotMLP.yml ../Hermes/MLP/80-500/shotGaussian.yml

echo "shot  80-800: " >> HermesGaussiana.txt
./mlp -s ../Hermes/dats/OnlyJEMV/Normalizados/shotFile.dat 5 $T $N5 $E3 $FEAT 0 >> HermesGaussiana.txt
mv shotMLP.yml ../Hermes/MLP/80-800/shotGaussian.yml
echo "==== " >> HermesSigmoidal.txt

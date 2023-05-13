#!/bin/sh

echo "...................................."
echo "Installing SAT solver kissat_pre"
echo "...................................."


# sudo apt install curl

curl -sS https://satcompetition.github.io/2022/downloads/sequential-solvers.zip > sequential-solvers.zip
unzip "sequential-solvers.zip" "kissat_pre/sources/*" -d "./"
cd "kissat_pre/sources/src"
sed -i '21s/.*/};/' "hashmap.h" # dirty fix to make it compile
cd ..
./configure
cd build
make kissat 
mv kissat "../../../kissat" 
cd "../../.."
rm -r "kissat_pre"
rm "sequential-solvers.zip"


echo "...................................."
echo "Compiling the encoder/decoder"
echo "...................................."

# cd ../../..
# mkdir obj
# mkdir obj/encoder
# mkdir obj/logic
# mkdir obj/simulator

# make main

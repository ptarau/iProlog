#echo go.sh <name_of_prog> assumed in dir ./progs
mkdir -p out
mkdir -p out/production
mkdir -p out/production/IP
export TARGET="out/production/IP"
mkdir -p "$TARGET"
rm -r -f $TARGET/iProlog
rm -f progs/*.pl.nl
echo "starting java compilations"
javac -O -d "$TARGET" src/iProlog/core/src/main/java/org/iprolog/*.java
echo "swipl starting pl2nl on $1.pl ...."
swipl -f pl2nl.pl -g "pl('$1'),halt"
echo "java starting on $1.pl.nl ...."
# java -cp "$TARGET" iProlog.Main "progs/$1.pl"
# java -cp "$TARGET/org/iprolog" org.iprolog.Main "progs/$1.pl"
java -cp "$TARGET" org.iprolog.Main progs/$1.pl

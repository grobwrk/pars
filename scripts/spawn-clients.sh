#!/usr/bin/env zsh

addr=$1 # server addr

t=$2 # total
c=$3 # concurrent
n=$4 # the argument for fib(n)
a=$5 # the algorithm (fast or slow)

k=$(($t/$c))

client="./client"
[ -f "./client.exe" ] && client="./client.exe"

echo "RUNNING $t CLIENT $c AT A TIME, EACH COMPUTING FIB($n)"

sum=0
for j in {1..$k}
do
    out=$(for i in {1..$c}; do $client dial $addr $((($j-1)*$c+$i)) $a $n &; done | grep "FIB($n)")
    if [ $? -eq 1 ]; then
        echo "Client Error! Exiting ..."
        exit
    fi
    # echo $out
    echo $out | wc -l | read computed
    sum=$(($sum+$computed))
    echo "$computed ($sum)"
done

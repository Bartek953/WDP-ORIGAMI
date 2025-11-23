gcc @opcje_mac origami.c -o wzo

dir=tests/custom
start=0
end=9


for((i=$start;i<=$end;i++))
do
    echo "test $i"
    MallocNanoZone=0 ./wzo < $dir/$i.in > t.out
    #cat out
    #cat $dir/$i.out
    diff -b t.out $dir/$i.out || break
    echo "OK"
    echo ""
done
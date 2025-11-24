gcc @opcje_mac origami.c -o wzo

dir=origami-tests/random/float_test
start=1
end=4000

echo "$dir"

for((i=$start;i<=$end;i++))
do
    echo "test $i"
    MallocNanoZone=0 ./wzo < $dir/ori$i.in > t.out
    #cat out
    #cat $dir/$i.out
    diff -b t.out $dir/ori$i.out || break
    echo "OK"
    echo ""
done
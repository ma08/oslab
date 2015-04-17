cc gen_input.c
dimension=6
int_per_page=3
./a.out $dimension $int_per_page > input.txt
#rm FIFO_output.txt LRU_output.txt LFU_output.txt secondChance.txt
dir_name="output_data_$dimension"
dir_name+="_$int_per_page"
echo $dir_name
mkdir $dir_name
for (( c=4; c<=15; c++ ))
do
  g++ -std=c++11 page.cpp
  printf "1\n" | ./a.out $c > output.txt
  result=$(tail -n 1 output.txt)
  echo $c $result
  echo $c $result >> FIFO_output.txt
  printf "2\n" | ./a.out $c > output.txt
  result=$(tail -n 1 output.txt)
  echo $c $result
  echo $c $result >> LRU_output.txt
  printf "3\n" | ./a.out $c > output.txt
  result=$(tail -n 1 output.txt)
  echo $c $result
  echo $c $result >> LFU_output.txt
  printf "4\n" | ./a.out $c > output.txt
  result=$(tail -n 1 output.txt)
  echo $c $result
  echo $c $result >> secondChance.txt
done
mv FIFO_output.txt LRU_output.txt LFU_output.txt secondChance.txt $dir_name

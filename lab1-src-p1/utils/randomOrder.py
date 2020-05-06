import random;
import sys;

arr = [i for i in range(int(sys.argv[1]))];
random.shuffle(arr);
print(arr);

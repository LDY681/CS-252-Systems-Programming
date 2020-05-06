import random;
import sys;

arr = [random.randint(0, int(sys.argv[2])) for i in range(int(sys.argv[1]))];
random.shuffle(arr);
print(arr);

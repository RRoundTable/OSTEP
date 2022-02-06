#! /usr/bin/env python3
import time


if __name__ == "__main__":
    start = time.time()
    for _ in range(100):
        with open("dummpy.txt", "r") as f:
            f.read()
    print(f"Average Time for System Call Open: {(time.time() - start)/100}")
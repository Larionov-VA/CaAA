import random
import string
import sys

def generate_strings(length=20):
    chars = string.ascii_letters
    line1 = ''.join(random.choice(chars) for _ in range(length))
    line2 = ''.join(random.choice(chars) for _ in range(length))
    print(line1)
    print(line2)
    print("1 1 1 1")

if __name__ == "__main__":
    size = int(sys.argv[1]) if len(sys.argv) > 1 else 20
    generate_strings(size)
import random
import string
import sys


LINES_FOR_TEST = int(sys.argv[1])
FILE_NAME = "test.txt"

def generate_random_string(max_length: int):
    length = random.randint(5, max_length)
    return ''.join(random.choices(string.ascii_letters + string.digits, k=length))

def generate_data():
    count_keys = (LINES_FOR_TEST // 3) + random.randint(1, 10)
    if count_keys < 1:
        count_keys = 1
    keys = [round(random.uniform(-100, 100), 6) for _ in range(count_keys)]
    
    with open(FILE_NAME, "w", encoding="utf-8") as f:
        for _ in range(LINES_FOR_TEST):
            key = random.choice(keys)
            value = generate_random_string(max_length=2048)
            f.write(f"{key:.6f}\t{value}\n")

if __name__ == "__main__":
    generate_data()
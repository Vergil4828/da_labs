import random
import sys
import string

# Параметры из условия
MAX_KEY_LENGTH = 256
MAX_VALUE = 2**64 - 1  # 2^64 - 1


def gen_key():
    length = random.randint(1, MAX_KEY_LENGTH)
    # Генерируем случайные буквы (верхний и нижний регистр для проверки lower)
    return "".join(random.choice(string.ascii_letters) for _ in range(length))


if len(sys.argv) < 2:
    print("Usage: python3 test_gen.py <amount>")
    sys.exit(1)

amount = int(sys.argv[1])
keys = []

with open("test.txt", "w") as f:
    # 1. Добавление (операция +)
    for _ in range(amount):
        key = gen_key()
        val = random.randint(0, MAX_VALUE)
        keys.append(key)
        f.write(f"+ {key} {val}\n")

    # 2. Поиск (просто слово)
    # Смешаем существующие ключи и случайный мусор
    for _ in range(amount):
        if random.random() > 0.3:
            f.write(f"{random.choice(keys)}\n")
        else:
            f.write(f"{gen_key()}_missing\n")

    # 3. Удаление (операция -)
    for _ in range(amount):
        f.write(f"- {random.choice(keys)}\n")

print(f"Done! Generated {amount*3} commands in test.txt")

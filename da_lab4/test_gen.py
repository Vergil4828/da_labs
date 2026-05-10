import random
import string
import sys

FILENAME_INPUT = "input.txt"
FILENAME_EXPECTED = "expected.txt"
CHARS = string.ascii_letters + string.digits

def random_word(min_len=5, max_len=8):
    return "".join(random.choice(CHARS) for _ in range(random.randint(min_len, max_len)))

def change_case(word):
    return "".join(c.upper() if random.getrandbits(1) else c.lower() for c in word)

def generate_test(total_words):
    pattern_len = 100 
    word_a = "aaaaaaaa"
    word_b = "bbbbbbbb"

    pattern_origin = [word_a] * (pattern_len - 1) + [word_b]
    pattern_header = " ".join(change_case(w) for w in pattern_origin)

    words_per_line = 10
    
    flat_text = [change_case(word_a) for _ in range(total_words)]
    
    answers = []
    
    occurrences_count = max(1, total_words // 50000)
    indices = sorted(random.sample(range(total_words - pattern_len), occurrences_count))

    for idx in indices:
        for i in range(pattern_len):
            flat_text[idx + i] = change_case(pattern_origin[i])
        
        line_num = (idx // words_per_line) + 1
        word_num = (idx % words_per_line) + 1
        answers.append(f"{line_num}, {word_num}")

    with open(FILENAME_INPUT, "w", encoding="utf-8") as f:
        f.write(pattern_header + "\n")
        for i in range(0, len(flat_text), words_per_line):
            f.write(" ".join(flat_text[i : i + words_per_line]) + "\n")

    with open(FILENAME_EXPECTED, "w", encoding="utf-8") as f:
        f.write("\n".join(answers) + "\n")
    
    print(f"Worst-case test generated: {total_words} words, pattern length {pattern_len}")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        try:
            num = int(sys.argv[1])
            generate_test(num)
        except ValueError:
            pass
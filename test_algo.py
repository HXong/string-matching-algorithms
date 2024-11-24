import ctypes
import random
import string
import time
import pandas as pd
from concurrent.futures import ThreadPoolExecutor

boyer_moore_lib = ctypes.CDLL("./dll/boyer_moore.dll")
boyer_moore = boyer_moore_lib.boyer_moore
boyer_moore.argtypes = [
    ctypes.c_char_p,
    ctypes.c_char_p,
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
]
boyer_moore.restype = None

rabin_karp_lib = ctypes.CDLL("./dll/rabin_karp.dll")
rabin_karp = rabin_karp_lib.rabin_karp
rabin_karp.argtypes = [
    ctypes.c_char_p,
    ctypes.c_char_p,
    ctypes.POINTER(ctypes.c_int),
    ctypes.POINTER(ctypes.c_int),
]
rabin_karp.restype = None

def generate_random_test_case(text_length=100, pattern_length=10):
    text = ''.join(random.choices(string.ascii_letters + string.whitespace, k=text_length))
    pattern = ''.join(random.choices(string.ascii_letters + string.whitespace, k=pattern_length))
    return text, pattern

def generate_test_cases_parallel(test_cases, text_length, pattern_length):
    with ThreadPoolExecutor() as executor:
        test_cases_data = list(executor.map(
            lambda _: generate_random_test_case(text_length, pattern_length),
            range(test_cases)
        ))
    return test_cases_data

def naive_string_matching(text, pattern):
    matches = []
    n, m = len(text), len(pattern)
    for i in range(n - m + 1):
        if text[i:i + m] == pattern:
            matches.append(i)
    return matches

def super_test_run(test_cases=1000, text_length=10000000, pattern_length=100000):

    text_c = ctypes.create_string_buffer(text_length + 1)
    pattern_c = ctypes.create_string_buffer(pattern_length + 1)
    matches = (ctypes.c_int * text_length)()
    count = ctypes.c_int(0)

    test_cases_data = generate_test_cases_parallel(test_cases, text_length, pattern_length)

    bm_mismatches = []
    rk_mismatches = []
    BMtime = 0
    RKtime = 0
    #Naivetime = 0
    Progress = 0
    last_percentage = -1

    for text, pattern in test_cases_data:
        
        text_c.value = text.encode('utf-8')
        pattern_c.value = pattern.encode('utf-8')

        # Call Boyer-Moore in C

        start_time = time.time()
        boyer_moore(text_c, pattern_c, matches, ctypes.byref(count))
        end_time = time.time()
        BMtime += end_time - start_time
        bm_matches = [matches[i] for i in range(count.value)]

        start_time = time.time()
        rabin_karp(text_c, pattern_c, matches, ctypes.byref(count))
        end_time = time.time()
        RKtime += end_time - start_time
        rk_matches = [matches[i] for i in range(count.value)]

        '''
        start_time = time.time()
        naive_matches = naive_string_matching(text, pattern)
        end_time = time.time()
        Naivetime += end_time-start_time

        if bm_matches != naive_matches:
            bm_mismatches.append((text, pattern, bm_matches, naive_matches))
        elif rk_matches != naive_matches:
            rk_mismatches.append((text, pattern, rk_matches, naive_matches))
        '''
        Progress += 1
        percentage = Progress*100/test_cases
        if (percentage % 10 == 0 and percentage != last_percentage):
            last_percentage = percentage
            print("Progress: " + str(percentage) + "'%' completed\n")

    print("Avg boyer_moore time: \n" + str(BMtime/test_cases))
    print("Avg rabin_karp time: \n" + str(RKtime/test_cases))
    #print("Avg Naive time: \n" + str(Naivetime/test_cases))
    return bm_mismatches, rk_mismatches

bm_mismatches, rk_mismatches = super_test_run()

if bm_mismatches:
    bm_mismatch_df = pd.DataFrame(bm_mismatches, columns=["Text", "Pattern", "Boyer-Moore Matches", "Naive Matches"])
    bm_mismatch_df.to_csv("./mismatch/bm_mismatches.csv", index=False)
    print("Mismatch cases saved to bm_mismatches.csv")
elif rk_mismatches:
    rk_mismatch_df = pd.DataFrame(rk_mismatches, columns=["Text", "Pattern", "Rabin-Karp Matches", "Naive Matches"])
    rk_mismatch_df.to_csv("./mismatch/rk_mismatches.csv", index=False)
    print("Mismatch cases saved to rk_mismatches.csv")
else:
    print("All tests passed successfully!")